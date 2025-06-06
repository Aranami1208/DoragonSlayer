// -----------------------------------------------------------------------
// 
// ＨＤＲのシェーダー
// 
//                                                              2020.1.24
//                                                         Effect3D.hlsli
// -----------------------------------------------------------------------

//グローバル
Texture2D g_Texture: register(t0);
Texture2D g_BloomTexture: register(t1);
SamplerState g_Sampler : register(s0);

//グローバル
cbuffer global_0:register(b0)
{
	float2 g_ViewPort;  // ビューポート（スクリーン）サイズ
	float2 g_Info;      // 指定情報（x:しきい値 y:ブルームの強さ）
};
cbuffer global_1:register(b1)
{
	float4 g_OffsetWeight[15];        //  x,y:オフセット  w:ウェイト
};

//構造体
struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 UV : TEXCOORD;
};

//
// バーテックスシェーダー
//
PS_INPUT VS(float4 Pos : POSITION, float2 UV : TEXCOORD)
{
	PS_INPUT Out;

	Out.Pos = Pos;

	Out.Pos.x = (Out.Pos.x / g_ViewPort.x) * 2 - 1;
	Out.Pos.y = 1 - (Out.Pos.y / g_ViewPort.y) * 2;

	Out.UV = UV;

	return Out;
}

//
// ピクセルシェーダー ブライトテクスチャの作成	
//
float4 PS_BRIGHT(PS_INPUT In) : SV_Target
{
	float MinBright = g_Info.x; // しきい値
	float4 Color;
	float MonoColor;

	Color = float4(max(0, g_Texture.Sample(g_Sampler, In.UV) - MinBright).rgb, 1); // しきい値より明るいところを選択
	MonoColor = min(1, (Color.r + Color.g + Color.b));    // モノトーンにする。めいっぱい明るくする
	Color = float4(MonoColor, MonoColor, MonoColor, Color.a);

	return Color;
}

//
// ピクセルシェーダー ブルームテクスチャの作成
//
float4 PS_BLOOM(PS_INPUT In) : SV_Target
{
	float4 Color = 0;
	for (int i = 0; i < 15; i++)
	{
		float2 UV = In.UV + g_OffsetWeight[i].xy;
		if (UV.x < 0.0f || UV.x > 1.0f || UV.y < 0 || UV.y > 1)  // テクスチャ座標の外かどうか
		{
			;  // テクスチャ座標の外はブルームを追加しない
		}
		else {
			// 元のテクスチャにブルームを追加する
			Color += g_Texture.Sample(g_Sampler, UV) * g_OffsetWeight[i].w;
		}
	}

	return Color;
}

//
// ピクセルシェーダー 最終出力
//
float4 PS(PS_INPUT In) : SV_Target
{
	float4 Color;
	float4 texColor = g_Texture.Sample(g_Sampler, In.UV); // オリジナルの画像
	Color = texColor + g_BloomTexture.Sample(g_Sampler, In.UV) * g_Info.y; // ブルーム画像を合成する
	//Color = g_BloomTexture.Sample(g_Sampler, Input.UV);  // ブルームテクスチャのみを表示するとき
	Color.a = texColor.a;
	return Color;
}