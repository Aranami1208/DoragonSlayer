#include "Camera.h"
#include "Player.h"
#include "MapManager.h"
//                                      後方視点　　　　　　真上視点　　　　　　　右横視点
static const VECTOR3 CameraPos[] = { VECTOR3(0, 2, -5), VECTOR3(0, 100, -0.5) , VECTOR3(10, 1,  0 )};
static const VECTOR3 LookPos[] =   { VECTOR3(0, 1,  5), VECTOR3(0,  1,  1  )  , VECTOR3(0,  1,  0  ) };
static const float CHANGE_TIME_LIMIT = 0.5f; // 秒

Camera::Camera()
{
	ObjectManager::SetVisible(this, false);		// 自体は表示しない
	SetPriority(-10000); 	 // 最後に処理する
	viewType = 0;
	changeTime = CHANGE_TIME_LIMIT; // 切り替え時間
}

Camera::~Camera()
{
}

void Camera::Update()
{
	// ２つの視点を'L'キーによって切り替える
	if (GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_L)) {
		changePosStart = CameraPos[viewType];
		changeLookStart = LookPos[viewType];
		viewType += 1;
		if (viewType >= sizeof(CameraPos) / sizeof(CameraPos[0])) {
			viewType = 0;
		}
		changePosGoal = CameraPos[viewType];
		changeLookGoal = LookPos[viewType];
		changeTime = 0.0f;
	}

	// プレイヤーの行列を求める
	Player* player = ObjectManager::FindGameObject<Player>();
	MATRIX4X4 rotY = XMMatrixRotationY(player->Rotation().y);
	//MATRIX4X4 trans = XMMatrixTranslation( player->Position().x, 0.0f, player->Position().z);
	MATRIX4X4 trans = XMMatrixTranslationFromVector( player->Position());
	MATRIX4X4 m = rotY * trans;
	// プレイヤーが回転・移動してない時のカメラ位置に
	// プレイヤーの回転・移動行列を掛けると、
	if (changeTime >= CHANGE_TIME_LIMIT) {
		transform.position = CameraPos[viewType] * m;
		lookPosition = LookPos[viewType] * m;
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

