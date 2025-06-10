#include "Camera.h"
#include "Player.h"
#include "MapManager.h"
//                                      後方視点　　　　
static const VECTOR3 CameraPos = { VECTOR3(0, 2, -5)};
static const VECTOR3 LookPos =   { VECTOR3(0, 1,  5)};
static const float CHANGE_TIME_LIMIT = 0.5f; // 秒

Camera::Camera()
{
	ObjectManager::SetVisible(this, false);		// 自体は表示しない
	SetPriority(-10000); 	 // 最後に処理する
	viewType = 0;
	changeTime = CHANGE_TIME_LIMIT; // 切り替え時間
	LONG HalfHeight = 1080 / 2;
	LONG HalfWidth = 1920 / 2;

	MoucePoint.x = HalfWidth;
	MoucePoint.y = HalfHeight;

	SetCursorPos(HalfWidth, HalfHeight);
}

Camera::~Camera()
{
}

void Camera::Update()
{
	
	//入力を取得 
	CDirectInput* DInput = GameDevice()->m_pDI;

	DInput->ShowMouseCursor(false);

	POINT CurrentPoint;

	GetCursorPos(&CurrentPoint);

	int HalfHeight = 1080 / 2;
	int HalfWidth = 1920 / 2;


	MouceDiff.x = HalfWidth - CurrentPoint.x;
	MouceDiff.y = HalfHeight - CurrentPoint.y;
	
	MoucePoint = CurrentPoint;
	SetCursorPos(HalfWidth, HalfHeight);

	VECTOR3 MoveRate = VECTOR3(0.01f,0.005f,0.0f);



	//カメラローテーション
	transform.rotation = transform.rotation +VECTOR3(-MouceDiff.y * MoveRate.y, -MouceDiff.x * MoveRate.x, 0);

	

	// プレイヤーの行列を求める
	Player* player = ObjectManager::FindGameObject<Player>();
	MATRIX4X4 rot = XMMatrixRotationRollPitchYawFromVector(transform.rotation);
	//MATRIX4X4 trans = XMMatrixTranslation( player->Position().x, 0.0f, player->Position().z);
	MATRIX4X4 trans = XMMatrixTranslationFromVector( player->Position());
	MATRIX4X4 m = rot * trans;
	// プレイヤーが回転・移動してない時のカメラ位置に
	// プレイヤーの回転・移動行列を掛けると、


	

	if (changeTime >= CHANGE_TIME_LIMIT) {
		transform.position = CameraPos * m;
		lookPosition = LookPos * m;
	}
	else { // 視点切り替え中
		//changeTime += 1.0f / 60.0f;
		changeTime += 1.0f * SceneManager::DeltaTime();
		float timeRate = changeTime / CHANGE_TIME_LIMIT; // 0.0～1.0
		float rate = timeRate;
		VECTOR3 position = (changePosGoal - changePosStart) * rate + changePosStart;
		VECTOR3 look = (changeLookGoal - changeLookStart) * rate + changeLookStart;
		transform.position = position * m;
		lookPosition = look * m;
	}


	// カメラが壁にめり込まないようにする
	VECTOR3 start = player->Position() + VECTOR3(0, 1.5f, 0);
	VECTOR3 end = transform.position;
	// startからendに向かうベクトルを作り、長さに0.2を加える
	VECTOR3 camVec = end - start;
	camVec = XMVector3Normalize(camVec) * (camVec.Length() + 0.2f);
	end = start + camVec;

	VECTOR3 hit, normal;
	if (ObjectManager::FindGameObject<MapManager>()->IsCollisionLay(start, end, hit, normal)) {
		end = hit;
	}
	//endから0.02手前に置く;
	transform.position = XMVector3Normalize(camVec) * ((end - start).Length() - 0.02f) + start;

	//向いているベクトルを求める
	ForwardVector = lookPosition - transform.position;

	// ------------------------------------------------------------------
	// カメラ座標をGameMainに設定する
	GameDevice()->m_vEyePt = transform.position; // カメラ座標
	GameDevice()->m_vLookatPt = lookPosition; // 注視点
	GameDevice()->m_mView = XMMatrixLookAtLH( 	    // ビューマトリックス
								transform.position,
								lookPosition,
								VECTOR3(0, 1, 0));
	GameDevice()->m_mDrawView = GameDevice()->m_mView;

	// ------------------------------------------------------------------------
	// ライト視点からのビュートランスフォーム（ShadowMap用）ライト視点をＰＣの位置に合わせる
	GameDevice()->SetLightView();


	// ------------------------------------------------------------------
	// 視点からの距離の２乗をDrawObjectに設定する
	// これは、視点からの距離の降順に描画したいため
	std::list<Object3D*> objList = ObjectManager::FindGameObjects<Object3D>();
	for (Object3D*& obj : objList)
	{
		if (obj != this)
		{
			float distQ = magnitudeSQ(obj->Position() - transform.position);
			ObjectManager::SetEyeDist(obj, distQ);	 // 視点からの距離の２乗をDrawObjectに設定
		}
	}

}


VECTOR3 Camera::GetForwardVector()
{
	return XMVector3Normalize(ForwardVector);
}

VECTOR3 Camera::GetRotation()
{
	return transform.rotation;
}

