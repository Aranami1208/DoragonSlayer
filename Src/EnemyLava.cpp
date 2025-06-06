#include  "EnemyManager.h"
#include  "Player.h"

namespace {
	const int   AttackPoint = 30;
};

// コンストラクタ
EnemyLava::EnemyLava()
{
	// バウンディングボックスの大きさ 最小値　VECTOR3 vMin(省略値 VECTOR3(-50.0f, -10.0f, -50.0f))
	boxMin = VECTOR3(-50.0f, -10.0f, -50.0f);
	// バウンディングボックスの大きさ 最大値　VECTOR3 vMax(省略値 VECTOR3(50.0f, 0.0f, 50.0f))
	boxMax = VECTOR3(50.0f, 0.0f, 50.0f);

	// 初期化
	posMin = VECTOR3(0,0,0);
	posMax = VECTOR3(0,0,0);
	lava = nullptr;
}

// デストラクタ
EnemyLava::~EnemyLava()
{
	SAFE_DELETE(lava);
}

void EnemyLava::Start()
{
	lava = new CWave(boxMax.x-boxMin.x, boxMax.z - boxMin.z, "Data/Map/MapItem/lava03_NM.png", "Data/Map/MapItem/lava03.png");
	lava->m_fMaxDistance = 40.0f;
	lava->m_fMinDistance = 20.0f;
	lava->m_fWaveHeight = 0.15f;
}

void EnemyLava::SetBoundingBox(VECTOR3 minIn, VECTOR3 maxIn)
{
	boxMin = minIn;
	boxMax = maxIn;
}

// 敵溶岩流オブジェクトの更新
void	EnemyLava::Update()
{
	// ステータスごとの処理  --------------------------------
	switch (state)
	{
	case  stFlash: // 敵がダメージから復帰したときの処理。breakをつけずに次のNORMALを行う。
	case  stNormal: // 敵が通常状態のときの処理
		updateNormal();
		break;

	case  stDamage: // 敵がダメージ状態のときの処理
		state = stNormal;
		break;

	case  stDead:	// 敵が死亡状態のときの処理
		state = stNormal;
		break;
	}
}
void	EnemyLava::updateNormal()
{
	posMin = boxMin + transform.position;
	posMax = boxMax + transform.position;

	Player* pc = ObjectManager::FindGameObject<Player>();
	if (hitCheck(pc))
	{
		pc->AddDamage(AttackPoint, transform.position);
	}
	std::list<EnemyBase*> els = ObjectManager::FindGameObjects<EnemyBase>();
	for (EnemyBase* &em : els)
	{
		if (em == this) continue;
		if (hitCheck(em))
		{
			em->AddDamage(AttackPoint, transform.position);
		}
	}
}

bool	EnemyLava::hitCheck(Object3D* obj)
{
	if (obj->Position().x >= posMin.x && obj->Position().y >= posMin.y && obj->Position().z >= posMin.z
		&&
		obj->Position().x <= posMax.x && obj->Position().y <= posMax.y + lava->m_fWaveHeight && obj->Position().z <= posMax.z)
	{
		return true;
	}
	return false;
}

void EnemyLava::DrawDepth()
{
	// 影を作らない
}
void EnemyLava::DrawScreen()
{
	MATRIX4X4 mat = XMMatrixTranslation(posMin.x, posMax.y, posMin.z);
	lava->Render(mat);
}