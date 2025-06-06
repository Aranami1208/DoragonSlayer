#include "EnemyManager.h"
#include "EnemyBase.h"
#include "Player.h"
#include "DataCarrier.h"


EnemyBase::EnemyBase()
{
	state = stNormal;
	speedY = 0;
	navigationArea = false;
	target = VECTOR3(0, 0, 0);
	hitPoint = 0;
	flashTimer= 0;
	idleTimer= 0;
}

EnemyBase::~EnemyBase()
{
	navigationMap.clear();
	navigationMap.shrink_to_fit();
}

void EnemyBase::DrawScreen()
{
	Object3D::DrawScreen();
	DisplayName();
}

void EnemyBase::MakeNavigationMap(bool isArea, std::vector<VECTOR3> nvIn)
{
	navigationArea = isArea;
	navigationMap.clear();
	navigationMap.shrink_to_fit();
	for (const VECTOR3& nv : nvIn)
	{
		navigationMap.emplace_back(nv);
	}
	if (navigationMap.size() > 0)
	{
		if (navigationArea)
		{
			transform.position = GetNextAreaTarget();
			target = transform.position;
		}
		else {
			transform.position = navigationMap[0];
			target = transform.position;
		}
	}
}

void EnemyBase::AddDamage(float damage, VECTOR3 pPos)
{
	if (state != stNormal )	  return;	  // 平常状態以外は当たり判定はなし（無敵状態）

	hitPoint -= damage;
	if (hitPoint > 0) {
		// まだＨＰが残っているとき
		VECTOR3 push = transform.position - pPos;	// 飛ぶ方向のベクトルを作る
		push.y = 0;
		push = XMVector3Normalize(push) * 0.4f;	// そのベクトルの長さを移動数だけ加える
		transform.position += push;	// 自分のtransform.positionに移動ベクトルを加える
		transform.rotation.y = atan2f(-push.x, -push.z);   // 移動後ろ方向を向く
		state = stDamage;  	// ダメージ状態にする
	}
	else {
		// ＨＰが０になったとき
		state = stDead;  	// 死亡状態にする
	}
}

VECTOR3 EnemyBase::GetNextAreaTarget()
{
	VECTOR3 target = VECTOR3(0,0,0);

	// 次の目的地を求めるときの最低距離を求める
	// navigationMap[0]にMinが、navigationMap[1]にMaxが入っている
	float lx = navigationMap[1].x - navigationMap[0].x;	        // Ｘ方向の長さ
	float lz = navigationMap[1].z - navigationMap[0].z;	        // Ｚ方向の長さ
	float len = (lx < lz) ? lx : lz;  // 狭い方の長さを求める
	len = len * 0.5f * 0.99f;        // 狭い方の長さの半分弱を求める
	if (len > 10.0f) len = 10.0f;    // 最大10ｍを最低距離とする。

	// 次の目的地を乱数で求める
	do {
		target.x = (float)Random(navigationMap[0].x, navigationMap[1].x);	// 次の目的地（target）をセットする
		target.z = (float)Random(navigationMap[0].z, navigationMap[1].z);
	} while (magnitude(target - transform.position) < len);  // 目的地が現在地からlen以内のときはやり直し

	return target;
}

bool EnemyBase::MoveToTarget(VECTOR3 target, float speed, float rotSpeed)
{
	const float NearLimit = 0.5f;

	VECTOR3 toTarget = target - transform.position;
	if (magnitude(toTarget) <= NearLimit) return true;	 // 目的地に到達

	// 自分の正面のベクトルを作る
	MATRIX4X4 myRot = XMMatrixRotationY(transform.rotation.y);
	VECTOR3 front = VECTOR3(0, 0, 1) * myRot;
	//transform.position += front; // とりあえず

	float forward = speed;

	// ルートボーンアニメーションを行うかどうかルートアニメーションタイプを確認する
	if (mesh->GetRootAnimType(animator->PlayingID()) != eRootAnimNone)
	{
		// ルートボーンアニメーションでの前進移動値
		forward = GetPositionVector(mesh->GetRootAnimUpMatrices(animator)).z;
	}
	transform.position += front * forward * 60 * SceneManager::DeltaTime(); // とりあえず
	VECTOR3 right = VECTOR3(1, 0, 0) * myRot;

	// プレイヤーへのベクトルの長さ１のもの
	VECTOR3 toTarget1 = XMVector3Normalize(toTarget);
	// 内積を取る →これがcosθ
	float ip = Dot(front, toTarget1);
	float rSpeed = rotSpeed * DegToRad * 60 * SceneManager::DeltaTime();
	if (ip >= cosf(rSpeed)) {
		transform.rotation.y = atan2f(toTarget.x, toTarget.z);
	}
	else if (Dot(right, toTarget) > 0) {
		transform.rotation.y += rSpeed; // 右に曲げる
	}
	else {
		transform.rotation.y -= rSpeed; // 左に曲げる
	}

	return false;
}

bool EnemyBase::CheckReach(Object3D* player, float angle, float ReachDistLimit)
{
	auto* playObj = dynamic_cast<Player*>(player);
	if (playObj != nullptr && !playObj->IsNormal())	 // プレイヤーがNormal以外のときは対象としない
	{
		return false;
	}

	VECTOR3 toPlayer = player->Position() - transform.position;	   // 自分から見たプレイヤーの位置

	if (toPlayer.Length() < ReachDistLimit / 3)  return true; // 近接リミットの1/3まで近づいたときは無条件で近接。

	if (toPlayer.Length() < ReachDistLimit) {
		// 自分の正面のベクトルを作る
		VECTOR3 front = VECTOR3(0, 0, 1) * XMMatrixRotationY(transform.rotation.y);
		// プレイヤーへのベクトルの長さ１のもの
		VECTOR3 toPlayer1 = XMVector3Normalize(toPlayer);
		// 内積を取る →これがcosθ
		float ip = Dot(front, toPlayer1);
		if (ip >= cosf(angle * DegToRad)) {
			// 視野に入った
			return true;
		}
	}
	return false;
}


SphereCollider EnemyBase::Collider()
{
	SphereCollider col;
	col.radius = 1.5f;
	col.center = transform.position + VECTOR3( 0, col.radius, 0 );
	return col;
}

