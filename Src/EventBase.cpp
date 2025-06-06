#include "EventBase.h"
#include "Player.h"
#include "MapManager.h"
#include "Item.h"

namespace {
	const int DelayTime = 60;	 // 削除までの待ち時間
};

EventBase::EventBase()
{
	tag = "";
	needEnter = false;
	onceOn = false;
	eventcycle = ecStandby;
	countTime = 0;
}

EventBase::~EventBase()
{
}

void EventBase::Update()
{
	switch (eventcycle)
	{
	case ecStandby:
		// スタンバイ状態なのでPlayerとの接触判定を行う
		if (HitCheck())
		{
			if (needEnter)
			{
				// Enterキーが必要なときはecWaitにする
				eventcycle = ecWait;
			}
			else {
				// Enterキー不要なときは、実行後ecEndにする
				UpdateEvent();
				countTime = DelayTime;	 // 削除待ち時間を設定
				eventcycle = ecEnd;
			}
		}
		break;

	case ecWait:
		// ここでEnterキー待ちと実行。実行後ecEndにする
		if (GameDevice()->m_pDI->CheckKey(KD_TRG, DIK_RETURN))
		{
			UpdateEvent();
			countTime = DelayTime; 	 // 削除待ち時間を設定
			eventcycle = ecEnd;
		}
		else {
			if (!HitCheck())
			{
				// 接触が離れたので、ecStandbyに戻る。
				eventcycle = ecStandby;
				countTime = 0;
			}
		}
		break;

	case ecEnd:
		if (countTime == 0) UpdateOnce();  // 一度限りのとき、削除待ち時間に達したので削除処理
		if (!HitCheck())
		{
			// 接触が離れた
			if( countTime > 0 )	 UpdateOnce(); // 一度限りのとき、まだ削除時間に達していなかったので削除処理
			countTime = 0;
			eventcycle = ecStandby;	// ecStandbyに戻る。
		}
		else {
			countTime--;
		}
		break;
	}
}
void EventBase::UpdateOnce()
{
	if (onceOn)	   // 一度限りのとき
	{
		DeleteEvent();	// イベント対象のオブジェクトを削除または非表示化
		DestroyMe();	// 自分も削除
	}
}

bool EventBase::HitCheck()
{
	if (tag == "")	return false;

	Player* pc = ObjectManager::FindGameObject<Player>();

	// マップとの接触判定
	MapStage* ms = ObjectManager::FindGameObjectWithTag<MapStage>(tag);
	if (ms != nullptr)
	{
		if (ms->Mesh() == nullptr) return false;
		if (ms->MeshCol() == nullptr)
		{
			ms->CreateMeshcolliderFromMesh(ms->Mesh());  // メッシュコライダーを生成する
		}
		if (ms->HitSphereToMesh(pc->Collider()))
		{
			return true;
		}
		return false;
	}
	else {
		MapTree* mt = ObjectManager::FindGameObjectWithTag<MapTree>(tag);
		if (mt != nullptr)
		{
			if (mt->Mesh() == nullptr) return false;
			if (mt->MeshCol() == nullptr)
			{
				mt->CreateMeshcolliderFromMesh(mt->Mesh());  // メッシュコライダーを生成する
			}
			if (mt->HitSphereToMesh(pc->Collider()))
			{
				return true;
			}
			return false;
		}
	}

	// 移動マップとの接触判定
	MapMove* mm = ObjectManager::FindGameObjectWithTag<MapMove>(tag);
	if (mm != nullptr)
	{
		if (mm->ActiveOn())
		{
			VECTOR3 hit, normal;
			VECTOR3 posStart = VECTOR3(0, pc->Collider().radius, -pc->Collider().radius) * pc->Matrix();
			VECTOR3 posEnd = VECTOR3(0, -pc->Collider().radius, pc->Collider().radius) * pc->Matrix();
			if (mm->ColMoveMesh()->IsCollisionSphere(posStart, posEnd, pc->Collider().radius, hit, normal))
			{
				return true;
			}
		}
		return false;
	}

	// Itemとの接触判定
	Item* it = ObjectManager::FindGameObjectWithTag<Item>(tag);
	if (it != nullptr)
	{
		if (it->MeshCol() != nullptr)
		{
 			if (it->HitSphereToMesh(pc->Collider()))
			{
				return true;
			}
			return false;
		}
		else if (it->Radius() > 0)
		{
			if (magnitude(pc->Position() - it->Position()) < it->Radius())
			{
				return true;
			}
			return false;
		}
	}

	return false;
}

void EventBase::DeleteEvent()
{
	if (tag == "")	return;

	// ステージマップと樹マップの削除は行わない!!
	
	// 移動マップの非表示化処理
	MapMove* mm = ObjectManager::FindGameObjectWithTag<MapMove>(tag);
	if (mm != nullptr)
	{
		mm->SetActiveOn(false);
	}

	// Itemの削除処理
	Item* it = ObjectManager::FindGameObjectWithTag<Item>(tag);
	if (it != nullptr)
	{
		it->DestroyMe();
	}

}


SphereCollider EventBase::Collider()
{
	SphereCollider col;
	col.radius = 1.5f;
	col.center = transform.position + VECTOR3( 0, col.radius, 0 );
	return col;
}

void EventBase::DrawScreen()
{
	Object3D::DrawScreen();

	if (eventcycle == ecWait)  // ここでEnterキー待ちの表示。
	{
		DrawPushEnter();
	}
}

// Enterキー待ちの表示
void EventBase::DrawPushEnter()
{
	Player* pc = ObjectManager::FindGameObject<Player>();
	VECTOR3 pos = pc->Position();
	pos.y += pc->Collider().center.y + pc->Collider().radius + 0.7f;
	GameDevice()->m_pFont->Draw3D(pos, "Push Enter Key!!", VECTOR2(1.0f, 0.2f), RGB(255, 0, 0), 1.0f, _T("HGP創英角ｺﾞｼｯｸUB"));
}