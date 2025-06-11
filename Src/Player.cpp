#include "Player.h"
#include "../Libs/Imgui/imgui.h"
#include "EnemyManager.h"
#include "MapManager.h"
#include "WeaponManager.h"
#include "Camera.h"
namespace {
	const float Gravity = 0.025f; // 重力加速度(正の値)
	const float JumpPower = 0.5f;  // ジャンプの初速
	const float RotationSpeed = 3; // 回転速度(度)
	const float MoveSpeed = 0.1f;  // 移動スピード(ルートアニメ以外)
	const float ForwardPower = 1.0f; // 前進スピード倍率(ルートアニメも含む)
	const int   MaxNumber = 3;	   // プレイヤーの数
	const int   MaxHitPoint = 1000; // 体力
	const float MaxFlashTime = 5;
	const float MaxDeadTime = 100;
};

Player::Player():
	Cam(nullptr)
{
	animator = new Animator(); // インスタンスを作成


// Solia
	mesh = new CFbxMesh();
	mesh->Load("Data/Char/Night/Night.mesh");
	mesh->LoadAnimation(aIdle, "Data/Char/Night/Idle.anmx", true);
	mesh->LoadAnimation(aRun, "Data/Char/Night/Walking.anmx", true);
	mesh->LoadAnimation(aRoll, "Data/Char/Night/Roll.anmx", false);
	mesh->LoadAnimation(aAttack1, "Data/Char/Night/AttackLight.anmx", false);
	swordObj = ObjectManager::FindGameObject<WeaponManager>()->Spawn<WeaponSword>(WeaponBase::ePC);	  // 剣の発生　　
	swordObj->SetWeaponSword("Sword", 0, 1, VECTOR3(0, 0, 0), VECTOR3(0.0f, 0.01f, -0.02f), VECTOR3(0.0f, 0.0f, -90.0f));  // 剣のメッシュ、手首の位置(0,43は手首) ,長さはメッシュで指定、アジャストの位置と角度
	swordObj->SetParent(this);
	gunObj = ObjectManager::FindGameObject<WeaponManager>()->Spawn<WeaponGun>(WeaponBase::ePC);	  // 銃の発生
	//gunObj->SetWeaponGun("Pistol", 0, 43, VECTOR3(0,0,0), VECTOR3(0.0f, 0.0f, 0.0f),VECTOR3(180.0f, 0.0f, 90.0f));  // 銃のメッシュ　　手首の位置(0,43は手首) ,銃口位置はメッシュで指定、アジャストの位置と角度
	gunObj->SetWeaponGun("", 0, 1, VECTOR3(0, 0, 0));   // 銃メッシュなし、手首の位置(0,43は手首) ,銃口位置は(0,0,0)
	gunObj->SetParent(this);



	animator->SetModel(mesh); // このモデルでアニメーションする
	animator->Play(aRun);
	animator->SetPlaySpeed(1.0f);

	meshCol = new MeshCollider();
	meshCol->MakeFromMesh(mesh, animator);

	transform.position = VECTOR3(0, 0, 0);
	transform.rotation = VECTOR3(0, 0, 0);
	state = stNormal;
	atcstate = atIdle;
	speedY = 0;
	number = MaxNumber;
	hitPoint = MaxHitPoint;
	flashTimer = 0;
	tag = "";
	CurrentStamina = StaminaGaugeMax;


	velocity = VECTOR3(0,0,0);

	SetDrawOrder(-10);

	swordObj->SetActive(false);

}

Player::~Player()
{
	SAFE_DELETE(mesh);
	SAFE_DELETE(meshCol);
}

float Player::HpdivMax()
{
	return (float)hitPoint / MaxHitPoint;
}

void Player::SetMaxHp()
{
	hitPoint = MaxHitPoint;
}

SphereCollider Player::Collider()
{
	// 少し小さめのバウンディングボールとする
	SphereCollider col;
	col.radius = 0.5f;
	col.center = transform.position + VECTOR3(0, 0.7f, 0);
	return col;
}

void Player::Update()
{
	if (!Cam)
	{
		Cam = ObjectManager::FindGameObject<Camera>();//カメラを取得する
		return;
	}
	

	VECTOR3 positionOld = transform.position;
	velocity = VECTOR3(0, 0, 0);

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
	}

	/*
	ImGui::Begin("Sample");
	ImGui::InputInt("State", (int*)(&state));
	ImGui::InputFloat("SP", &speedY);
	ImGui::End();
	*/

	
	// Enemyにめり込まないようにする
	std::list<EnemyBase*> enemys = ObjectManager::FindGameObjects<EnemyBase>();
	for (EnemyBase* &enm : enemys) {
		VECTOR3 push;
		if (enm->Mesh() == nullptr) continue;
		if( HitSphereToSpherePush(enm->Collider(), false, &push)){
			transform.position += push;
		}
	}
	

 	// マップとの接触判定と自然落下処理
	transform.position.y += speedY;		 // 自然落下速度を加える
	speedY -= Gravity * 60 * SceneManager::DeltaTime();
	MapManager* mm = ObjectManager::FindGameObject<MapManager>();
	if( mm->IsCollisionMoveGravity(positionOld, transform.position) != clFall )
	{
		speedY = 0;
	}

	animator->Update(); // 毎フレーム、Updateを呼ぶ		 // -- 2024.9.5
}

void Player::updateNormal()
{
	switch (atcstate) {
	case atIdle:
		updateNormalWalk();
		break;
	case atWalk:
		updateNormalWalk();
		break;
	case atAttack:
		updateNormalAttack();
		break;
	}
}

void Player::updateNormalWalk()
{
	//ルート２(ナナメ移動用)
	float root2 = 1/ sqrtf(2.0f);
	VECTOR3 Input(0.0f);

	if (GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_W) || GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_UP)) {
		//前進処理
		Input.z += 1.0f;
	}
	if (GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_S) || GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_DOWN)) {
		// 後退
		Input.z += -1.0f;
	}
	if (GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_A) || GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_LEFT)) {
		//左に進む
		Input.x += -1.0f;
	}

	if (GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_D) || GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_RIGHT)) {
		//右に進む
		Input.x += 1.0f;
	}

	if (GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_LSHIFT)) {
		//走る
		Input.x *= 1.5f;

		//スタミナ消費
		CurrentStamina = max(0.0f,CurrentStamina - StaminaCost);

		//ゲージを使い切ったらクールタイムをセット
		if (CurrentStamina < 0 && StaminaCoolCount == 0.0f)
		StaminaCoolCount = StaminaCoolTime;

	}
	else
	{
		StaminaCoolCount = max(0.0f, StaminaCoolCount--);
		if (StaminaCoolCount == 0.0f)
		CurrentStamina = min(StaminaGaugeMax, CurrentStamina + StaminaCost * 0.5f);
	}

	if(Input.Length() == 0.0f) {//移動していなければ攻撃を開始
		if (atcstate == atAttack)
		{
			animator->MergePlay(aAttack1);
		}
		else {
			animator->MergePlay(aIdle);
		}
	}

	//斜め入力の時は速度を変える
	if (Input.x != 0 && Input.z != 0)
	{
		Input.x *= root2;
		Input.z *= root2;
	}

	move(Input);

	//移動していた場合、キャラクターの向く方向を設定
	if (velocity.Length() > 0.0f)
	{
		targetRotY = atan2f(velocity.x, velocity.z);

		float RotDiff = targetRotY - transform.rotation.y;

		//回転方向を最短にするための処理
		while (RotDiff > XM_PI) RotDiff -= XM_2PI;
		while (RotDiff < -XM_PI) RotDiff += XM_2PI;

		//1フレームで回転する量
		float RotationStep = (1.0f / RotationTime) * SceneManager::DeltaTime();

		if (fabs(RotDiff) < RotationStep)
		{
			transform.rotation.y = targetRotY;
		}
		else
		{
			transform.rotation.y += (RotDiff > 0) ? RotationStep : -RotationStep;
		}
	}


	if ( speedY == 0 && GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_SPACE)) {	  // ジャンプボタン
		speedY = JumpPower;
	}

	if (GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_X)) { // ショット攻撃ボタン
		gunObj->ShotLaser(WeaponBase::ePC);
	}

	if ( atcstate != atAttack && GameDevice()->m_pDI->CheckMouse(KD_TRG,DIM_LBUTTON)) { // 剣攻撃ボタン
		animator->MergePlay(aAttack1);
		atcstate = atAttack;
		swordObj->SetActive(true);
	}
	
	if (GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_Q)) { // 火の玉攻撃ボタン
		VECTOR3 startIn = transform.position + VECTOR3(0, 1.0f, 0);
		VECTOR3 targetIn = XMVector3TransformCoord(VECTOR3(0, 1.0f, 30), transform.matrix());
		ObjectManager::FindGameObject<WeaponManager>()->SpawnMany<WeaponFireBall2>(startIn, targetIn, WeaponBase::ePC);
	}
	
	
	if (GameDevice()->m_pDI->CheckKey(KD_DAT, DIK_E)) { // 火炎放射器攻撃ボタン
		VECTOR3 startIn = transform.position + VECTOR3(0, 1.0f, 0);
		VECTOR3 targetIn = XMVector3TransformCoord(VECTOR3(0, 1.0f, 30), transform.matrix());
		ObjectManager::FindGameObject<WeaponManager>()->SpawnFlamethrower(startIn, targetIn, WeaponBase::ePC, tag);
	}
	

	// -100m以下に落ちてしまったときに+2mの位置に戻す処理
	if (transform.position.y < -100.0f)
	{
		transform.position.y = 2.0f;    // +2mの位置
		speedY = 0.0f;        // 自然落下の停止
	}

	transform.position += velocity;	   // 加速度を加える
}


void Player::updateNormalAttack()
{
	if (animator->Finished()) {
		atcstate = atIdle;
		swordObj->SetActive(false);
	}
}

void Player::updateDamage()
{
	state = stFlash;
	flashTimer = MaxFlashTime;
}

void Player::updateDead()
{
	if (animator->Finished())
	{
		if (--number <= 0) // ＰＣを一人減らす
		{
			// ＰＣが全て死亡したらゲームオーバー画面へ
			SceneManager::ChangeScene("OverScene");
		}
		else {
			// 復帰する
			hitPoint = MaxHitPoint;
			state = stFlash;
			flashTimer = MaxDeadTime;
		}
	}
}

void Player::move(VECTOR3 Direction)
{
	//移動入力をされていなければ処理しない
	if (Direction.Length() == 0.0f) return;

	VECTOR3 vec;

	// ルートボーンアニメーションを行うかどうかルートアニメーションタイプを確認する
	if (mesh->GetRootAnimType(animator->PlayingID()) == eRootAnimNone)
	{
		vec = Direction * MoveSpeed * 60 * SceneManager::DeltaTime();
	}
	else {
		// ルートボーンアニメーションでの前進移動値
		vec = GetPositionVector(mesh->GetRootAnimUpMatrices(animator)) * 60 * SceneManager::DeltaTime();
	}
	vec *= ForwardPower;	// 前進のスピード倍率を掛ける
	MATRIX4X4 rotY = XMMatrixRotationY(Cam->GetRotation().y); // Yの回転行列
	velocity += vec * rotY; // キャラの向いてる方への移動量

	//移動量を加算
	transform.position += velocity;

	//攻撃入力されている場合は、MergePlay
	if (atcstate == atAttack)
	{
		animator->MergePlay(aAttack1);
	}
	else {
		animator->MergePlay(aRun);
	}
}

void Player::AddDamage(float damage, VECTOR3 pPos)
{
	if (state != stNormal)	  return;  	  // 平常状態以外は当たり判定はなし（無敵状態）

	hitPoint -= damage;
	if (hitPoint > 0) {
		// まだＨＰが残っているとき
		VECTOR3 push = transform.position - pPos;	// 飛ぶ方向のベクトルを作る
		push.y = 0;
		push = XMVector3Normalize(push) * 0.4f;	// そのベクトルの長さを移動数だけ加える
		transform.position += push;	// transform.positionに移動ベクトルを加える
		transform.rotation.y = atan2f(-push.x, -push.z);   // 移動後ろ方向を向く
		state = stDamage; 	// ダメージ状態にする
	}
	else {
		// ＨＰが０になったとき
		//animator->Play(aDead);
		state = stDead;	 	// 死亡状態にする
	}
}

void Player::DrawScreen()
{
	Object3D::DrawScreen(); // 継承元の描画関数を呼ぶ


}


