// -----------------------------------------------------------------------
// 
// 流体・レンダリングのシェーダー
// 
//                                                              2021.1.11
//                                                      FluidRender.hlsli
// -----------------------------------------------------------------------

// グローバル
Texture2D g_texA: register(t0);
Texture2D g_texB: register(t1);
Texture3D g_tex3D: register(t2);
SamplerState g_samLinear : register(s0);

cbuffer global
{
	matrix g_mW;          // ワールド行列
	matrix g_mWVP;        // ワールドから射影までの変換行列
	float4 g_vLightDir;   // ライトの方向ベクトル
	float4 g_EyePos;      // 視点位置
	float4 g_vOption;     // オプション(x:表示立方体の大きさ　y:色コード　z:なし　w:発生ランダム率)
	float4 g_DrawInfo;    // 各種情報(使っていない)
};

// バーテックスシェーダー出力用構造体
struct VS_OUTPUT
{
	float4 Pos  : SV_POSITION;
	float4 CPos : POSITION;
};

// ピクセルシェーダー出力用の構造体
struct PS_OUTPUT
{
	float4 ColorBuffer : SV_Target;   // カラーマップ
	float  DepthBuffer : SV_Depth;    // 深度マップ
};

// 参考となるサイト
//
// http:// maverickproj.web.fc2.com/d3d11_21.html
// https: //t-pot.com/program/98_BG-Map/index.html
// https: //qpp.bitbucket.io/translation/maximizing_depth_buffer_range_and/
// https: //enginetrouble.net/2016/10/reconstructing-world-position-from-depth-2016.html
//

//
// バーテックスシェーダー
//
VS_OUTPUT VS( float3 Pos : POSITION)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 Pos4 = float4(Pos, 1);
	output.Pos  = mul(Pos4,g_mWVP);
	output.CPos = output.Pos;

	return output;
}

//
// ピクセルシェーダー
//
// ①３Ｄテクスチャをレイマーチング（行進）の手法でレンダリングする
// ②深度バッタァを書き換え、炎の外郭を深度面とする
//
PS_OUTPUT PS( VS_OUTPUT input )
{	
	PS_OUTPUT output = (PS_OUTPUT)0;

	const int Loops = 64;        // レイの行進回数（多いほど細かくなる）

	const float Stride = 1.0f / Loops;   // レイの行進の幅

	// 頂点座標からテクスチャー座標を作成
	float2 Tex =  input.CPos.xy /= input.CPos.w;

    Tex.x      =  0.5f * Tex.x + 0.5f;
    Tex.y      = -0.5f * Tex.y + 0.5f;

	float3 Front = g_texA.Sample(g_samLinear,Tex).xyz;  // レイの行進の始点
    float3 Back = g_texB.Sample(g_samLinear,Tex).xyz;   // レイの行進の終点

	float3 Dir = normalize(Back-Front);                 // レイの方向	
    float3 Step = Stride * Dir;                         // 1回の行進量
    float4 Texel3D = float4(0,0,0,0);

	float   MaxDns = 0;			// レイの中の最大密度
	float   DnsZ   = 0;			// レイの中で最初に密度が出現した位置。つまり流体の外郭
	float3  FrontStart = Front;	// レイの始点を保存
	for(int i=0; i<Loops; i++)	// レイを始点から終点に向かってStepづづ進めて最大値を求める
	{
		Texel3D = g_tex3D.Sample(g_samLinear, Front);	// ３Ｄテクスチャから値を得る
		Front += Step;									// ステップを進める

		// 密度はrのみ使用する。gbaは使用していない)
		MaxDns = max(MaxDns, Texel3D.r);				// レイの中の最大密度をMaxDnsに求める

		if (DnsZ == 0 && Texel3D.r > 0.001f)    // 最初に密度が出現した位置をDnsZにセットする
		{
			DnsZ = length(Front-FrontStart) * g_vOption.x;   // 流体の外郭のレイの始点からの位置（ｍ）
		}
	}

	// 深度バッファに書き込み
	// 
	// input.Pos : SV_POSITION の値
	//  x = レンダーターゲット内でのX座標 (例えば横幅が512ピクセルのレンダーターゲットに書き込んでいるなら、0～511の値)
	//  y = レンダーターゲット内でのY座標(例えば縦幅が512ピクセルのレンダーターゲットに書き込んでいるなら、0～511の値)
	//  z = 深度値(0.0～1.0。つまりwで除算済みの値)
	//  w = プロジェクションマトリクスで変換した後のw値そのもの(z値による拡縮率。視体積膨らみ情報)
	// 
	//output.DepthBuffer = input.Pos.z;   // 表示用立方体の表面位置の深度(従来の方法)
	output.DepthBuffer = input.Pos.z +  DnsZ / input.Pos.w * (1-input.Pos.z);   // 流体の外郭深度の設定、これでいいのか??

	// 最終的に密度が０のときは、表示ピクセルを破棄する
	if (DnsZ == 0) discard;

	// 暗いDarkColorと明るいBrightColorの2色を用意しそれらの中間色を最終色とする
	float4 DarkColor   = float4(0, 0, 0, 1);
	float4 BrightColor = float4(0, 0, 0, 1);

	// 色コードの指定によって表示する色を選択する
	switch (g_vOption.y)
	{
	case 0:
		// 炎（黒煙なし）
		DarkColor = float4(0,-1,-2,0);			// 暗い赤色(黒煙にはならない)
		BrightColor = float4(1,0.5,0.0,2);		// 明るい黄色
		break;
	case 1:
		// 炎（黒煙あり）
		DarkColor   = float4(-0.5f,-1,-1.5f,0);	// 暗い赤色（炎がすぐに黒煙になる）
		BrightColor = float4(1,0.5,0.0,2);		// 明るい黄色
		break;
	case 2:
		// 青い炎
		DarkColor   = float4(-3.0f,-2.0f,-1.0f,0.0f)/2;	// 暗い青色
		BrightColor = float4(0.5f,1.5f,1.5f,2);			// 明るい空色
		break;
	case 3:
		// 濃い煙
		DarkColor = float4(0.3f,0.3f,0.3f,0);			// グレイ
		BrightColor = float4(1.0f,1.0f,1.0f,1.0f);		// 明るい白
		break;
	case 4:
		// 薄い煙
		DarkColor = float4(0.25f,0.25f,0.25f,0);		// グレイ
		BrightColor = float4(0.4f,0.4f,0.4f,0.1f);		// 薄い白
		break;
	}

	// 色のバッファに書き込み
	output.ColorBuffer = saturate( lerp(DarkColor, BrightColor, MaxDns) );  // MaxDnsの値によって、DarkColorとBrightColorの間を線形補完lerpする	

	return output;
}