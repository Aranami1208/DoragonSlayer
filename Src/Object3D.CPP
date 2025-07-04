#include "Object3D.h"
#include "MeshCollider.h"
#include "Animator.h"

Object3D::Object3D()
{
	animator = nullptr;
	mesh = nullptr;
	meshCol = nullptr;
}

Object3D::~Object3D()
{
	if (animator != nullptr) {
		delete animator;
	}
	// meshとmeshColは削除していない
}

void Object3D::Update()
{
}

void Object3D::Draw()
{
	if (ObjectManager::DrawTimes() == 1)
	{
		// 画面への描画処理
		DrawScreen();
	}
	else if (ObjectManager::DrawTimes() == MAX_CASCADE_SHADOW + 1){
		if (ObjectManager::DrawCounter() == MAX_CASCADE_SHADOW)
		{
			// 画面への描画処理
			DrawScreen();
		}
		else {
			// シャドウマップへの書き込み処理
			DrawDepth();
		}
	}
}

// シャドウマップへの書き込み処理
void Object3D::DrawDepth()
{
	if (mesh == nullptr)  return;
	if (animator == nullptr)
	{
		if (mesh->m_iMaxDevide <= 1)
		{
			mesh->RenderDepth(transform.matrix());
		}
		else {
			mesh->RenderDisplaceDepth(transform.matrix());
		}
	}
	else {
		if (mesh->m_iMaxDevide <= 1)
		{
			mesh->RenderDepth(animator, transform.matrix());
		}
		else {
			mesh->RenderDisplaceDepth(animator, transform.matrix());
		}
	}
}
// 画面への描画処理
void Object3D::DrawScreen()
{
	if (mesh == nullptr)  return;
	if (animator == nullptr)
	{
		if (mesh->m_iMaxDevide <= 1)
		{
			mesh->Render(transform.matrix());
		}
		else {
			mesh->RenderDisplace(transform.matrix());
		}
	}
	else {
		if (mesh->m_iMaxDevide <= 1)
		{
			mesh->Render(animator, transform.matrix());
		}
		else {
			mesh->RenderDisplace(animator, transform.matrix());
		}
	}
}

void Object3D::SetPosition(const VECTOR3& pos)
{
	transform.position = pos;
}

void Object3D::SetPosition(float x, float y, float z)
{
	SetPosition(VECTOR3(x, y, z));
}

void Object3D::SetRotation(const VECTOR3& rot)
{
	transform.rotation = rot;
}

void Object3D::SetRotation(float x, float y, float z)
{
	SetRotation(VECTOR3(x, y, z));
}

void Object3D::SetScale(const VECTOR3& sca)
{
	transform.scale = sca;
}

void Object3D::SetScale(float x, float y, float z)
{
	SetScale(VECTOR3(x, y, z));
}

SphereCollider Object3D::Collider()
{
	SphereCollider sphere;
	sphere.center = transform.position + VECTOR3(0, 1, 0);
	sphere.radius = 0.5f;
	return sphere;
}

bool Object3D::HitSphereToMeshPush(const SphereCollider& sphere, VECTOR3* push)
{
	if (meshCol == nullptr)
		return false;
	MATRIX4X4 mat = transform.matrix();
	std::list<MeshCollider::CollInfo> meshes = meshCol->CheckCollisionSphereList(mat, sphere.center, sphere.radius);
	if (meshes.size() == 0)
		return false;
	if (push != nullptr) { // 座標が必要なのであれば
		VECTOR3 pushVec = VECTOR3(0, 0, 0); // 最終的に押すベクトル
		for (const MeshCollider::CollInfo& m : meshes) { // 当たっているすべてのポリゴンで
			VECTOR3 move = sphere.center - m.hitPosition;
			float len = move.Length(); // 当たった点から中心への距離
			move = move * ((sphere.radius - len) / len);
			VECTOR3 push = m.normal * Dot(move, m.normal); // 押し返したいベクトル
			// 今のpushVecと合成する
			VECTOR3 pushVecNorm = XMVector3Normalize(pushVec); // 合成済みベクトルの向き
			float dot = Dot(push, pushVecNorm);	// その成分の長さ
			if (dot < pushVec.Length()) {
				pushVec += push - pushVecNorm * dot; // その成分を減らしていい
			}
			else {
				pushVec = push;
			}
		}
		*push = pushVec;
	}
	return true;
}


bool Object3D::HitSphereToMesh(const SphereCollider& sphere, MeshCollider::CollInfo* collOut)
{
	if (meshCol == nullptr)
		return false;
	MATRIX4X4 mat = transform.matrix();
	MeshCollider::CollInfo coll;
	bool ret = meshCol->CheckCollisionSphere(mat, sphere.center, sphere.radius, &coll);
	if (ret) {
		if (collOut != nullptr) {
			*collOut = coll;
		}
		return true;
	}
	return false;
}

bool Object3D::HitLineToMesh(const VECTOR3& from, const VECTOR3& to, MeshCollider::CollInfo* collOut)
{
	if (meshCol == nullptr)
		return false;
	MATRIX4X4 mat = transform.matrix();
	MeshCollider::CollInfo coll;
	bool ret = meshCol->CheckCollisionLine(mat, from, to, &coll);
	if (ret) {
		if (collOut != nullptr) {
			*collOut = coll;
		}
		return true;
	}
	return false;
}

float Object3D::HitSphereToSphere(const SphereCollider& target, bool withY)
{
	SphereCollider my = Collider();
	VECTOR3 diff = target.center - my.center;
	if (withY == false)
		diff.y = 0.0f;
	float rsum = target.radius + my.radius;
	if (diff.LengthSquare() < rsum * rsum) {
		return rsum - diff.Length();
	}
	return 0.0f;
}

bool Object3D::HitSphereToSpherePush(const SphereCollider& target, bool withY, VECTOR3* push)
{
	SphereCollider my = Collider();
	VECTOR3 pushVec = my.center - target.center;
	if (withY == false)
		pushVec.y = 0.0f;
	float rsum = my.radius + target.radius;
	if (pushVec.LengthSquare() < rsum * rsum) {	  // 球の当たり判定
		// 当たってるとき
		if (push != nullptr) {
			// 押し出す方向はpushVec  押し出す長さはpushLen
			// 押し出すベクトルpushを求める
			float pushLen = rsum - pushVec.Length();
			pushVec = XMVector3Normalize(pushVec); // pushVecの長さを１にする
			*push = pushVec * pushLen;
		}
		return true;
	}
	return false;
}

SphereCollider  Object3D::GetSphereCollider()
{
	SphereCollider col;
	if (meshCol != nullptr)
	{
		meshCol->GetBall(&col.center, &col.radius);
	}
	return col;
}
