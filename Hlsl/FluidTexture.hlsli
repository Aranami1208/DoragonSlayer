// -----------------------------------------------------------------------
// 
// 流体・テクスチャのシェーダー
// 
//                                                              2021.1.11
//                                                     FluidTexture.hlsli
// -----------------------------------------------------------------------

// グローバル
cbuffer global
{
	matrix g_mW;          // ワールド行列
	matrix g_mWVP;        // ワールドから射影までの変換行列
	float4 g_vLightDir;   // ライトの方向ベクトル
	float4 g_EyePos;      // 視点位置
	float4 g_vOption;     // オプション(x:表示立方体の大きさ　y:色コード　z:なし　w:発生ランダム率)
	float4 g_DrawInfo;    // 各種情報　(使っていない)
};

// 構造体
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float4 LocalPos : POSITION;
};

//
// バーテックスシェーダー
//
VS_OUTPUT VS( float3 Pos : POSITION)
{
	VS_OUTPUT output = (VS_OUTPUT)0;
	float4 Pos4 = float4(Pos, 1);
	output.Pos = mul(Pos4, g_mWVP);

	// 頂点のローカル座標値を出力する
	output.LocalPos.xyz = Pos4.xyz / g_vOption.x;  // 一辺の大きさを正規化する
	output.LocalPos.w = Pos4.w;

	return output;
}

//
// ピクセルシェーダー
//
float4 PS( VS_OUTPUT input ) : SV_Target
{
	// 表示位置のローカル座標値を出力する（始点または終点となる）
	float4 ret = input.LocalPos;
	return ret;
}