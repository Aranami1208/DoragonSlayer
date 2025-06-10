#include "EnemyManager.h" 
#include "MapManager.h" 
#include "Player.h" 
#include "DataCarrier.h" 
#include "DisplayInfo.h" 

namespace {
	const float Gravity = 0.025f; // 重力加速度(正の値) 
	const float MoveSpeed = 0.06f;         // 移動スピード 
	const float RotSpeed = 12.0f;          // 回転スピードは速くする 
	const float ReachAngle = 270.0f;       // 視野 
	const float ReachDistLimit = 25;       // 近接Reach限界値 
	const float AttackDistLimit = 3.0f;    // 攻撃接近限界値 
	const int   MaxHitPoint = 2000;        // 体力 
	const float MaxFlashTime = 5;
	const float MaxIdleTime = 480;
};

EnemyAndroid::EnemyAndroid()
{
	animator = new Animator(); // インスタンスを作成 
	mesh = ObjectManager::FindGameObject<EnemyManager>()->MeshList("Android");//EnemyManagerのメッシュ 
	swordObj = ObjectManager::FindGameObject<WeaponManager>()->Spawn<WeaponSword>
		(WeaponBase::eENM);  // 剣・盾の発生 
	swordObj->SetWeaponSword("", 0, 55, VECTOR3(0, 0, 1.6f), VECTOR3(0, 0, 0), VECTOR3(0.0f, 0.0f, 0.0f));
	swordObj->SetParent(this);
	animator->SetModel(mesh); // このモデルでアニメーションする 
	animator->Play(aRun);
	animator->SetPlaySpeed(1.0f);
	meshCol = new MeshCollider();
	meshCol->MakeFromMesh(mesh, animator);
	state = stNormal;
	hitPoint = MaxHitPoint;
	flashTimer = 0;
	aroundID = 0;
	targetPlayer = nullptr;
	swordObj->SetActive(false);
	rootNode = nullptr;
	makeBehaviour();   // ビヘイビアツリーの構築 
}
EnemyAndroid::~EnemyAndroid()
{
	SAFE_DELETE(meshCol);
	SAFE_DELETE(rootNode);
}

SphereCollider EnemyAndroid::Collider() // 少し小さめのバウンディングボールとする 
{
	SphereCollider col;
	col.radius = 1.5f;
	col.center = transform.position + VECTOR3(0, col.radius, 0);
	return col;
}
void EnemyAndroid::DisplayName()
{
	// 敵の頭上のステータス表示 
	char name[] = "アンドロイド";
	//VECTOR2 dispSize = VECTOR2(3.0f, 1.0f); // 表示の大きさ 大 
	//VECTOR2 dispSize = VECTOR2(2.0f, 0.7f);  // 表示の大きさ 中 
	VECTOR2 dispSize = VECTOR2(0.8f, 0.3f); // 表示の大きさ 小 
	// 表示位置 
	VECTOR3 vEnmPos = transform.position;
	vEnmPos.y += mesh->m_vMax.y + dispSize.y * 0.7f;
	// ＨＰバーの表示 
	float fHp = (float)hitPoint / MaxHitPoint;
	CSprite sprite;
	sprite.Draw3D(ObjectManager::FindGameObject<DisplayInfo>()->Image(), vEnmPos,
		VECTOR2(dispSize.x * fHp, dispSize.y * 0.5f), VECTOR2(320, 96), VECTOR2(125 * fHp, 16));
	// 敵の名前表示 
	vEnmPos.y += dispSize.y * 0.7f;
	GameDevice()->m_pFont->Draw3D(vEnmPos, name, dispSize, RGB(255, 0, 0), 1.0f, _T("HGP創英角ｺﾞｼｯｸUB"));
}

void EnemyAndroid::Update()
{
	VECTOR3 positionOld = transform.position;
	switch (state) {
	case stFlash:
		flashTimer -= 60 * SceneManager::DeltaTime();
		if (flashTimer <= 0) state = stNormal;
		updateNormal();
		break;
	case stNormal:
		updateNormal();
		break;
	case stDamage:
		updateDamage();
		break;
	case stDead:
		updateDead();
		break;
	}
	// 他のEnemyにめり込まないようにする 
	std::list<EnemyBase*> enemys = ObjectManager::FindGameObjects<EnemyBase>();
	for (EnemyBase*& enm : enemys) {
		if (enm->Mesh() == nullptr) continue;
		if (enm == this) continue;
		VECTOR3 push;
		if (HitSphereToSpherePush(enm->Collider(), false, &push)) {
				transform.position += push;
		}
	}
	// マップとの接触判定と自然落下処理 
	transform.position.y += speedY;    // 自然落下の処理 
	speedY -= Gravity * 60 * SceneManager::DeltaTime();
	MapManager* mm = ObjectManager::FindGameObject<MapManager>();
	if (mm->IsCollisionMoveGravity(positionOld, transform.position) != clFall)
	{
		speedY = 0;
	}
	animator->Update();    // 2024.9.5 
}
void EnemyAndroid::updateNormal()
{
	rootNode->Run(); // ビヘイビアツリーの実行 
}
void EnemyAndroid::updateDamage()
{
	state = stFlash;
	flashTimer = MaxFlashTime;
}
void EnemyAndroid::updateDead()
{
	animator->MergePlay(aDead);
	if (animator->Finished()) {
		DataCarrier* dc = ObjectManager::FindGameObject<DataCarrier>();
		dc->AddScore(MaxHitPoint / 10); // 最大ＨＰの1/10をスコアに加える 
		swordObj->SetActive(false);   // 剣の削除より先に親が消えるときの対策 
		ObjectManager::Destroy(swordObj);
		DestroyMe();
	}
}


// ビヘイビアツリーの構築 
void EnemyAndroid::makeBehaviour()
{
	rootNode = new BehaviourSequence();
	auto* node1 = rootNode->AddChild<BehaviourSelector>();
	auto* node2 = node1->AddChild<BehaviourSequence>();
	node2->AddChild<BehaviourAction>(this, actionIsReach);
	auto* node3 = node2->AddChild<BehaviourSelector>();
	node3->AddChild<BehaviourAction>(this, actionReach);
	node3->AddChild<BehaviourAction>(this, actionAttackNeedleFire);
	auto* node4 = node1->AddChild<BehaviourSelector>();
	node4->AddChild<BehaviourAction>(this, actionIdle);
	node4->AddChild<BehaviourAction>(this, actionWalk);
}


// ビヘイビアツリーのアクション関数 
// (static関数のため自インスタンスアドレスthisを引数objInで受け取る) 
BehaviourBase::BtState EnemyAndroid::actionIsReach(Object3D * objIn)
{
	auto* my = dynamic_cast<EnemyAndroid*>(objIn);
	// plyerが視野に入ったかのチェック 
	Player* player = ObjectManager::FindGameObject<Player>();
	if (my->CheckReach(player, ReachAngle, ReachDistLimit))
	{
		my->targetPlayer = player; // ターゲットとなるＰＣ 
		return BehaviourBase::bsTrue;   // 視野に入ったらbsTrue 
	}
	else {
		my->targetPlayer = nullptr;
	}
	return BehaviourBase::bsFalse;   // 視野に入らなかったらbsFalse 
}
BehaviourBase::BtState EnemyAndroid::actionIdle(Object3D* objIn)
{
	auto* my = dynamic_cast<EnemyAndroid*>(objIn);
	my->idleTimer -= 60 * SceneManager::DeltaTime();
	if (my->idleTimer <= 0 || my->targetPlayer != nullptr)
	{
		return BehaviourBase::bsFalse;   // アイドルが終わったらbsFalse 
	}
	my->animator->MergePlay(aIdle);
	return BehaviourBase::bsTrue;    // アイドル中はbsTrue 
}

BehaviourBase::BtState EnemyAndroid::actionWalk(Object3D* objIn)   // ルートに従って移動 
{
	auto* my = dynamic_cast<EnemyAndroid*>(objIn);
	if (my->navigationArea)
	{
		if (my->MoveToTarget(my->target, MoveSpeed, RotSpeed)) // エリアの目的地へ移動 
		{
			my->target = my->GetNextAreaTarget();
			my->animator->MergePlay(aIdle);
			my->idleTimer = MaxIdleTime;
			return BehaviourBase::bsFalse;  // 到達したらbsFalse 
		}
	}
	else {
		my->target = my->navigationMap[my->aroundID];  // ルートの目的地 
		if (my->MoveToTarget(my->target, MoveSpeed, RotSpeed)) // ルートの目的地へ移動 
		{
			my->aroundID = (my->aroundID + 1) % my->navigationMap.size(); //次の目標 
			my->animator->MergePlay(aIdle);
			my->idleTimer = MaxIdleTime;
			return BehaviourBase::bsFalse;  // 到達したらbsFalse 
		}
	}
	my->animator->MergePlay(aRun);
	return BehaviourBase::bsTrue;      // 移動中(到達しなかったら)はbsTrue 
}

BehaviourBase::BtState EnemyAndroid::actionReach(Object3D * objIn)   // Playerに向かって移動 
{
	auto* my = dynamic_cast<EnemyAndroid*>(objIn);

	if (my->targetPlayer == nullptr) { // ターゲットとなるＰＣが無いとき(普通はあり得ない) 
		return BehaviourBase::bsTrue;
	}

	VECTOR3 toPlayer = my->targetPlayer->Position() - my->transform.position;// 自分から見た位置 
	if (magnitude(toPlayer) < AttackDistLimit)  // 攻撃距離に入ったか 
	{
		return BehaviourBase::bsFalse;   // 攻撃に移るときはbsFalse 
	}
	else {
		// Reach距離の外に出たら、元に戻る 
		if (toPlayer.Length() >= ReachDistLimit) {
			my->animator->MergePlay(aRun);
			return BehaviourBase::bsTrue;  // Reach終わるときはbsTrue 
		}
		my->MoveToTarget(my->targetPlayer->Position(), MoveSpeed, RotSpeed);//プレイヤーに向 
	}
	my->animator->MergePlay(aRun);
	return BehaviourBase::bsTrue;     // Reach継続のときはbsTrue 
}

BehaviourBase::BtState EnemyAndroid::actionAttackSword(Object3D* objIn)  // 攻撃処理 
{
	auto* my = dynamic_cast<EnemyAndroid*>(objIn);
	my->swordObj->SetActive(true);
	my->animator->MergePlay(aAttack1);
	if (my->animator->Finished()) // 攻撃アニメーションが終了？ 
	{
		my->animator->Play(aRun);
		my->swordObj->SetActive(false);
		return BehaviourBase::bsTrue;  // 攻撃アニメーションが正常に終了したときはbsTrue 
	}
	return BehaviourBase::bsRunning;   // 攻撃中のときはbsRunning 
}

BehaviourBase::BtState EnemyAndroid::actionAttackNeedleFire(Object3D* objIn)
{
	auto* my = dynamic_cast<EnemyAndroid*>(objIn);
	my->animator->MergePlay(aAttack1);

	if (my->targetPlayer == nullptr) {  	// ターゲットとなるＰＣが無いとき(普通はあり得ない)
		my->animator->MergePlay(aRun);
		return BehaviourBase::bsFalse;
	}

	my->FireTimer++;


	WeaponManager* wm = ObjectManager::FindGameObject<WeaponManager>();

	VECTOR3 startIn = my->TargetPos;
	VECTOR3 targetIn = startIn + VECTOR3(0, 2, 0);

	std::string tag = "";

	if (my->FireTimer == 1)
	{
		ObjectManager::FindGameObject<WeaponManager>()->SpawnMany<WeaponFireBall2>(my->Position(), targetIn, WeaponBase::eENM);
	}

	if (my->FireTimer % my->FireInterval == 0)
	{
		ObjectManager::FindGameObject<WeaponManager>()->SpawnMany<WeaponFireBall2>(startIn, targetIn, WeaponBase::eENM);
		my->TargetPos = my->targetPlayer->Position();
	}

	if (my->animator->Finished()) // 攻撃アニメーションが終了？ 
	{
		my->TargetPos = VECTOR3(-5000);
		my->FireTimer = 0;
		my->animator->Play(aRun);
		my->swordObj->SetActive(false);
		return BehaviourBase::bsTrue;  // 攻撃アニメーションが正常に終了したときはbsTrue 
	}

	return BehaviourBase::bsRunning;
}
