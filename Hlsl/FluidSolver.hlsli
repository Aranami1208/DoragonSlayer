// -----------------------------------------------------------------------
// 
// 流体・計算（Slover）のシェーダー
// 
//                                                              2021.1.11
//                                                      FluidSolver.hlsli
// -----------------------------------------------------------------------

// マクロ定義
#define FLUID_SIZE 64
#define FLUID_RSIZE 8
#define FLUID_TIME_STEP 0.01

#define ID(i,j,k) (i)+(j)*(FLUID_SIZE+2)+(k)*(FLUID_SIZE+2)*(FLUID_SIZE+2)

// サンプラーステート
SamplerState g_samLinear : register(s1);

//	ストラクチャードバッファフォーマット
struct Buff
{
	float f;
	float f0;
};

// 3Dテクスチャ	(密度用)
Texture3D<float4> g_rDns : register(t0);     // 読み出し用
RWTexture3D<float4> g_wDns : register(u0);   // 書き込み用

// 3Dテクスチャ	(速度用)
Texture3D<float4> g_rVlc : register(t1);
RWTexture3D<float4> g_wVlc : register(u1);

Texture3D<float4> g_rVlc_s : register(t2);
RWTexture3D<float4> g_wVlc_s : register(u2);

Texture3D<float4> g_rVlc_bs : register(t3);
RWTexture3D<float4> g_wVlc_bs : register(u3);

// ストラクチャードバッファ
RWStructuredBuffer<Buff> g_Prs : register(u3); // u3ダブり ????????
RWStructuredBuffer<Buff> g_Div : register(u4);

// 3Dテクスチャ(画像用)
RWTexture3D<float4> g_D_Tex3D : register(u5);

// コンスタントバッファ
cbuffer global
{
	float4 g_AddVoxelPos;
	float4 g_AddDensity;
	float4 g_AddVelocity;
	float4 g_Option;       //オプション(x:表示立方体の大きさ　y:色コード　z:なし　w:発生ランダム率)
}

//
// 密度と速度を指定位置に注入し、発生させる
//
[numthreads(1,1,1)]
void AddSource( uint3 id : SV_DispatchThreadID ,uint3 gid : SV_GroupID )
{
	float3 center = g_AddVoxelPos.xyz; // 指定位置
	int radius    = g_AddVoxelPos.w;   // 発生半径

	// 半径内に注入し発生させる
	for(int i=-radius;i<radius;i++)
	{
		for(int j=-radius;j<radius;j++)
		{
			for(int k=-radius;k<radius;k++)
			{
				// HLSLには乱数関数がないため、dotとexpで疑似乱数を発生させる
				float3 p = float3(i,j,k);     // i,j,k半径の値をベクトルにする
				float l = dot(p, p);          // 内積値を求める（各ベクトルメンバの２乗値の計となる）

				// expはeを底とする指数を求める関数である
				float rate = exp(-l/radius);  // 0～1の値を返す
				float3 pos = center + p;      // 位置にi,j,k半径のベクトルを足す

				// 発生場所を半円内に。発生密度をランダムに
				g_wDns[uint3(pos.x,pos.y,pos.z)] = g_rDns[uint3(pos.x,pos.y,pos.z)] + g_AddDensity*rate;

				// 発生場所を半円内に。発生速度は指定方向またはランダムに
				float  fRand = g_Option.w;  // ランダム率
				float4 vAddVlc = g_AddVelocity;
				if (g_AddVelocity.x == 0 && g_AddVelocity.z == 0)	// Ｙ方向に発生しているとき
				{
					vAddVlc.x = g_AddVelocity.y * (rate - 0.5f) * fRand * 3;
					vAddVlc.z = g_AddVelocity.y * (rate - 0.5f) * fRand * 3;
				}
				else if (g_AddVelocity.x == 0 && g_AddVelocity.y == 0)	// Ｚ方向に発生しているとき
				 {	
					vAddVlc.x = g_AddVelocity.z * (rate - 0.5f) * fRand * 3;
					vAddVlc.y = g_AddVelocity.z * (rate - 0.5f) * fRand * 3;
				}
				//g_wVlc[uint3(pos.x,pos.y,pos.z)] = g_rVlc[uint3(pos.x,pos.y,pos.z)] + vAddVlc;		// 発生速度（方向）は一定に
				g_wVlc[uint3(pos.x,pos.y,pos.z)] = g_rVlc[uint3(pos.x,pos.y,pos.z)] + vAddVlc*(1-fRand) + vAddVlc*(rate*fRand);	// 発生速度（方向）はランダム率で決まる
			}
		}
	}
}
//
// 移流密度の計算
//
[numthreads(FLUID_RSIZE,FLUID_RSIZE,FLUID_RSIZE)]
void AdvectDensity( uint3 id : SV_DispatchThreadID ,uint3 gid : SV_GroupID )
{
	// 場の端を除いたFLUID_SIZEだけ処理をする。このため添字を一つ進める
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	float3 uv = id+1;  // これから密度を処理する場所の位置
	uv.x -= g_rVlc[uint3(i,j,k)].x*FLUID_TIME_STEP;  // 速度をさかのぼって一つ前の位置
	uv.y -= g_rVlc[uint3(i,j,k)].y*FLUID_TIME_STEP;
	uv.z -= g_rVlc[uint3(i,j,k)].z*FLUID_TIME_STEP;

	uv=(uv+0.5)/(FLUID_SIZE+2);  // 位置をテクスチャのＵＶ座標に変換する
	
	// サンプリングにより値を取り出す
	// (サンプリングによって、トリリニア補完が自動的に行われる）
	g_wDns[uint3(i,j,k)]=g_rDns.SampleLevel(g_samLinear,uv,0);    // 取り出された値は、g_wDns（書き込み領域）に書き込む

	// 値を３Ｄテクスチャーに書き込む
	g_D_Tex3D[uint3(id.x,id.y,id.z)] = g_rDns[uint3(i,j,k)];      //  2019.12.6 元の内容　　取り出すときは、g_rDns(読み込み領域)から取り出す
	//g_D_Tex3D[uint3(id.x,id.y,id.z)]= g_rDns.SampleLevel(g_samLinear, uv, 0);  // 2019.12.6 変更後


	// 場の形を見せたくない場合。場の端でちょうど消えるように少し手前から減衰させる。
	float thresh = FLUID_SIZE*0.8f;
	float range = FLUID_SIZE*0.2f;
	float Disolve = 1;

	if( (int)i < FLUID_SIZE - thresh) Disolve =  min(Disolve, i / range);
	if( (int)j < FLUID_SIZE - thresh) Disolve =  min(Disolve, j / range);
	if( (int)k < FLUID_SIZE - thresh) Disolve =  min(Disolve, k / range);
	if( (int)i > thresh)        Disolve =  min(Disolve, (FLUID_SIZE-i) / range);
	if( (int)j > thresh)        Disolve =  min(Disolve, (FLUID_SIZE-j) / range);
	if( (int)k > thresh)        Disolve =  min(Disolve, (FLUID_SIZE-k) / range);
	if (Disolve < 1)
	{
		g_wDns[uint3(i,j,k)] = g_rDns.SampleLevel(g_samLinear,uv,0)*Disolve;
	}

}
//
// 圧力の計算１
//		圧力を求め、速度から発散を抜く
//
// ①　圧力の配列g_Prsを０クリヤーする
// 　　速度の逆方向の発散g_Divを計算する
//
[numthreads(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE)]
void Project1(uint3 id : SV_DispatchThreadID, uint3 gid : SV_GroupID)
{
	// 場の端を除いたFLUID_SIZEだけ処理をする。このため添字を一つ進める
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	// 圧力をゼロクリヤーする
	g_Prs[ID(i,j,k)].f=0;

	// 速度の逆方向の発散Divを計算する
	g_Div[ID(i,j,k)].f = (g_rVlc[uint3(i+1,j,k)].x -g_rVlc[uint3(i-1,j,k)].x)/2+
						(g_rVlc[uint3(i,j+1,k)].y - g_rVlc[uint3(i,j-1,k)].y)/2+
						(g_rVlc[uint3(i,j,k+1)].z - g_rVlc[uint3(i,j,k-1)].z)/2;
}
//
// 圧力の計算２
//		圧力を求め、速度から発散を抜く
//
// ②　発散g_Divのトリリニア補完を求め、圧力の配列g_Prsに格納する
//
[numthreads(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE)]
void Project2(uint3 id : SV_DispatchThreadID, uint3 gid : SV_GroupID)
{
	// 場の端を除いたFLUID_SIZEだけ処理をする。このため添字を一つ進める
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	// 発散Divのトリリニア補完を求め、圧力の配列Prsに格納する
	g_Prs[ID(i,j,k)].f=(g_Prs[ID(i-1,j,k)].f+g_Prs[ID(i+1,j,k)].f+
						g_Prs[ID(i,j-1,k)].f+g_Prs[ID(i,j+1,k)].f+
						g_Prs[ID(i,j,k-1)].f+g_Prs[ID(i,j,k+1)].f-g_Div[ID(i,j,k)].f)/6;
}
//
// 圧力の計算３
//		圧力を求め、速度から発散を抜く
//
// ③　圧力g_Prsを使って速度g_wVlcを計算する
//		(速度から圧力値を抜く)
//
[numthreads(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE)]
void Project3( uint3 id : SV_DispatchThreadID ,uint3 gid : SV_GroupID )
{
	// 場の端を除いたFLUID_SIZEだけ処理をする。このため添字を一つ進める
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	// 圧力Prsを使って速度vWlcを計算する
	float x=g_rVlc[uint3(i,j,k)].x-(g_Prs[ID(i+1,j,k)].f-g_Prs[ID(i-1,j,k)].f)/2;
	float y=g_rVlc[uint3(i,j,k)].y-(g_Prs[ID(i,j+1,k)].f-g_Prs[ID(i,j-1,k)].f)/2;
	float z=g_rVlc[uint3(i,j,k)].z-(g_Prs[ID(i,j,k+1)].f-g_Prs[ID(i,j,k-1)].f)/2;

	g_wVlc[uint3(i,j,k)]=float4(x,y,z,0);
}

//
// 移流速度の計算
//
[numthreads(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE)]
void AdvectVelocity( uint3 id : SV_DispatchThreadID ,uint3 gid : SV_GroupID )
{
	// 場の端を除いたFLUID_SIZEだけ処理をする。このため添字を一つ進める
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	float3 uv = id+1;  // これから速度を処理する場所の位置
	uv.x -= g_rVlc[uint3(i,j,k)].x*FLUID_TIME_STEP;  // 速度をさかのぼって一つ前の位置
	uv.y -= g_rVlc[uint3(i,j,k)].y*FLUID_TIME_STEP;
	uv.z -= g_rVlc[uint3(i,j,k)].z*FLUID_TIME_STEP;

	uv = (uv+0.5)/(FLUID_SIZE+2);  // 位置をテクスチャのＵＶ座標に変換する
	
	// サンプリングにより値(セミグランジアン)を取り出しg_wVlc_sに格納する
	// (サンプリングによって、トリリニア補完が自動的に行われる)
	g_wVlc_s[uint3(i,j,k)] = g_rVlc.SampleLevel(g_samLinear,uv,0);   // MacCormackを使用するとき
	//g_wVlc[uint3(i,j,k)] = g_rVlc.SampleLevel(g_samLinear,uv,0);   // MacCormackを使用しないとき
}

//
// 逆向きのセミグランジアンを行う (MacCormackを使用するとき)
//
[numthreads(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE)]
void AdvectBack( uint3 id : SV_DispatchThreadID ,uint3 gid : SV_GroupID )
{
	// 場の端を除いたFLUID_SIZEだけ処理をする。このため添字を一つ進める
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	float3 uv = id+1;  // これから速度を処理する場所の位置

	uv.x += g_rVlc[uint3(i,j,k)].x*FLUID_TIME_STEP;  // 速度を進めて次の位置
	uv.y += g_rVlc[uint3(i,j,k)].y*FLUID_TIME_STEP;
	uv.z += g_rVlc[uint3(i,j,k)].z*FLUID_TIME_STEP;

	uv = (uv+0.5)/(FLUID_SIZE+2);  // 位置をテクスチャのＵＶ座標に変換する
	
	// サンプリングにより値(逆向きのセミグランジアン)を取り出しg_wVlc_bsに格納する
	// (サンプリングによって、トリリニア補完が自動的に行われる)
	g_wVlc_bs[uint3(i,j,k)] = g_rVlc_s.SampleLevel(g_samLinear,uv,0);
}
//
// マコーマックスキームを追加する (MacCormackを使用するとき)
//
[numthreads(FLUID_RSIZE, FLUID_RSIZE, FLUID_RSIZE)]
void MacCormack( uint3 id : SV_DispatchThreadID ,uint3 gid : SV_GroupID )
{
	// 場の端を除いたFLUID_SIZEだけ処理をする。このため添字を一つ進める
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	// マコーマックスキーム計算を行う
	float4 New = g_rVlc_s[uint3(i,j,k)] + 0.5*(g_rVlc[uint3(i,j,k)] - g_rVlc_bs[uint3(i,j,k)]);

	// 破綻防止リミッター
	float4 Max = 0, Min = 0;
	float4 v1 = g_rVlc_s[uint3(i,j,k)];
	float4 v2 = g_rVlc_s[uint3(i+1,j,k)];
	float4 v3 = g_rVlc_s[uint3(i,j+1,k)];
	float4 v4 = g_rVlc_s[uint3(i,j,k+1)];
	float4 v5 = g_rVlc_s[uint3(i-1,j,k)];
	float4 v6 = g_rVlc_s[uint3(i,j-1,k)];
	float4 v7 = g_rVlc_s[uint3(i,j,k-1)];
	float4 v8 = g_rVlc_s[uint3(i+1,j+1,k+1)];

	Max = max(max(max(max(max(max(max(v1,v2),v3),v4),v5),v6),v7),v8);
	Min = min(min(min(min(min(min(min(v1,v2),v3),v4),v5),v6),v7),v8);

	float4 Mac = max( min( New, Max ), Min);

	// 上昇気流なし
	//g_wVlc[uint3(i, j, k)] = Mac;

	// 上昇気流(簡易)をつける
	//float Force = 15.0 * g_rDns[uint3(i,j,k)].x;
	float Force = 0.5f * g_rDns[uint3(i,j,k)].x;
	g_wVlc[uint3(i,j,k)] = Mac + float4(0, Force, 0, 0);

}
//
// 端まで行ったら折り返す１
//
// ①　場の端まで行ったら一つ手前の速度を反転させる
//
[numthreads(FLUID_RSIZE, FLUID_RSIZE,1)]
void Boundary1(uint3 id : SV_DispatchThreadID, uint3 gid : SV_GroupID)
{
	// 場の端を除いたFLUID_SIZEだけ処理をする。このため添字を一つ進める
	uint i = id.x+1;
	uint j = id.y+1;
	uint k = id.z+1;

	// 場の端まで行ったら一つ手前の速度を反転させる
	g_wVlc[uint3(0,i,j)]=float4(-g_rVlc[uint3(1,i,j)].x,g_rVlc[uint3(0,i,j)].y,g_rVlc[uint3(0,i,j)].z,0);
	g_wVlc[uint3(FLUID_SIZE,i,j)]=float4(-g_rVlc[uint3(FLUID_SIZE-1,i,j)].x,g_rVlc[uint3(0,i,j)].y,g_rVlc[uint3(0,i,j)].z,0);

	g_wVlc[uint3(i,0,j)]=float4(g_rVlc[uint3(i,0,j)].x,-g_rVlc[uint3(i,1,j)].y,g_rVlc[uint3(i,0,j)].z,0);
	g_wVlc[uint3(i,FLUID_SIZE,j)]=float4(g_rVlc[uint3(i,FLUID_SIZE,j)].x,-g_rVlc[uint3(i,FLUID_SIZE-1,j)].y,g_rVlc[uint3(i,FLUID_SIZE,j)].z,0);

	g_wVlc[uint3(i,j,0)]=float4(g_rVlc[uint3(i,j,0)].x,g_rVlc[uint3(i,j,0)].y,-g_rVlc[uint3(i,j,1)].z,0);
	g_wVlc[uint3(i,j,FLUID_SIZE)]=float4(g_rVlc[uint3(i,j,FLUID_SIZE)].x,g_rVlc[uint3(i,j,FLUID_SIZE)].y,-g_rVlc[uint3(i,j,FLUID_SIZE-1)].z,0);

}
