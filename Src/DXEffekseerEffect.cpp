#include "DXEffekseerEffect.h"
#include "DXEffekseer.h"

DXEffekseerEffect::DXEffekseerEffect()
{
	effek = ObjectManager::FindGameObject<DXEffekseer>();
	effManager = ObjectManager::FindGameObject<DXEffekseerManager>();
}

DXEffekseerEffect::~DXEffekseerEffect()
{
	if (isActive) Stop();
}

void DXEffekseerEffect::SetEffect(std::string name)
{
	EffectR = effManager->EffectList(name, radius);
}

void DXEffekseerEffect::Spawn(std::string name, const VECTOR3& posIn, const bool& isLoopIn)
{
	if (isActive) return;

	SetEffect(name);

	isActive = true;
	isLoop = isLoopIn;
	isDelaySpawned = false;
	transform.position = posIn;

	Effekseer::Vector3D pos;
	pos.X = posIn.x;
	pos.Y = posIn.y;
	pos.Z = posIn.z;
	EffectHandle = effek->GetEffekseerManager()->Play(EffectR, pos);
}

void DXEffekseerEffect::Spawn(std::string name, const VECTOR3& posIn, const VECTOR3& rotIn, const bool& isLoopIn)
{
	if (isActive) return;

	SetEffect(name);

	isActive = true;
	isLoop = isLoopIn;
	isDelaySpawned = false;
	transform.position = posIn;
	transform.rotation = rotIn;

	Effekseer::Vector3D pos;
	pos.X = posIn.x;
	pos.Y = posIn.y;
	pos.Z = posIn.z;
	EffectHandle = effek->GetEffekseerManager()->Play(EffectR, pos);
}

void DXEffekseerEffect::SpawnFromNormal(std::string name, const VECTOR3& posIn, const VECTOR3& normalIn, const bool& isLoopIn)
{
	if (isActive) return;

	SetEffect(name);

	isActive = true;
	isLoop = isLoopIn;
	isDelaySpawned = false;
	transform.position = posIn;
	transform.rotation = GetNormalRotateVector(normalIn);

	Effekseer::Vector3D pos;
	pos.X = posIn.x;
	pos.Y = posIn.y;
	pos.Z = posIn.z;
	EffectHandle = effek->GetEffekseerManager()->Play(EffectR, pos);
}

void DXEffekseerEffect::Play( const bool& isLoopIn)
{
	if (isActive) return;

	isActive = true;
	isLoop = isLoopIn;

	Effekseer::Vector3D pos;
	pos.X = transform.position.x;
	pos.Y = transform.position.y;
	pos.Z = transform.position.z;
	EffectHandle = effek->GetEffekseerManager()->Play(EffectR, pos);
}

VECTOR3 DXEffekseerEffect::GetNormalRotateVector(const VECTOR3& normalIn)
{
	VECTOR3 rotation = GetLookatRotateVector(VECTOR3(0, 0, 0), normalIn);
	rotation.x += XM_PI / 2;		 // 90度X回転
	return rotation;
}

void DXEffekseerEffect::SetDelayTime(const unsigned int& delaySpawnTimeIn)
{
	DelaySpawnTime = delaySpawnTimeIn;
	if (DelaySpawnTime > 0)
	{
		isDelaySpawned = true;
	}
	else {
		isDelaySpawned = false;
	}
}

void DXEffekseerEffect::Stop()
{
	effek->GetEffekseerManager()->StopEffect(EffectHandle);
}

void DXEffekseerEffect::Update()
{
	if (!isActive) return;

	if (0 < DelaySpawnTime) DelaySpawnTime -= 60 * SceneManager::DeltaTime();

	// エフェクトの位置・角度・スケールの更新
	SetEffectLocation(transform.position);
	SetEffectRotation(transform.rotation);
	SetEffectScale(transform.scale);

	// 毎フレーム、エフェクトが再生終了しているか確認する
	if (!effek->GetEffekseerManager()->Exists(EffectHandle))
	{
		// 再生が終了したとき
		if (isLoop)	 // ループ再生のとき
		{
			// 新たにエフェクトを再生し直す。座標はエフェクトを表示したい場所を設定する
			Effekseer::Vector3D pos;
			pos.X = transform.position.x;
			pos.Y = transform.position.y;
			pos.Z = transform.position.z;
			EffectHandle = effek->GetEffekseerManager()->Play(EffectR, pos);
		}
		else
		{	// ループ再生でないとき
			isActive = false; // 自身を終了とする

			if (!dontDestroy)
			{
				DestroyMe();	// 自身を削除する
			}
		}
	}
}
void DXEffekseerEffect::DrawScreen()
{
	if (DelaySpawnTime <= 0)   // 遅延時間が終了しているか
	{
		// 遅延時間が終了して、描画可能のとき
		if (isDelaySpawned)
		{
			Effekseer::Vector3D pos;
			pos.X = transform.position.x;
			pos.Y = transform.position.y;
			pos.Z = transform.position.z;
			EffectHandle = effek->GetEffekseerManager()->Play(EffectR, pos);
			isDelaySpawned = false;
		}

		DXEffekseerBase::DrawScreen();	 	   // 描画
	}
}

