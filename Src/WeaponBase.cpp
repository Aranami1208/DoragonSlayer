#include "WeaponManager.h"
#include "WeaponBase.h"
#include "Player.h"
#include "EnemyManager.h"

WeaponBase::WeaponBase()
{
}

WeaponBase::~WeaponBase()
{
}

void WeaponBase::SetActive(bool active)
{
	ObjectManager::SetActive(this, active);
	ObjectManager::SetVisible(this, active);
}
bool WeaponBase::HitCheckLine(VECTOR3 positionOld, VECTOR3 position, MeshCollider::CollInfo* coll, int AttackPoint )
{
	if (owner == ePC)
	{
		// 敵との当たり判定
		std::list<EnemyBase*> enemys = ObjectManager::FindGameObjects<EnemyBase>();
		for (EnemyBase* enm : enemys) {
			if (enm->HitLineToMesh(positionOld, position, coll)) {
				enm->AddDamage(AttackPoint, positionOld);
				return true;
			}
		}
	}
	else if (owner == eENM) {
		// PLayerとの当たり判定
		Player* pc = ObjectManager::FindGameObject<Player>();
		if (pc->HitLineToMesh(positionOld, position, coll)) {
			pc->AddDamage(AttackPoint, positionOld);
			return true;
		}
	}
	return false;
}