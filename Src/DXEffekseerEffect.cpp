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
	rotation.x += XM_PI / 2;		 // 90�xX��]
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

	// �G�t�F�N�g�̈ʒu�E�p�x�E�X�P�[���̍X�V
	SetEffectLocation(transform.position);
	SetEffectRotation(transform.rotation);
	SetEffectScale(transform.scale);

	// ���t���[���A�G�t�F�N�g���Đ��I�����Ă��邩�m�F����
	if (!effek->GetEffekseerManager()->Exists(EffectHandle))
	{
		// �Đ����I�������Ƃ�
		if (isLoop)	 // ���[�v�Đ��̂Ƃ�
		{
			// �V���ɃG�t�F�N�g���Đ��������B���W�̓G�t�F�N�g��\���������ꏊ��ݒ肷��
			Effekseer::Vector3D pos;
			pos.X = transform.position.x;
			pos.Y = transform.position.y;
			pos.Z = transform.position.z;
			EffectHandle = effek->GetEffekseerManager()->Play(EffectR, pos);
		}
		else
		{	// ���[�v�Đ��łȂ��Ƃ�
			isActive = false; // ���g���I���Ƃ���

			if (!dontDestroy)
			{
				DestroyMe();	// ���g���폜����
			}
		}
	}
}
void DXEffekseerEffect::DrawScreen()
{
	if (DelaySpawnTime <= 0)   // �x�����Ԃ��I�����Ă��邩
	{
		// �x�����Ԃ��I�����āA�`��\�̂Ƃ�
		if (isDelaySpawned)
		{
			Effekseer::Vector3D pos;
			pos.X = transform.position.x;
			pos.Y = transform.position.y;
			pos.Z = transform.position.z;
			EffectHandle = effek->GetEffekseerManager()->Play(EffectR, pos);
			isDelaySpawned = false;
		}

		DXEffekseerBase::DrawScreen();	 	   // �`��
	}
}

