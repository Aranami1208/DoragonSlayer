// -----------------------------------------------------------------------
// 
// スキンメッシュ　カスケードシャドウマッピング　ディスプレースメントマッピングのシェーダー
//                                                              2021.6.11
//  (フォグの処理が入る)
//                                             DisplaceSkinShadowMap.hlsli
// -----------------------------------------------------------------------
#define MAX_CASCADE_SHADOW 2
#define CASCADE_SPLIT      30
#define MAX_BONE 255

// グローバル
Texture2D g_Texture: register(t0);         // テクスチャーは レジスターt(n)
Texture2D g_NormalTexture: register(t1);   // 法線マップ
Texture2D g_HeightTexture: register(t2);   // ハイトマップ
Texture2D g_SpecularTexture: register(t3);   // スペキュラーマップ
Texture2D g_DepthTexture0 : register(t4);      // 深度テクスチャー
Texture2D g_DepthTexture1 : register(t5);      // 深度テクスチャー

SamplerState g_samLinear : register(s0);   // サンプラーはレジスターs(n)
SamplerState g_samBorder : register(s1);  //サンプラー（シャドウマップ用）

// グローバル
// ワールドから射影までの変換行列・他(b0)
cbuffer global_0 : register(b0)        // -- 2020.4.5
{
	matrix g_mW;         // ワールド行列
	matrix g_mWVP;       // ワールドから射影までの変換合成行列
	matrix g_mWLP[MAX_CASCADE_SHADOW];       // ワールド・”ライトビュー”・プロジェクションの合成。カスケードの段数分
	float4 g_LightDir;   // ライトの方向ベクトル
	float4 g_EyePos;     // カメラ位置
	float4 g_Diffuse;    // ディフューズ色
	float4 g_DrawInfo;   // 各種情報(使っていない)   // -- 2020.12.15
};
// ディスプレースメントマッピング用の各種データを渡す  // -- 2020.1.24
cbuffer global_1: register(b1)
{
	float3 g_EyePosInv;     // モデルから見た視点位置
	float  g_fMinDistance;  // ポリゴン分割の最小距離
	float  g_fMaxDistance;  // ポリゴン分割の最大小距離
	int    g_iMaxDevide;    // 分割最大数
	float2 g_Height;        // ディスプレースメントマッピング時の盛り上げ高さ
	float4 g_WaveMove;      // 波の移動量(波の処理時のみ)
	float4 g_Specular;      // 鏡面反射(波の処理時のみ)
};

// ボーン行列(b2)
cbuffer cbBones : register(b2)
{
	matrix BoneFramePose[MAX_BONE];		// 指定フレームでのメッシュに影響を与える全ての骨のポーズ行列
};

// マテリアルカラーのコンスタントバッファ
cbuffer global : register(b3)               // -- 2020.12.15
{
	float4 g_MatDiffuse = float4(0.5, 0.5, 0.5, 1);   // ディフューズ色	
	float4 g_MatSpecular = float4(0, 0, 0, 0);        // スペキュラ色
};

//  コンスタントバッファ フォグ用        // -- 2019.4.14
cbuffer cbFog:register(b4)
{
	float4 g_vFogInfo = float4(0, 0, 0, 0);      // x:フォグ有無(0:無し 1:指数フォグ 2:グランドフォグ)  y:フォグ量  z: フォグ密度  w:フォグ高さ
	float4 g_vFogColor = float4(0, 0, 0, 0);     // フォグの色
};

// スキニング後の頂点・法線が入る
struct Skin
{
	float4 Pos4;
	float3 Normal;
	float3 Tangent; // 接線      //-- 2021.6.11
	float3 Binormal; // 従法線   //-- 2021.6.11
};

// バーテックスシェーダーの入力パラメータ(頂点フォーマットと同一)
struct VS_IN
{
	float3 Pos      : POSITION;		// 頂点座標
	float3 Normal   : NORMAL;		// 法線
	float2 Tex      : TEXCOORD;		// テクセル
	uint4  Bones    : BONE_INDEX;	// ボーンのインデックス
	float4 Weights  : BONE_WEIGHT;	// ウェイト
	float3 Tangent  : TANGENT;      // 接線
	float3 Binormal : BINORMAL;     // 従法線
};

struct VS_OUTPUT
{
	float3 pos    : POSITION;
	float2 uv     : TEXCOORD0;
	float3 normal : NORMAL;
};

struct HS_CONSTANT_OUTPUT
{
	float factor[3]  : SV_TessFactor;
	float inner_factor : SV_InsideTessFactor;
};

struct HS_OUTPUT
{
	float3 pos    : POSITION;
	float2 uv     : TEXCOORD0;
	float3 normal : NORMAL;    // -- 2018.4.16
};

struct DS_OUTPUT
{
	float4 pos : SV_POSITION;
	//float3 Light : TEXCOORD0;
	float3 EyeVector : TEXCOORD1;
	float4 PosWorld : TEXCOORD2;
	float2 uv:TEXCOORD3;
	float4 LighViewPos[MAX_CASCADE_SHADOW] : TEXCOORD4;
	float3 Normal : NORMAL;    // -- 2020.1.15
};

struct DS_OUTPUT_DEPTH
{
	float4 pos : SV_POSITION;
	float4 Depth : POSITION;
	float2 uv:TEXCOORD0;
	float3 normal : NORMAL;    // -- 2020.1.15
};

//
// 頂点をスキニング（ボーンによる変形）するサブ関数    // -- 2021.6.11
// （バーテックスシェーダーで使用）
//
Skin SkinVert(VS_IN In)
{
	Skin Out;
	Out.Pos4 = float4(0.0f, 0.0f, 0.0f, 0.0f);
	Out.Normal = float3(0.0f, 0.0f, 0.0f);
	Out.Tangent = float3(0.0f, 0.0f, 0.0f);
	Out.Binormal = float3(0.0f, 0.0f, 0.0f);

	float4 pos4 = float4(In.Pos, 1);
	float3 normal = In.Normal;
	float3 tangent = In.Tangent;
	float3 binormal = In.Binormal;


	// ボーン0
	uint iBone = In.Bones.x;
	float fWeight = In.Weights.x;
	matrix m = BoneFramePose[iBone];
	Out.Pos4 += fWeight * mul(pos4, m);
	Out.Normal += fWeight * mul(normal, (float3x3)m);
	Out.Tangent += fWeight * mul(tangent, (float3x3)m);
	Out.Binormal += fWeight * mul(binormal, (float3x3)m);
	// ボーン1
	iBone = In.Bones.y;
	fWeight = In.Weights.y;
	m = BoneFramePose[iBone];
	Out.Pos4 += fWeight * mul(pos4, m);
	Out.Normal += fWeight * mul(normal, (float3x3)m);
	Out.Tangent += fWeight * mul(tangent, (float3x3)m);
	Out.Binormal += fWeight * mul(binormal, (float3x3)m);
	// ボーン2
	iBone = In.Bones.z;
	fWeight = In.Weights.z;
	m = BoneFramePose[iBone];
	Out.Pos4 += fWeight * mul(pos4, m);
	Out.Normal += fWeight * mul(normal, (float3x3)m);
	Out.Tangent += fWeight * mul(tangent, (float3x3)m);
	Out.Binormal += fWeight * mul(binormal, (float3x3)m);
	// ボーン3
	iBone = In.Bones.w;
	fWeight = In.Weights.w;
	m = BoneFramePose[iBone];
	Out.Pos4 += fWeight * mul(pos4, m);
	Out.Normal += fWeight * mul(normal, (float3x3)m);
	Out.Tangent += fWeight * mul(tangent, (float3x3)m);
	Out.Binormal += fWeight * mul(binormal, (float3x3)m);

	return Out;
}

// 
// フォグの計算をするサブ関数                   // -- 2019.4.14
// 
float4 CalcFog(float4 color, float4 Pos, float4 PosWorld)
{
	// x:フォグ有無が0の時と透明色の時には、フォグを掛けない
	if (g_vFogInfo.x == 0 || color.a < 0.001f)
	{
		;
	}
	else {
		if (g_vFogInfo.x == 1)    // フォグの種類
		{
			// フォグ処理(指数フォグ) ----------------------------------
			float d = Pos.z*Pos.w;          // Ｚ深度（視点からの距離）
			float density = g_vFogInfo.z;   // フォグ密度
			float e = 2.71828f;             // 自然対数の低
			float f = pow(e, -d*density);   // フォグファクター  0に近いほどフォグが濃い
			f *= g_vFogInfo.y;              // フォグファクターにフォグ量(指数)を掛ける
			f = saturate(f);
			color = saturate(f*color + (1 - f)*g_vFogColor);  // フォグの計算
		}
		else {
			// フォグ処理(グランドフォグ) ------------------------------
			// 指数フォグファクタｆを求める
			float d = Pos.z*Pos.w;          // Ｚ深度（視点からの距離）
			float density = g_vFogInfo.z;   // フォグ密度
			float e = 2.71828f;             // 自然対数の低
			float f = pow(e, -d*density);   // フォグファクター  0に近いほどフォグが濃い

			// グランドフォグファクタｆhを求める
			PosWorld /= PosWorld.w;
			float y = PosWorld.y;           // 絶対座標での高さ
			float h = g_vFogInfo.w;         // 基準の高さ
			float fh = saturate(y / h);     // フォグファクター  [0,1]の範囲にクランプ

			// 指数フォグとグランドフォグを合成する
			f = f + fh;                     // 指数フォグにグランドフォグファクタを加える
			f *= g_vFogInfo.y;              // フォグファクターにフォグ量(指数)を掛ける
			f = saturate(f);
			color = saturate(f*color + (1 - f)*g_vFogColor);  // フォグの計算
		}
	}

	return color;
}

//
// スペキュラの計算
//
float4 CalcSpecular(float4 Color, float4 texSpecular, float4 PosWorld, float3 Normal, float NMbright)
{
	if (texSpecular.w == 1.0f) // スペキュラマップがあるか
	{
		// ハーフベクトルを使ったスペキュラの計算
		float3 ViewDir = normalize( g_EyePos.xyz - PosWorld.xyz);  // PosWorldからg_EyePosへ向かう視線ベクトル（正規化）
		float3 HalfVec = normalize( g_LightDir.xyz + ViewDir);     // ハーフベクトル（視線ベクトルと光源ベククトルの中間ベクトル）（正規化）

		float Gloss = 4;   // 光沢度係数。指数値が大きいほどスペキュラが狭く強くなる。ここでは4としている。
		float Spec = 2.5f * pow( saturate(dot(HalfVec, Normal)), Gloss );   // スペキュラの計算。後でスペキュラマップ値を掛けるため2.5倍にしておく
		Color.rgb = saturate(Color.rgb + Spec * texSpecular.rgb * NMbright);   // スペキュラにスペキュラマップ値を掛けてカラーに加える。

	}
	return Color;
}

// 
// 影の計算処理
// 
float4 CalcShadow(float4 Color, float4 LighViewPos, uint Cn)
{

	// ライトビューにおける位置のXY座標をテクスチャ座標に変換
	float2 shadowMapTexCoord;
	shadowMapTexCoord.x = (1.0f + LighViewPos.x / LighViewPos.w) * 0.5f;
	shadowMapTexCoord.y = (1.0f - LighViewPos.y / LighViewPos.w) * 0.5f;

	// シャドウマップテクスチャのZ値を得る
	float shadowMap_Z;
	if (Cn == 0)   // カスケードの要素番号で深度テクスチャを選択する
	{
		shadowMap_Z = g_DepthTexture0.Sample(g_samBorder, shadowMapTexCoord).x;
	}
	else {
		shadowMap_Z = g_DepthTexture1.Sample(g_samBorder, shadowMapTexCoord).x;
	}

	// ライト視点によるZ値の算出
	float LightLength = LighViewPos.z / LighViewPos.w;

	// シャドウマップのZ値と比較(ライトビューでの長さが短い（ライトビューでは遮蔽物がある))
	//if(shadowMap_Z + 0.0000005 < LightLength) // ライトビューでの長さが短い（シャドーアクネ対策として深度オフセットに値を追加する）
	if(shadowMap_Z + 0.00005 < LightLength) // ライトビューでの長さが短い（シャドーアクネ対策として深度オフセットに値を追加する）
	//if(shadowMap_Z < LightLength) // ライトビューでの長さが短い（シャドーアクネが発生する）
	{
		Color.rgb /= 2; // 影の部分を定数値で暗くする
	}
	return Color;
}


// ----------------------------------------------------------------------------
//
// バーテックスシェーダー
//
// ----------------------------------------------------------------------------
VS_OUTPUT VS( VS_IN In )
{
	VS_OUTPUT Out;

	Skin vSkinned = SkinVert(In);   // スキニング

	Out.pos   = vSkinned.Pos4.xyz;  // スキニング後の頂点
	Out.normal= vSkinned.Normal;    // スキニング後の法線
	Out.uv    = In.Tex;             // テクスチャ座標はそのまま

	return Out;
}

// ----------------------------------------------------------------------------
//
// ハルシェーダーは、パッチ定数関数とコントロールポイントシェーダの２つのフェーズからなっている
//
// パッチとは	プリミティブ・トポロジー IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST)
// などで指定されるもので、この例ではコントロールポイントを３つ持ったパッチということになる。
// なお、コントロールポイントは、最大３２まで指定できる。
//
// ----------------------------------------------------------------------------
// ハルシェーダーのパッチ定数フェーズ(パッチ定数関数)
// 
// この関数は、指定パッチごとに１回起動されます。（ここでは、コントロールポイント３つ毎に起動される）
// この時 ip の添字はコントロールポイント（頂点）の０番から２番となります
// 処理内容は、視点から頂点までの距離に応じて分割数を決定します
// ３頂点それぞれの分割数を戻り値で返します
// 
//  引数  InputPatch<VS_OUTPUT, 3> ip   頂点（バーテックス）シェーダーから指定パッチ（ここでは３回）数の値を受け取る
//        uint pid                      パッチのＩＤ
// 
// ----------------------------------------------------------------------------
HS_CONSTANT_OUTPUT HSConstant( InputPatch<VS_OUTPUT, 3> ip, uint pid : SV_PrimitiveID )
{
	HS_CONSTANT_OUTPUT Out;
	float divide = 0;

	float distance = length(g_EyePosInv - ip[0].pos);   // 頂点０番と視点との距離を測る
	if(distance < g_fMinDistance) distance = g_fMinDistance;  // 最小距離と最大距離の間になるようにクランプ
	if(distance > g_fMaxDistance) distance = g_fMaxDistance;

	// xは、距離が最大距離と同じ場合は1.0に、最小距離と同じ場合は0.0に、その中間の場合は0.0～1.0になる。
	float x = (distance - g_fMinDistance) / (g_fMaxDistance - g_fMinDistance);
	// 上記の計算で、xは遠近が逆となっているため、1の補数(1-x)をとる。
	divide = (1-x) * g_iMaxDevide + 1;   // 分割数を計算する

	// 分割数をfactorに設定する
	Out.factor[0] = divide;
	Out.factor[1] = divide;
	Out.factor[2] = divide;

	Out.inner_factor = divide;

	return Out;
}
// ----------------------------------------------------------------------------
// ハルシェーダーのコントロールポイントフェーズ(コントロールポイントシェーダ)
// （こちらがハルシェーダーのシェーダー名となる）
// 
// この関数は、コントロールポイント（頂点）ごとに１回起動されます。
// 
//  属性
//  domain("tri")                    ドメインタイプの指定（qiad:四角ドメイン tri:三角ドメイン isoline:線ドメイン）
//  partitioning("integer")          分割数は整数か浮動小数点数かを指定
//  outputtopology("triangle_ccw")    最終的な分割後のポリゴンについて（triangle_cw:時計回り triangle_ccw:反時計回り line:線ポリゴン)
//  outputcontrolpoints(3)           ハルシェーダーのコントロール ポイントフェーズがコールされる回数（ここでは３回）
//  patchconstantfunc("HSConstant")  対応するハルシェーダーのパッチ定数関数名
// 
//  引数  InputPatch<VS_OUTPUT, 3> ip   頂点（バーテックス）シェーダーから指定パッチ（ここでは３回）数の値を受け取る
//        uint cpid                     コントロールポイントの番号（添字）
//        uint pid                      パッチのＩＤ
// 
// ----------------------------------------------------------------------------
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("HSConstant")]
HS_OUTPUT HS( InputPatch<VS_OUTPUT, 3> ip, uint cpid : SV_OutputControlPointID, uint pid : SV_PrimitiveID )
{
	HS_OUTPUT Out;
	Out.pos    = ip[cpid].pos;
	Out.uv     = ip[cpid].uv;
	Out.normal = ip[cpid].normal;

	return Out;
}
// ----------------------------------------------------------------------------
// 
// ドメインシェーダー
// 
//  ドメインシェーダーは、テセレーターの分割によって生成された頂点毎に起動します
//  ドメインシェーダーは、分割後の実際の頂点座標を計算します。 
//  三角形重心位置（DomLoc)は分割後の 0 ～ 1 の範囲内の補完係数です。
//  コントロールポイントの座標から分割後の頂点座標を計算します。 
//  Hightテクスチャーを使用して、頂点の高さを計算します。
// 
//  属性
//  domain("tri")                           ドメインタイプの指定（qiad:四角ドメイン tri:三角ドメイン isoline:線ドメイン）
// 
//  引数  HS_CONSTANT_OUTPUT In             ハルシェーダー定数関数の出力値を受け取る
//        float3 DomLoc                     当該頂点の位置情報（三角形の重心位置）。生成された頂点のパッチ上の位置を求めるために使用する
//        OutputPatch<HS_OUTPUT, 3> patch   全てのコントロールポイントへアクセス可能
// 
// ----------------------------------------------------------------------------
[domain("tri")]
DS_OUTPUT DS( HS_CONSTANT_OUTPUT In, float3 DomLoc : SV_DomaInLocation, const OutputPatch<HS_OUTPUT, 3> patch )
{
	DS_OUTPUT Out;

	// ３つのコントロールポイントのテクスチャ座標に三角形重心位置（x,y,z)を掛けて、実際のテクスチャ座標を求めている。
	// （このとき、0番目のテクスチャ座標の重心位置がDomLoc.xに、1番目がDomLoc.yに、2番目がDomLoc.zに対応する）
	Out.uv = patch[0].uv*DomLoc.x + patch[1].uv*DomLoc.y + patch[2].uv*DomLoc.z;

	// Heightテクスチャーを使用して、先ほど求めたテクスチャ座標位置の頂点値を求める
	float3 texHeight = g_HeightTexture.SampleLevel(g_samLinear, Out.uv, 0 ).xyz;

	// ３つのコントロールポイントの法線座標に三角形重心位置（x,y,z)を掛けて、法線座標を求める     // -- 2020.1.15
	// （このとき、0番目の法線座標の重心位置がDomLoc.xに、1番目がDomLoc.yに、2番目がDomLoc.zに対応する）
	Out.Normal = normalize( patch[0].normal*DomLoc.x + patch[1].normal*DomLoc.y + patch[2].normal*DomLoc.z);

	// ３つのコントロールポイントの座標に三角形重心位置（x,y,z)を掛けて、頂点座標を求めている。
	// （このとき、0番目の頂点の重心位置がDomLoc.xに、1番目がDomLoc.yに、2番目がDomLoc.zに対応する）
	float3 pos = patch[0].pos*DomLoc.x + patch[1].pos*DomLoc.y + patch[2].pos*DomLoc.z;
	if (DomLoc.x != 0 && DomLoc.y != 0 && DomLoc.z != 0)   // 重心位置が分割前の頂点位置のときは、高くしない。ポリゴンの切れ目が裂けないため
	{
		pos = pos + ( Out.Normal * ((texHeight.x + texHeight.y + texHeight.z) / 3 * g_Height.y) );  // 頂点の高さを加える(法線方向に)
	}
	float4 pos4 = float4(pos, 1);


	Out.pos = mul(pos4, g_mWVP);  // ワールド・ビュー・プロジェクション変換をする

	// ライト方向
	//Out.Light = normalize(g_LightDir.xyz);

	// 法線のワールド変換をする                               // -- 2021.6.11
	Out.Normal = normalize(mul(Out.Normal, (float3x3)g_mW));

	// 視線ベクトル　ワールド空間上での頂点から視点へ向かうベクトル
	Out.PosWorld = mul( pos4, g_mW );
    Out.EyeVector = normalize( g_EyePosInv.xyz - Out.PosWorld.xyz );

	// ライトビューにおける位置(変換後)
	for (uint i = 0; i < MAX_CASCADE_SHADOW; i++)
	{
		Out.LighViewPos[i] = mul(pos4, g_mWLP[i]);
	}

	return Out;
}
// ----------------------------------------------------------------------------
//
// ピクセルシェーダー
//
// ----------------------------------------------------------------------------
float4 PS( DS_OUTPUT In ) : SV_Target
{
	float4 Color;      // 最終出力値
	uint width, height;
	g_Texture.GetDimensions(width, height);  // ディフューズテクスチャのサイズを得る  // -- 2020.12.15

	// テクスチャカラー
	float4 texCol;
	float4 texSpecular;

	if (width == 0)  // ディフューズテクスチャがないとき  // -- 2020.12.15
	{
		texCol = g_MatDiffuse;      // -- 2020.12.15
		texSpecular = g_MatSpecular;      // -- 2020.12.15
	}
	else {
		texCol = g_Texture.Sample(g_samLinear, In.uv);  // ディフューズテクスチャ
		texSpecular = g_SpecularTexture.SampleLevel(g_samLinear, In.uv, 0);  // スペキュラテクスチャ
	}
	float4 texNM  = g_NormalTexture.SampleLevel(g_samLinear, In.uv, 0);  // ノーマルテクスチャ

	// 光源の方向が設定されているかどうかのチェック
	if (g_LightDir.x == 0 && g_LightDir.y == 0 && g_LightDir.z == 0)
	{
		// 光の方向が0,0,0の場合は光の計算をしないで描画
		Color.rgb = texCol.rgb * g_Diffuse.rgb;
		Color.a = texCol.a - (1 - g_Diffuse.a);
	}
	else {
		// 光の方向が設定されている場合
		if (texNM.w == 1.0f)   // 法線マップがあるかどうかチェック
		{
			// 法線マップがある場合は法線マッピング付きのライティング
			// 法線マッピングの明るさ計算
			float3 Normal = -(texNM.xyz * 2 - 1);  // ノーマルマップテクスチャから-法線を得る
			float  NMbright = max(0, dot(Normal, g_LightDir.xyz)) * 0.9f + 0.5f;  // ノーマルマップの光計算

			// 光源の方向計算
			float halflamb = max(0, dot(normalize(In.Normal), g_LightDir.xyz));  // ランバートの余弦則
			halflamb = halflamb * 0.7f + 0.7f;   // ハーフランバート。ちょっと明るめに
			Color.rgb = texCol.rgb * NMbright * halflamb * g_Diffuse.rgb;  // テクスチャ色にノーマルマップとハーフランバート、拡散反射色を重ねる

			// スペキュラの計算
			Color.rgb = CalcSpecular(Color, texSpecular, In.PosWorld, In.Normal, NMbright).rgb;

			// 影の計算処理
			if (g_DrawInfo.y == 2)
			{
				// カメラからの距離によって、使用するカスケード番号を決定する
				uint Cn;
				if (In.pos.w < CASCADE_SPLIT)
				{
					Cn = 0;
				}
				else {
					Cn = MAX_CASCADE_SHADOW - 1;
				}
				Color = CalcShadow(Color, In.LighViewPos[Cn], Cn);
			}

			// 透明色の処理
			Color.a = texCol.a - (1 - g_Diffuse.a);   // 透明度はテクスチャ透明度に拡散反射色透明度を加味する
		}
		else {
			// 法線マップがセットされていない場合は、ハーフランバートで描画
			float halflamb = max(0, dot(normalize(In.Normal), g_LightDir.xyz));  // マイナスは0に補正
			//halflamb = halflamb * 0.5f + 0.8f;	// ちょっと明るめに
			halflamb = halflamb * 0.7f + 0.6f;	// ちょっと明るめに      // -- 2021.6.11
			Color.rgb = texCol.rgb * halflamb * g_Diffuse.rgb;

			// スペキュラの計算
			Color.rgb = CalcSpecular(Color, texSpecular, In.PosWorld, In.Normal, 1).rgb;

			// 影の計算処理
			if (g_DrawInfo.y == 2)
			{
				// カメラからの距離によって、使用するカスケード番号を決定する
				uint Cn;
				if (In.pos.w < CASCADE_SPLIT)
				{
					Cn = 0;
				}
				else {
					Cn = MAX_CASCADE_SHADOW - 1;
				}
				Color = CalcShadow(Color, In.LighViewPos[Cn], Cn);
			}

			// 透明色の処理
			Color.a = texCol.a - (1 - g_Diffuse.a);
		}
	}
	// フォグの計算
	Color = CalcFog(Color, In.pos, In.PosWorld);
	return saturate(Color);  // 最終出力値を0～１の範囲に制限する
}

// ----------------------------------------------------------------------------
// 
// ドメインシェーダー  深度テクスチャー描画用
// 
//  ドメインシェーダーは、テセレーターの分割によって生成された頂点毎に起動します
//  ドメインシェーダーは、分割後の実際の頂点座標を計算します。 
//  uv座標は分割後の 0 ～ 1 の範囲内の補完係数です。
//  コントロールポイントの座標から分割後の頂点座標を計算します。 
//  Hightテクスチャーを使用して、頂点の高さを計算します。
// 
//  属性
//  domain("tri")                           ドメインタイプの指定（qiad:四角ドメイン tri:三角ドメイン isoline:線ドメイン）
// 
//  引数  HS_CONSTANT_OUTPUT In             ハルシェーダー定数関数の出力値を受け取る
//        float3 DomLoc                         当該頂点の位置情報（相対座標）
//        OutputPatch<HS_OUTPUT, 3> patch   全てのコントロールポイントへアクセス可能
// 
// ----------------------------------------------------------------------------
[domain("tri")]
DS_OUTPUT_DEPTH DS_Depth(HS_CONSTANT_OUTPUT In, float3 DomLoc : SV_DomaInLocation, const OutputPatch<HS_OUTPUT, 3> patch)
{
	DS_OUTPUT_DEPTH Out;

	// ３つのコントロールポイントのテクスチャ座標に三角形重心位置（x,y,z)を掛けて、実際のテクスチャ座標を求めている。
	// （このとき、0番目のテクスチャ座標の重心位置がDomLoc.xに、1番目がDomLoc.yに、2番目がDomLoc.zに対応する）
	Out.uv = patch[0].uv*DomLoc.x + patch[1].uv*DomLoc.y + patch[2].uv*DomLoc.z;

	// Heightテクスチャーを使用して、先ほど求めたテクスチャ座標位置の頂点の高さを求める
	float4 texHeight = g_HeightTexture.SampleLevel(g_samLinear, Out.uv, 0);

	// ３つのコントロールポイントの法線座標に三角形重心位置（x,y,z)を掛けて、法線座標を求める     // -- 2020.1.15
	// （このとき、0番目の法線座標の重心位置がDomLoc.xに、1番目がDomLoc.yに、2番目がDomLoc.zに対応する）
	Out.normal = normalize(patch[0].normal*DomLoc.x + patch[1].normal*DomLoc.y + patch[2].normal*DomLoc.z);

	// ３つのコントロールポイントの座標に三角形重心位置（x,y,z)を掛けて、頂点座標を求めている。
	// （このとき、0番目の頂点の重心位置がDomLoc.xに、1番目がDomLoc.yに、2番目がDomLoc.zに対応する）
	float3 pos = patch[0].pos*DomLoc.x + patch[1].pos*DomLoc.y + patch[2].pos*DomLoc.z;
	if (DomLoc.x != 0 && DomLoc.y != 0 && DomLoc.z != 0)   // 重心位置が元の頂点位置のときは、高くしない。ポリゴンの切れ目が裂けないため
	{
		//pos = pos + (((patch[0].normal + patch[1].normal + patch[2].normal) / 3) * ((texHeight.x + texHeight.y + texHeight.z) / 3 * g_Height.y));  // 頂点の高さを加える(法線方向に)    // -- 2018.4.16
		pos = pos + (Out.normal * ((texHeight.x + texHeight.y + texHeight.z) / 3 * g_Height.y));  // 頂点の高さを加える(法線方向に)
	}

	float4 Pos4 = float4(pos, 1);

	Out.pos = mul(Pos4, g_mWVP);  // ワールド・ライトビュー・プロジェクション変換をする  !!!!!!!
	Out.Depth = Out.pos;

	return Out;
}


// ----------------------------------------------------------------------------
// ピクセルシェーダー  深度テクスチャー描画用
// ----------------------------------------------------------------------------
float4 PS_Depth(DS_OUTPUT_DEPTH In) : SV_Target
{
	float4 Color = float4(0,0,0,0);
	if (g_Texture.Sample(g_samLinear, In.uv).w > 0)  // 透明色でないところ
	{
		Color = In.Depth.z / In.Depth.w;
	}
	return Color;

}

