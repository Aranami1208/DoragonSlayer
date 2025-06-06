#include "EnemyManager.h"
#include "MapManager.h"
#include "WeaponManager.h"
#include "Player.h"
#include "DataCarrier.h"
#include "DisplayInfo.h"

namespace {
	const float Gravity = 0.025f; // 重力加速度(正の値)
	const float MoveSpeed = 0.02f;
	const float RotSpeed = 3.0f;
	const float ReachAngle = 120.0f;        // 視野
	const float ReachDistLimit = 20;        // 近接Reach限界値
	const float AttackDistLimit = 10;       // 攻撃接近限界値
	const int   MaxHitPoint = 1000;
	const float MaxFlashTime = 5;
	const float MaxIdleTime = 480;
};

EnemyGolem::EnemyGolem()
{
	animator = new Animator(); // インスタンスを作成

	mesh = ObjectManager::FindGameObject<EnemyManager>()->MeshList("Golem");	  // EnemyManagerのメッシュを参照する
	animator->SetModel(mesh); // このモデルでアニメーションする
	animator->SetPlaySpeed(1.0f);
	animator->Play(aRun);

	meshCol = new MeshCollider();
	meshCol->MakeFromMesh(mesh, animator);

	state = stNormal;
	hitPoint = MaxHitPoint;

	flashTimer = 0;

	aroundID = 0;
	targetPlayer = nullptr;

	rootNode = nullptr;
	makeBehaviour();   // ビヘイビアツリーの構築
}

EnemyGolem::~EnemyGolem()
{
	SAFE_DELETE(meshCol);
	SAFE_DELETE(rootNode);
}

SphereCollider EnemyGolem::Collider()
{
	// 少し小さめのバウンディングボールとする
	SphereCollider col;
	col.radius = 3.0f;
	col.center = transform.position + VECTOR3(0, col.radius, 0);
	return col;
}

void EnemyGolem::DisplayName()
{
	// 敵の頭上のステータス表示

	char name[] = "ゴーレム";
	//VECTOR2 dispSize = VECTOR2(3.0f, 1.0f);	// 表示の大きさ　大
	VECTOR2 dispSize = VECTOR2(2.0f, 0.7f);		// 表示の大きさ　中
	//VECTOR2 dispSize = VECTOR2(0.8f, 0.3f);	// 表示の大きさ　小

	// 表示位置
	VECTOR3 vEnmPos = transform.position;
	vEnmPos.y += mesh->m_vMax.y + dispSize.y * 0.7f;
	// ＨＰバーの表示
	float fHp = (float)hitPoint / MaxHitPoint;
	CSprite sprite;
	sprite.Draw3D(ObjectManager::FindGameObject<DisplayInfo>()->Image(), vEnmPos, VECTOR2(dispSize.x * fHp, dispSize.y * 0.5f), VECTOR2(320, 96), VECTOR2(125 * fHp, 16));
	// 敵の名前表示
	vEnmPos.y += dispSize.y * 0.7f;
	GameDevice()->m_pFont->Draw3D(vEnmPos, name, dispSize, RGB(255, 0, 0), 1.0f, _T("HGP創英角ｺﾞｼｯｸUB"));
}

void EnemyGolem::Update()
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
		if (enm == this ) continue;
		VECTOR3 push;
		if (HitSphereToSpherePush(enm->Collider(), false, &push)) {
			transform.position += push;
		}
	}

	// マップとの接触判定と自然落下処理
	transform.position.y += speedY;	   // 自然落下の処理
	speedY -= Gravity * 60 * SceneManager::DeltaTime();
	MapManager* mm = ObjectManager::FindGameObject<MapManager>();
	if (mm->IsCollisionMoveGravity(positionOld, transform.position) != clFall)
	{
		speedY = 0;
	}

	animator->Update();
}

void EnemyGolem::updateNormal()
{
	rootNode->Run();	// ビヘイビアツリーの実行
}

void EnemyGolem::updateDamage()
{
	state = stFlash;
	flashTimer = MaxFlashTime;
}
void EnemyGolem::updateDead()
{
	animator->MergePlay(aDead);
	if (animator->Finished()) {
		DataCarrier* dc = ObjectManager::FindGameObject<DataCarrier>();
		dc->AddScore(MaxHitPoint/10);   // 最大ＨＰの1/10をスコアに加える

		DestroyMe();
	}
}


// ビヘイビアツリーの構築
void EnemyGolem::makeBehaviour()
{
	rootNode = new BehaviourSequence();
	auto* node1 = rootNode->AddChild<BehaviourSelector>();
	auto* node2 = node1->AddChild<BehaviourSequence>();
	node2->AddChild<BehaviourAction>(this, actionIsReach);
	auto* node3 = node2->AddChild<BehaviourSelector>();
	node3->AddChild<BehaviourAction>(this, actionReach);
	node3->AddChild<BehaviourAction>(this, actionAttackFireBall);
	auto* node4 = node1->AddChild<BehaviourSelector>();
	node4->AddChild<BehaviourAction>(this, actionIdle);
	node4->AddChild<BehaviourAction>(this, actionWalk);

}


// ビヘイビアツリーのアクション関数
// (static関数のため関数アドレスthisを引数objInで受け取る)

BehaviourBase::BtState EnemyGolem::actionIsReach(Object3D* objIn)
{
	auto* my = dynamic_cast<EnemyGolem *>(objIn);

	// plyerが視野に入ったかのチェック
	Player* player = ObjectManager::FindGameObject<Player>();
	if (my->CheckReach(player, ReachAngle, ReachDistLimit))
	{
		my->targetPlayer = player;	// ターゲットとなるＰＣ
		return BehaviourBase::bsTrue;	  // 視野に入ったらbsTrue
	}
	else {
		my->targetPlayer = nullptr;
	}
	return BehaviourBase::bsFalse;	  // 視野に入らなかったらbsFalse
}

BehaviourBase::BtState EnemyGolem::actionIdle(Object3D* objIn)
{
	auto* my = dynamic_cast<EnemyGolem *>(objIn);

	my->idleTimer -= 60 * SceneManager::DeltaTime();
	if (my->idleTimer <= 0 || my->targetPlayer != nullptr)
	{
		return BehaviourBase::bsFalse;	  // アイドルが終わったらbsFalse
	}
	my->animator->MergePlay(aIdle);
	return BehaviourBase::bsTrue;		  // アイドル中はbsTrue
}

BehaviourBase::BtState EnemyGolem::actionWalk(Object3D* objIn)   // ルート／エリアに従って移動
{
	auto* my = dynamic_cast<EnemyGolem *>(objIn);

	if (my->navigationArea)
	{
		if (my->MoveToTarget(my->target, MoveSpeed, RotSpeed)) // エリアの目的地へ移動
		{
			my->target = my->GetNextAreaTarget();			   // 到達したら次の目標値に
			my->animator->MergePlay(aIdle);
			my->idleTimer = MaxIdleTime;
			return BehaviourBase::bsFalse;	 // 到達したらbsFalse
		}
	}
	else {
		my->target = my->navigationMap[my->aroundID];  // ルートの目的地
		if (my->MoveToTarget(my->target, MoveSpeed, RotSpeed)) // ルートの目的地へ移動
		{
			my->aroundID = (my->aroundID + 1) % my->navigationMap.size();	 // 到達したら次の目標値に
			my->animator->MergePlay(aIdle);
			my->idleTimer = MaxIdleTime;
			return BehaviourBase::bsFalse;	 // 到達したらbsFalse
		}
	}
	my->animator->MergePlay(aRun);
	return BehaviourBase::bsTrue;	   	 // 移動中(到達しなかったら)はbsTrue
}

BehaviourBase::BtState EnemyGolem::actionReach(Object3D* objIn)   // Playerに向かって移動
{
	auto* my = dynamic_cast<EnemyGolem*>(objIn);

	if (my->targetPlayer == nullptr) {	// ターゲットとなるＰＣが無いとき(普通はあり得ない)
		return BehaviourBase::bsTrue;
	}

	VECTOR3 toPlayer = my->targetPlayer->Position() - my->transform.position;	   // 自分から見たプレイヤーの位置
	if (magnitude(toPlayer) < AttackDistLimit)	 // 攻撃距離に入ったか
	{
		return BehaviourBase::bsFalse;		 // 攻撃に移るときはbsFalse
	}
	else {
		// Reach距離の外に出たら、元に戻る
		if (toPlayer.Length() >= ReachDistLimit) {
			my->animator->MergePlay(aRun);
			return BehaviourBase::bsTrue;	 // Reach終わるときはbsTrue
		}
		my->MoveToTarget(my->targetPlayer->Position(), MoveSpeed, RotSpeed);   // プレイヤーに向かう
	}
	my->animator->MergePlay(aRun);
	return BehaviourBase::bsTrue;  		 // Reach継続のときはbsTrue
}

BehaviourBase::BtState EnemyGolem::actionAttackFireBall(Object3D* objIn)	 // 火の玉攻撃処理
{
	auto* my = dynamic_cast<EnemyGolem*>(objIn);

	if (my->targetPlayer == nullptr) {  	// ターゲットとなるＰＣが無いとき(普通はあり得ない)
		my->animator->MergePlay(aRun);
		return BehaviourBase::bsFalse;
	}

	WeaponManager* wm = ObjectManager::FindGameObject<WeaponManager>();

	my->animator->MergePlay(aAttack1);

	if ((int)(my->animator->CurrentFrame()) == 40)    // 攻撃アニメーションが攻撃態勢に入ったとき
	{ 
		// 攻撃
		wm->SpawnMany<WeaponFireBall>(my->transform.position + VECTOR3(0, 12, 0), my->targetPlayer->Collider().center, WeaponBase::eENM);
	}
	if (my->animator->Finished()) // 攻撃アニメーションが終了？
	{
		my->animator->Play(aRun);
		return BehaviourBase::bsTrue;	  // 攻撃アニメーションが正常に終了したときはbsTrue
	}

	return BehaviourBase::bsRunning;   // 攻撃中のときはbsRunning
}

BehaviourBase::BtState EnemyGolem::actionAttackFlame(Object3D* objIn)	 // 火炎攻撃処理
{
	auto* my = dynamic_cast<EnemyGolem*>(objIn);

	if (my->targetPlayer == nullptr) {  	// ターゲットとなるＰＣが無いとき(普通はあり得ない)
		my->animator->MergePlay(aRun);
		return BehaviourBase::bsFalse;
	}

	WeaponManager* wm = ObjectManager::FindGameObject<WeaponManager>();

	my->animator->MergePlay(aAttack1);

	if (((int)(my->animator->CurrentFrame())) > 40 && 
		((int)(my->animator->CurrentFrame())) % 4 == 0)    // 攻撃アニメーション
	{ 
		// 攻撃
		wm->SpawnMany<WeaponFireBall2>(my->transform.position + VECTOR3(0, 12, 0), my->targetPlayer->Collider().center, WeaponBase::eENM);
	}
	if (my->animator->Finished()) // 攻撃アニメーションが終了？
	{
		my->animator->Play(aRun);
		return BehaviourBase::bsTrue;	  // 攻撃アニメーションが正常に終了したときはbsTrue
	}

	return BehaviourBase::bsRunning;   // 攻撃中のときはbsRunning
}

BehaviourBase::BtState EnemyGolem::actionAttackSelect(Object3D* objIn)	 // 選択処理
{
	if (Random(1, 5) == 1) return BehaviourBase::bsFalse;
	return BehaviourBase::bsTrue;
}