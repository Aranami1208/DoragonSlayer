#include "EffectManager.h"

namespace {
	const int EFFECT_NUM_MAX = 100;    // MAX :１つのオブジェクトの中の発生数。

}

EffectParticle::EffectParticle()
{
	ObjectManager::SetDrawOrder(this, -200);   // 透明色のため遅めに描画する

	partB = ObjectManager::FindGameObject<EffectManager>()->ParticleList("");

	maxParticle = EFFECT_NUM_MAX;
	ptArray = new PART[EFFECT_NUM_MAX];     // 一つのオブジェクトの中のパーティクル配列
	frame = 0;
	normal = VECTOR3(0, 0, 0);
}
EffectParticle::~EffectParticle()
{
	SAFE_DELETE_ARRAY(ptArray);
}

void EffectParticle::SetEffectName(std::string name)
{
	partB = ObjectManager::FindGameObject<EffectManager>()->ParticleList(name);
}

void EffectParticle::Start()
{

	maxParticle = partB->m_nNum;  // パーティクル構造体のパーティクル数を設定する
	if (maxParticle > EFFECT_NUM_MAX) maxParticle = EFFECT_NUM_MAX; // もしも要素数をオーバーしていたら要素数に訂正する
	frame = 0;

	// 移動方向。法線方向を中心としてランダム方向に発生させる
	VECTOR3 vDist, vMin, vMax;
	vDist.x = (1.0f - fabsf(normal.x)) / 2;
	vDist.y = (1.0f - fabsf(normal.y)) / 2;
	vDist.z = (1.0f - fabsf(normal.z)) / 2;

	if (normal.x < 0)
	{
		vMin.x = normal.x - vDist.x;
		vMax.x = 0.0f + vDist.x;
	}
	else {
		vMin.x = 0.0f - vDist.x;
		vMax.x = normal.x + vDist.x;
	}
	if (normal.y < 0)
	{
		vMin.y = normal.y - vDist.y;
		vMax.y = 0.0f + vDist.y;
	}
	else {
		vMin.y = 0.0f - vDist.y;
		vMax.y = normal.y + vDist.y;
	}
	if (normal.z < 0)
	{
		vMin.z = normal.z - vDist.z;
		vMax.z = 0.0f + vDist.z;
	}
	else {
		vMin.z = 0.0f - vDist.z;
		vMax.z = normal.z + vDist.z;
	}

	// パーティクルの開始
	for (int i = 0; i < maxParticle; i++)
	{
		ptArray[i].Pos = transform.position;
		ptArray[i].Dir.x = ((float)rand() / (float)RAND_MAX) * (vMax.x - vMin.x) + vMin.x;
		ptArray[i].Dir.y = ((float)rand() / (float)RAND_MAX) * (vMax.y - vMin.y) + vMin.y;
		ptArray[i].Dir.z = ((float)rand() / (float)RAND_MAX) * (vMax.z - vMin.z) + vMin.z;
		ptArray[i].Dir = normalize(ptArray[i].Dir);
		ptArray[i].Speed = (5 + ((float)rand() / (float)RAND_MAX)) * partB->m_fSpeed;      // 移動スピード。ランダム
		ptArray[i].BirthFrame = (int)(((float)rand() / RAND_MAX) * partB->m_iBarthFrame);  // 開始までの待ち時間。ランダム
	}
}
void EffectParticle::Update()
{
	frame += 60 * SceneManager::DeltaTime();

	for (int i = 0; i < maxParticle; i++)
	{
		if (frame > ptArray[i].BirthFrame) // 一個のパーティクルが表示開始時間になったとき
		{
			// 移動
			ptArray[i].Pos = ptArray[i].Pos + ptArray[i].Dir * ptArray[i].Speed * 60 * SceneManager::DeltaTime();

			// 重力の計算
			ptArray[i].Dir = ptArray[i].Dir + VECTOR3(0, -0.049f * 60 * SceneManager::DeltaTime(), 0);

			// 地面でのバウンド
			if (partB->m_ifBound == 1)
			{
				if (ptArray[i].Pos.y < 0)
				{
					ptArray[i].Dir.y = -ptArray[i].Dir.y;
					ptArray[i].Speed /= (1.8f * 60 * SceneManager::DeltaTime());
				}
			}
		}
	}

	if (frame >= partB->m_FrameEnd) // 表示時間終了か
	{
		DestroyMe();
	}

}

void EffectParticle::DrawDepth()
{
	// 影を作らない
}

void EffectParticle::DrawScreen()
{
	//使用するシェーダーのセット
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetShader(GameDevice()->m_pShader->m_pEffect3D_VS_POINT, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->GSSetShader(GameDevice()->m_pShader->m_pEffect3D_GS_POINT, NULL, 0);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShader(GameDevice()->m_pShader->m_pEffect3D_PS, NULL, 0);

	//バーテックスバッファーをセット
	UINT stride = sizeof(PARTICLE_VERTEX);
	UINT offset = 0;
	GameDevice()->m_pD3D->m_pDeviceContext->IASetVertexBuffers(0, 1, &partB->m_pVertexBuffer, &stride, &offset);

	// 加算合成色のブレンディングを設定
	UINT mask = 0xffffffff;
	if (partB->m_nBlendFlag == 1)   // 加算合成色指定
	{
		GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateAdd, NULL, mask);
	}

	// パーティクル１粒を１枚ポイントスプライトとしてm_MaxParticle枚描画
	for (int i = 0; i < maxParticle; i++)
	{
		// 描画開始前のものと描画フレームアウトのものは描画しない
		if (partB->m_FrameEnd < frame + (partB->m_iBarthFrame - ptArray[i].BirthFrame)) continue;

		//個々のパーティクルの、視点を向くワールドトランスフォームを求める
		MATRIX4X4 mWorld = GetLookatMatrix(ptArray[i].Pos, GameDevice()->m_vEyePt);

		renderParticle(mWorld, GameDevice()->m_mView, GameDevice()->m_mProj);  // パーティクルをレンダリング
	}

	if (partB->m_nBlendFlag == 1)   // 加算合成色指定
	{
		// 通常のブレンディングに戻す
		GameDevice()->m_pD3D->m_pDeviceContext->OMSetBlendState(GameDevice()->m_pD3D->m_pBlendStateTrapen, NULL, mask);
	}

	//ジオメトリシェーダーのリセット
	GameDevice()->m_pD3D->m_pDeviceContext->GSSetShader(NULL, NULL, 0);

	return;

}

void EffectParticle::renderParticle(MATRIX4X4 W, MATRIX4X4 V, MATRIX4X4 P)
{
	//シェーダーのコンスタントバッファーに各種データを渡す
	D3D11_MAPPED_SUBRESOURCE pData;
	CONSTANT_BUFFER_EFFECT cb;
	ZeroMemory(&cb, sizeof(cb));
	if (SUCCEEDED(GameDevice()->m_pD3D->m_pDeviceContext->Map(GameDevice()->m_pShader->m_pConstantBufferEffect, 0, D3D11_MAP_WRITE_DISCARD, 0, &pData)))
	{
		//ワールド、カメラ、射影行列を渡す // -- 2017.8.25
		cb.mW = XMMatrixTranspose(W);
		cb.mV = XMMatrixTranspose(V);
		cb.mP = XMMatrixTranspose(P);

		cb.fAlpha = partB->m_fAlpha;            // パーティクルの透明度
		cb.fSize = partB->m_fDestSize;          // パーティクルのサイズ 
		memcpy_s(pData.pData, pData.RowPitch, (void*)(&cb), sizeof(cb));
		GameDevice()->m_pD3D->m_pDeviceContext->Unmap(GameDevice()->m_pShader->m_pConstantBufferEffect, 0);
	}

	//このコンスタントバッファーをどのシェーダーで使うか
	GameDevice()->m_pD3D->m_pDeviceContext->VSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferEffect);
	GameDevice()->m_pD3D->m_pDeviceContext->GSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferEffect);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetConstantBuffers(0, 1, &GameDevice()->m_pShader->m_pConstantBufferEffect);
	//頂点インプットレイアウトをセット
	GameDevice()->m_pD3D->m_pDeviceContext->IASetInputLayout(GameDevice()->m_pShader->m_pEffect3D_VertexLayout);
	//プリミティブ・トポロジーをセット
	GameDevice()->m_pD3D->m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
	//テクスチャーをシェーダーに渡す
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetSamplers(0, 1, &GameDevice()->m_pD3D->m_pSampleLinear);
	GameDevice()->m_pD3D->m_pDeviceContext->PSSetShaderResources(0, 1, &partB->m_pTexture);
	//プリミティブをレンダリング
	GameDevice()->m_pD3D->m_pDeviceContext->Draw(1, 0);

}
