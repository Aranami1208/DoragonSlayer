// -----------------------------------------------------------------------
// 
// Ｆｂｘスキンメッシュ　カスケードシャドウマッピングのシェーダー
//                                                              2021.6.11
// 
//  (フォグの処理が入る)
//  (視差マッピング)       2021.5.27
//                                                  FbxSkinShadowMap.hlsli
// -----------------------------------------------------------------------
#define MAX_CASCADE_SHADOW 2
#define CASCADE_SPLIT      30
#define MAX_BONE 255

// グローバル変数	
Texture2D g_Texture: register(t0);             // テクスチャーは レジスターt(n)
Texture2D g_NormalTexture : register(t1);      // 法線マップ
Texture2D g_HeightTexture: register(t2);   // ハイトマップ                   // -- 2021.5.27
Texture2D g_SpecularTexture : register(t3);    // スペキュラーマップ
Texture2D g_DepthTexture0 : register(t4);      // 深度テクスチャー[0]
Texture2D g_DepthTexture1 : register(t5);      // 深度テクスチャー[1]

SamplerState g_samLinear : register(s0);  //サンプラーはレジスターs(n)
SamplerState g_samBorder : register(s1);
SamplerComparisonState g_samComp   : register(s2);  //サンプラー（シャドウマップエッジぼかし用）

// コンスタントバッファ
// ワールドから射影までの変換行列・他(b0)        // -- 2020.4.5
cbuffer global_0 : register(b0)
{
	matrix g_mW;          // ワールド行列
	matrix g_mWVP;        // ワールドから射影までの変換合成行列
	matrix g_mWLP[MAX_CASCADE_SHADOW];        // ワールド・”ライトビュー”・プロジェクションの合成。カスケードの段数分
	float4 g_LightDir;    // ライトの方向ベクトル
	float4 g_EyePos;      // カメラ位置
	float4 g_Diffuse;     // ディフューズ色
	float4 g_DrawInfo;     // 各種情報　y:影有り無し(2:影有り)
};

// ボーン行列(b1)
cbuffer cbBones : register(b1)
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
	float3 Tangent; // 接線      // -- 2021.6.11
	float3 Binormal; // 従法線   // -- 2021.6.11
};

// バーテックスシェーダーの入力パラメータ(頂点フォーマットと同一)
struct VS_IN
{
	float3 Pos     : POSITION;		// 頂点座標
	float3 Normal  : NORMAL;		// 法線
	float2 Tex     : TEXCOORD;		// テクセル
	uint4  Bones   : BONE_INDEX;	// ボーンのインデックス
	float4 Weights : BONE_WEIGHT;	// ウェイト
	float3 Tangent : TANGENT; // 接線
	float3 Binormal : BINORMAL; // 従法線

};

// バーテックスシェーダー出力構造体
// (ピクセルシェーダーの入力となる)
struct VS_OUTPUT_DEPTH   // 深度テクスチャーレンダー用
{
	float4 Pos : SV_POSITION;
	float4 Depth :POSITION;
	float2 Tex   : TEXCOORD0;
};

// バーテックスシェーダー出力構造体
// (ピクセルシェーダーの入力となる)
struct VS_OUTPUT         // 通常レンダー用
{
	float4 Pos : SV_POSITION;
	float3 Normal : TEXCOORD1;
	float3 EyeVector : TEXCOORD2;
	float4 PosWorld : TEXCOORD3;
	float2 Tex : TEXCOORD4;
	float4 LighViewPos[MAX_CASCADE_SHADOW] : TEXCOORD6;
	float4 lightTangentSpace : TEXCOORD10; // 接空間に変換されたライトベクトル
	float4 EyePosTangentSpace : TEXCOORD11; // 接空間に変換された視線ベクトル  // -- 2021.5.27
};

// 接空間行列の逆行列を算出
float4x4 InverseTangentMatrix(float3 tangent, float3 binormal, float3 normal)
{
	tangent = normalize(mul(tangent, (float3x3)g_mW));     // 接線をワールド変換する      // -- 2021.6.11
	binormal = normalize(mul(binormal, (float3x3)g_mW));   // 従法線をワールド変換する
	normal = normalize(mul(normal, (float3x3)g_mW));       // 法線をワールド変換する

	float4x4 mat = { float4(tangent, 0.0f),
		float4(binormal, 0.0f),
		float4(normal, 0.0f),
		{ 0.0f, 0.0f, 0.0f, 1.0f } };

	return transpose(mat); // 転置行列にする
}


// 頂点をスキニング（ボーンによる変形）するサブ関数          // -- 2021.6.11
// （バーテックスシェーダーで使用）
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

// フォグの計算をするサブ関数                   // -- 2019.4.14
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
	float Bias = 0.00005f;  // シャドウアクネ対策のバイアス値

	// ライトビューにおける位置のXY座標をテクスチャ座標に変換
	float2 shadowMapTexCoord;
	shadowMapTexCoord.x = (1.0f + LighViewPos.x / LighViewPos.w) * 0.5f;
	shadowMapTexCoord.y = (1.0f - LighViewPos.y / LighViewPos.w) * 0.5f;

	// ライト視点によるZ値の算出
	float LightLength = LighViewPos.z / LighViewPos.w;

	// シャドウマップテクスチャのZ値を得る
	float shadowMap_Z;
	// カスケード番号によってソフトエッジを切り替える
	if (Cn == 0)   // カスケードの要素番号で深度テクスチャを選択する
	{
		// 深度テクスチャ[0] 近視点の場合はエッジをソフトにする（ぼかす）
		// （シャドーアクネ対策として深度オフセットにバイアス値を追加する）
		shadowMap_Z = g_DepthTexture0.SampleCmp(g_samComp, shadowMapTexCoord, LightLength - Bias).x;
		Color.rgb *= lerp(0.5f, 1.0f, shadowMap_Z);
	}
	else {
		// 深度テクスチャ[1] 遠視点の場合は従来通りエッジをシャープにする。遠方なので目立たない
		// 理由は、エッジぼかし計算でシャドウアクネが発生するため
		shadowMap_Z = g_DepthTexture1.Sample(g_samBorder, shadowMapTexCoord).x;
		// シャドウマップのZ値と比較(ライトビューでの長さが短い（ライトビューでは遮蔽物がある))
		if (shadowMap_Z + Bias < LightLength) // ライトビューでの長さが短い（シャドーアクネ対策として深度オフセットにバイアス値を追加する）
		{
			Color.rgb /= 2; // 影の部分を定数値で暗くする
		}
	}

	return Color;
}

// ---------------------------------------------------------
// 
// シャドウマップテクスチャへの描画処理
// 
// ---------------------------------------------------------

//
// 深度テクスチャー用バーテックスシェーダー
//
VS_OUTPUT_DEPTH VS_Depth(VS_IN In)
{
	VS_OUTPUT_DEPTH output = (VS_OUTPUT_DEPTH)0;

	Skin vSkinned = SkinVert(In);		// スキニング

	output.Pos = mul(vSkinned.Pos4, g_mWVP);  // 頂点をワールド・ライトビュー・ライトプロジェクション変換する
	output.Depth = output.Pos;

	output.Tex = In.Tex;

	return output;
}
//
// 深度テクスチャー用ピクセルシェーダー
//
float4 PS_Depth(VS_OUTPUT_DEPTH In) : SV_Target
{
	float4 Color;

	uint width, height;
	g_Texture.GetDimensions(width, height);  // ディフューズテクスチャのサイズを得る  // -- 2020.12.15

	float4 texColor;
	if (width == 0)  // ディフューズテクスチャがないとき  // -- 2020.12.15
	{
		texColor = g_MatDiffuse;      // -- 2020.12.15
	}
	else {
		texColor = g_Texture.Sample(g_samLinear, In.Tex);
	}

	// 透明色の処理                            // -- 2020.9.13
	float Alpha = texColor.a - (1 - g_Diffuse.a);
	if (Alpha <= 0.0f)  discard;  // 透明色のところは切り捨てる  // -- 2020.9.13

	Color = In.Depth.z / In.Depth.w;

	return Color;
}

// ---------------------------------------------------------
// 
// シャドウマップテクスチャを参照しながら
// 　　　　　　　　　　　　通常描画を行う処理
// 
// ---------------------------------------------------------
//
// バーテックスシェーダー
//
VS_OUTPUT VS( VS_IN In )
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4x4 inverseTangentMat;

	Skin vSkinned = SkinVert(In);		// スキニング

	output.Pos = mul(vSkinned.Pos4, g_mWVP);  // 頂点をワールド・ビュー・プロジェクション変換する
	output.Normal = mul(vSkinned.Normal, (float3x3)g_mW);     // 法線をワールド変換する
	output.Normal = normalize(output.Normal);     // 法線を正規化する

	// 接空間行列の逆行列を算出
	inverseTangentMat = InverseTangentMatrix(normalize(vSkinned.Tangent), normalize(vSkinned.Binormal), normalize(vSkinned.Normal));   // -- 2021.6.11

	// ライトベクトルを接空間上に変換
	output.lightTangentSpace = mul(float4(g_LightDir.xyz, 1.0), inverseTangentMat);

	//視線ベクトル　ワールド空間上での頂点から視点へ向かうベクトル
	output.PosWorld = mul(vSkinned.Pos4, g_mW);
	output.EyeVector = normalize(g_EyePos.xyz - output.PosWorld.xyz);

	// 視線ベクトルを接空間上に変換                   // -- 2021.5.27
	output.EyePosTangentSpace = mul(g_EyePos - output.PosWorld, inverseTangentMat);

	output.Tex = In.Tex;     // テクスチャ座標はそのまま出力

	//ライトビューにおける位置(変換後)
	for (uint i = 0; i < MAX_CASCADE_SHADOW; i++)
	{
		output.LighViewPos[i] = mul(vSkinned.Pos4, g_mWLP[i]);
	}

	return output;
}

//
// ピクセルシェーダー
//
float4 PS(VS_OUTPUT In) : SV_Target
{
	float4 Color;
	uint width, height;
	g_Texture.GetDimensions(width, height);  // ディフューズテクスチャのサイズを得る  // -- 2020.12.15

	// 視差（Parallax）マッピング                   // -- 2021.5.27
	float4 texHeight = g_HeightTexture.Sample(g_samLinear, In.Tex);  // ハイトマップ
	if (texHeight.w == 1.0f)
	{
		float scale = 0.02f;           // 視差スケール
		float h = (texHeight.x + texHeight.y + texHeight.z) / 3 * scale;
		In.Tex = In.Tex + h * normalize(In.EyePosTangentSpace).xy;    // 視差によってテクスチャ座標をずらす
	}

	// テクスチャカラー
	float4 texColor;
	float4 texSpecular;

	if (width == 0)  // ディフューズテクスチャがないとき  // -- 2020.12.15
	{
		texColor = g_MatDiffuse;      // -- 2020.12.15
		texSpecular = g_MatSpecular;      // -- 2020.12.15
	}
	else {
		texColor = g_Texture.Sample(g_samLinear, In.Tex);
		texSpecular = g_SpecularTexture.Sample(g_samLinear, In.Tex);
	}
	float4 texNormal = g_NormalTexture.Sample(g_samLinear, In.Tex);

	// 光源の方向が設定されているかどうかのチェック
	if (g_LightDir.x == 0 && g_LightDir.y == 0 && g_LightDir.z == 0)
	{
		// 光の方向が0,0,0の場合は光の計算をしないで描画
		Color.rgb = texColor.rgb * g_Diffuse.rgb;
		Color.a = texColor.a - (1 - g_Diffuse.a);
	}
	else {
		// 光の方向が設定されている場合
		if (texNormal.w == 1.0f)   // 法線マップがあるかどうかチェック
		{
			// 法線マップがある場合は法線マッピング付きのライティング
			// 法線マッピングの明るさ計算
			float4 normalVector = normalize(2.0 * texNormal - 1.0f);  // ベクトルへ変換(法線マップの色は0～1.0。これを-1.0～1.0のベクトル範囲にする)
			float  NMbright = max(0.0f, dot(normalVector, normalize(In.lightTangentSpace))) * 0.9f + 0.5f;  // ライトの向きと法線マップの法線とで明度算出

			// 光源の方向計算
			float halflamb = max(0, dot(normalize(In.Normal), g_LightDir.xyz));
			halflamb = halflamb * 0.7f + 0.7f;	// ちょっと明るめに
			Color.rgb = texColor.rgb * NMbright * halflamb * g_Diffuse.rgb;

			// スペキュラの計算
			Color.rgb = CalcSpecular(Color, texSpecular, In.PosWorld, In.Normal, NMbright).rgb;

			// 影の計算処理
			if( g_DrawInfo.y == 2 )
			{
				// カメラからの距離によって、使用するカスケード番号を決定する
				uint Cn;
				if (In.Pos.w < CASCADE_SPLIT)
				{
					Cn = 0;
				}
				else {
					Cn = MAX_CASCADE_SHADOW - 1;
				}
				Color = CalcShadow( Color, In.LighViewPos[Cn], Cn);
			}

			// 透明色の処理
			Color.a = texColor.a - (1 - g_Diffuse.a);
		}
		else {
			// 法線マップがセットされていない場合はハーフランバートで描画
			float halflamb = dot(g_LightDir.xyz, In.Normal);
			//halflamb = halflamb * 0.5f + 0.8f;	// ちょっと明るめに
			halflamb = halflamb * 0.7f + 0.6f;	// ちょっと明るめに      // -- 2021.6.11
			Color.rgb = texColor.rgb * halflamb * g_Diffuse.rgb;

			// スペキュラの計算
			Color.rgb = CalcSpecular(Color, texSpecular, In.PosWorld, In.Normal, 1).rgb;

			// 影の計算処理
			if (g_DrawInfo.y == 2)
			{
				// カメラからの距離によって、使用するカスケード番号を決定する
				uint Cn;
				if (In.Pos.w < CASCADE_SPLIT)
				{
					Cn = 0;
				}
				else {
					Cn = MAX_CASCADE_SHADOW - 1;
				}
				Color = CalcShadow(Color, In.LighViewPos[Cn], Cn);
			}

			// 透明色の処理
			Color.a = texColor.a - (1 - g_Diffuse.a);
		}
	}

	// フォグの計算
	Color = CalcFog(Color, In.Pos, In.PosWorld);   // -- 2019.4.14

	return saturate(Color);  // 最終出力値を0～１の範囲に制限する
}
