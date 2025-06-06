#include "EnemyManager.h"

EnemyManager::EnemyManager()
{
	ObjectManager::DontDestroy(this);		// 自体は消されない
	ObjectManager::SetVisible(this, false);		// 自体は表示しない
	mesh = nullptr;
	meshCol = nullptr;

	meshstruct ms;

	// RS
	meshList.push_back(ms);
	meshList.back().name = "RS";
	meshList.back().mesh = new CFbxMesh();
	meshList.back().mesh->Load("Data/Char/RS-0B/RS-0B.mesh");
	meshList.back().mesh->LoadAnimation(aIdle, "Data/Char/RS-0B/RS-0B_wait.anmx", true);
	meshList.back().mesh->LoadAnimation(aRun, "Data/Char/RS-0B/RS-0B_forward_RAXZ.anmx", true, eRootAnimXZ);
	meshList.back().mesh->LoadAnimation(aAttack1, "Data/Char/RS-0B/RS-0B_attack.anmx", false);
	meshList.back().mesh->LoadAnimation(aDead, "Data/Char/RS-0B/RS-0B_dead.anmx", false);

	// Golem
	meshList.push_back(ms);
	meshList.back().name = "Golem";
	meshList.back().mesh = new CFbxMesh();
	meshList.back().mesh->Load("Data/Char/Golem/golem.mesh");
	meshList.back().mesh->LoadAnimation(aIdle, "Data/Char/Golem/golem_stand.anmx", true);
	meshList.back().mesh->LoadAnimation(aRun, "Data/Char/Golem/golem_walk_RAXZ.anmx", true, eRootAnimXZ);
	meshList.back().mesh->LoadAnimation(aAttack1, "Data/Char/Golem/golem_attack.anmx", false);
	meshList.back().mesh->LoadAnimation(aDead, "Data/Char/Golem/golem_die.anmx", false);

	// BlueDragon
	meshList.push_back(ms);
	meshList.back().name = "Bd";
	meshList.back().mesh = new CFbxMesh();
	meshList.back().mesh->Load("Data/Char/bd/bd.mesh");
	meshList.back().mesh->LoadAnimation(aIdle, "Data/Char/bd/bd_stand.anmx", true);
	meshList.back().mesh->LoadAnimation(aRun, "Data/Char/bd/bd_walk_RAXZ.anmx", true, eRootAnimXZ);
	meshList.back().mesh->LoadAnimation(aAttack1, "Data/Char/bd/bd_attack.anmx", false);
	meshList.back().mesh->LoadAnimation(aDead, "Data/Char/bd/bd_die.anmx", false);

	// Android 
	meshList.push_back(ms);
	meshList.back().name = "Android";
	meshList.back().mesh = new CFbxMesh();
	meshList.back().mesh->Load("Data/Char/Android/Android2.mesh");
	meshList.back().mesh->LoadAnimation(aIdle, "Data/Char/Android/Android2_wait.anmx", true);
	meshList.back().mesh->LoadAnimation(aRun,
		"Data/Char/Android/Android2_forward_middle_RAXZ.anmx", true, eRootAnimXZ);
	meshList.back().mesh->LoadAnimation(aAttack1, "Data/Char/Android/Android2_attack.anmx",
		false);
	meshList.back().mesh->LoadAnimation(aDead, "Data/Char/Android/Android2_die.anmx", false);

}

EnemyManager::~EnemyManager()
{
	for (meshstruct &ms : meshList)
	{
		SAFE_DELETE(ms.mesh);
	}
}

CFbxMesh* EnemyManager::MeshList(std::string str)
{
	for (meshstruct& ms : meshList)
	{
		if (str == ms.name) return ms.mesh;
	}
	MessageBox(nullptr, "EnemyManager::MeshList()", _T("■□■ 指定のメッシュ名のメッシュはメッシュリストにありません ■□■"), MB_OK);
	return nullptr;
}

void EnemyManager::Spawn(TextReader* txt, int n)
{
	const int headColumn = 1;

	// 溶岩の敵のとき
	if (txt->GetString(n, 0) == "EnemyLava")
	{
		VECTOR3 pos, boxMin, boxMax;
		pos.x = txt->GetFloat(n, headColumn + 0);
		pos.y = txt->GetFloat(n, headColumn + 1);
		pos.z = txt->GetFloat(n, headColumn + 2);
		boxMax.x = txt->GetFloat(n, headColumn + 3) / 2;
		boxMax.y = 0;
		boxMax.z = txt->GetFloat(n, headColumn + 5) / 2;
		boxMin.x = -txt->GetFloat(n, headColumn + 3) / 2;
		boxMin.y = -txt->GetFloat(n, headColumn + 4);
		boxMin.z = -txt->GetFloat(n, headColumn + 5) / 2;

		EnemyLava* enmObj = Instantiate<EnemyLava>();
		enmObj->SetPosition(pos);
		enmObj->SetBoundingBox(boxMin, boxMax);

		return;
	}

	// 溶岩以外の敵のとき
	if (txt->GetString(n, headColumn) == "Area")   // ナビがエリアかルートかどうか
	{
		navigationArea = true;
	}
	else {
		navigationArea = false;
	}
	navigationMap.clear();
	navigationMap.shrink_to_fit();
	int num = (txt->GetColumns(n) - (headColumn+1)) / 3;	// ナビマップの個数
	for (int i = 0; i < num; i++ )
	{
		VECTOR3 pos;
		pos.x = txt->GetFloat(n, headColumn + 1 + i * 3);
		pos.y = txt->GetFloat(n, headColumn + 1 + i * 3 + 1);
		pos.z = txt->GetFloat(n, headColumn + 1 + i * 3 + 2);
		navigationMap.emplace_back(pos);
	}

	std::string str = txt->GetString(n, 0);

	// ---------------------------------------

	if (str == "EnemyRS")
	{
		EnemyRS* enmObj = Instantiate<EnemyRS>();
		enmObj->MakeNavigationMap(navigationArea, navigationMap);
	}

	if (str == "EnemyGolem")
	{
		EnemyGolem* enmObj = Instantiate<EnemyGolem>();
		enmObj->MakeNavigationMap(navigationArea, navigationMap);
	}

	if (str == "EnemyBd")
	{
		EnemyBd* enmObj = Instantiate<EnemyBd>();
		enmObj->MakeNavigationMap(navigationArea, navigationMap);


	}
	if (str == "EnemyAndroid")
	{
		EnemyAndroid* enmObj = Instantiate<EnemyAndroid>();
		enmObj->MakeNavigationMap(navigationArea, navigationMap);
	}
}




// 敵の数を数える
// 敵マネージャー(this)と敵・溶岩を除く
int EnemyManager::GetNumber()
{
	int cnt = 0;
	std::list<EnemyBase*> enemys = ObjectManager::FindGameObjects<EnemyBase>();
	for ( EnemyBase*& enm : enemys)
	{
		if (enm == this) continue;
		EnemyLava* el = dynamic_cast<EnemyLava*>(enm);
		if (el != nullptr)	continue;
		cnt++;
	}
	return cnt;
}


