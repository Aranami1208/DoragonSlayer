#include "DXEffekseerBase.h"
#include "GameMain.h"
#include "DXEffekseer.h"

#include <algorithm>
#include <functional>

DXEffekseerBase::DXEffekseerBase()
{
	isActive = false;
	isLoop = false;
	isDelaySpawned = false;
	dontDestroy = false;
	Timer = 0;
	DeathTime = 0.0f;
	DelaySpawnTime = 0;
	EffectR = nullptr;
	EffectHandle = -1;
	radius = 0.0f;

	SetDrawOrder(-200);
}

DXEffekseerBase::~DXEffekseerBase()
{

}

void DXEffekseerBase::Death()
{
	;
}

void DXEffekseerBase::DrawDepth()
{
	;	 // 影は描画しない
}

void DXEffekseerBase::DrawScreen()
{
	DXEffekseer* eff = ObjectManager::FindGameObject<DXEffekseer>();
	eff->BeginEffekseerDraw();
	eff->DrawEffect(EffectHandle);
	eff->EndEffekseerDraw();
}

void DXEffekseerBase::LoadEffect(const TCHAR* effectPath)
{
	EffectR = LoadEffectEffectR(effectPath);
}
Effekseer::EffectRef DXEffekseerBase::LoadEffectEffectR(const TCHAR* effectPath)
{
	WCHAR wstr[512] = { L'\0' };

#if _UNICODE
	// unicodeの場合
	wcscpy_s(wstr, 512, effectPath);
#else
	// マルチバイト文字の場合
	MultiByteToWideChar(CP_OEMCP, MB_PRECOMPOSED, effectPath, -1, wstr, 512);
#endif

	return Effekseer::Effect::Create(ObjectManager::FindGameObject<DXEffekseer>()->GetEffekseerManager(), (char16_t*)wstr);
}

void DXEffekseerBase::SetEffectLocation(const VECTOR3 &argPos)
{
	Effekseer::Vector3D pos;
	pos.X = argPos.x;
	pos.Y = argPos.y;
	pos.Z = argPos.z;
	ObjectManager::FindGameObject<DXEffekseer>()->GetEffekseerManager()->SetLocation(EffectHandle, pos);
}

void DXEffekseerBase::SetEffectRotation(const float angle)
{
	ObjectManager::FindGameObject<DXEffekseer>()->GetEffekseerManager()->SetRotation(EffectHandle, Effekseer::Vector3D(0.0f, 1.0f, 0.0f), angle);
}

void DXEffekseerBase::SetEffectRotation(const VECTOR3& angle)
{
	ObjectManager::FindGameObject<DXEffekseer>()->GetEffekseerManager()->SetRotation(EffectHandle, angle.x, angle.y, angle.z);
}

void DXEffekseerBase::SetEffectScale(const VECTOR3& scale)
{
	ObjectManager::FindGameObject<DXEffekseer>()->GetEffekseerManager()->SetScale(EffectHandle, scale.x, scale.y, scale.z);
}