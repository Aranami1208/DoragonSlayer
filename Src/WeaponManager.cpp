#include "WeaponManager.h"

WeaponManager::WeaponManager()
{
	ObjectManager::DontDestroy(this);		// 自体は消されない
	ObjectManager::SetVisible(this, false);		// 自体は表示しない
	mesh = nullptr;
	meshCol = nullptr;

	// メッシュリスト
	meshstruct ms;

	// FireBall		ファイヤーボール　大
	meshList.push_back(ms);
	meshList.back().name = "FireBall";
	meshList.back().mesh = new CFbxMesh();
	meshList.back().mesh->Load("Data/Item/FireBall.mesh");

	// FireBall2	ファイヤーボール　小
	meshList.push_back(ms);
	meshList.back().name = "FireBall2";
	meshList.back().mesh = new CFbxMesh();
	meshList.back().mesh->Load("Data/Item/FireBall2.mesh");

	// Bullet		弾
	meshList.push_back(ms);
	meshList.back().name = "Bullet";
	meshList.back().mesh = new CFbxMesh();
	meshList.back().mesh->Load("Data/Item/Bullet.mesh");

	// Laser		レーザー光
	meshList.push_back(ms);
	meshList.back().name = "Laser";
	meshList.back().mesh = new CFbxMesh();
	meshList.back().mesh->Load("Data/Item/Laser2.mesh");

	// 日本刀
	meshList.push_back(ms);
	meshList.back().name = "JapanSword";
	meshList.back().mesh = new CFbxMesh();
	meshList.back().mesh->Load("Data/Item/Sword.mesh");
	meshList.back().offset = VECTOR3(0, 1.6f, 0);	  // 先端の位置

	// ソード
	meshList.push_back(ms);
	meshList.back().name = "Sword";
	meshList.back().mesh = new CFbxMesh();
	meshList.back().mesh->Load("Data/Item/Swordm.mesh");
	meshList.back().offset = VECTOR3(0, 1.7f, 0);	  // 先端の位置

	// ライフル
	meshList.push_back(ms);
	meshList.back().name = "Rifle";
	meshList.back().mesh = new CFbxMesh();
	meshList.back().mesh->Load("Data/Item/Gun.mesh");
	meshList.back().offset = VECTOR3(0.51f, 0, 0);	  // 銃口の位置

	// ピストル
	meshList.push_back(ms);
	meshList.back().name = "Pistol";
	meshList.back().mesh = new CFbxMesh();
	meshList.back().mesh->Load("Data/Item/Pistol.mesh");
	meshList.back().offset = VECTOR3(0.2f, 0.1f, 0);  // 銃口の位置

	// マーク
	meshList.push_back(ms);
	meshList.back().name = "Mark";
	meshList.back().mesh = new CFbxMesh();
	meshList.back().mesh->Load("Data/Item/Mark.mesh");
	meshList.back().offset = VECTOR3(0, 0, 0);  // 

}

WeaponManager::~WeaponManager()
{
	for (meshstruct &ms : meshList)
	{
		SAFE_DELETE(ms.mesh);
	}
}

Fluid* WeaponManager::SpawnFlamethrower(VECTOR3 startIn, VECTOR3 targetIn, OwnerID owner, std::string& tag)
{
	WeaponFlamethrower* wf = Instantiate<WeaponFlamethrower>();
	wf->SetPosition(startIn);
	wf->SetTarget(targetIn);
	wf->SetOwner(owner);
	return wf->Spawn(tag);
}



