#include "DXEffekseer.h"
#include "DXEffekseerManager.h"
#include <codecvt>

DXEffekseerManager::DXEffekseerManager()
{
	ObjectManager::DontDestroy(this);		// ���̂͏�����Ȃ�
	ObjectManager::SetActive(this, false);		// ���͍̂X�V���Ȃ�
	ObjectManager::SetVisible(this, false);		// ���͕̂\�����Ȃ�

	effectstruct es = {};

	effectList.push_back(es);
	effectList.back().effectR = LoadEffectEffectR("Data/EffekseerEffect/Laser/Laser02.efkefc");
	effectList.back().name = "Laser02";

	effectList.push_back(es);
	effectList.back().effectR = LoadEffectEffectR("Data/EffekseerEffect/Flame/Flame.efkefc");
	effectList.back().name = "Flame";
	effectList.back().radius = 5.0f;

	effectList.push_back(es);
	effectList.back().effectR = LoadEffectEffectR("Data/EffekseerEffect/Magic/Magic3.efkefc");
 	effectList.back().name = "Magic3";
	effectList.back().radius = 3.0f;

	effectList.push_back(es);
	effectList.back().effectR = LoadEffectEffectR("Data/EffekseerEffect/Sword/Sword2.efkefc");
 	effectList.back().name = "Sword2";

	effectList.push_back(es);
	effectList.back().effectR = LoadEffectEffectR("Data/EffekseerEffect/Hiteffect/Hiteffect.efkefc");
	effectList.back().name = "Hiteffect";
	effectList.back().radius = 2.0f;

}

DXEffekseerManager::~DXEffekseerManager()
{
}

Effekseer::EffectRef DXEffekseerManager::EffectList(std::string str, float& radius)
{
	for (effectstruct& es : effectList)
	{
		if (str == es.name)
		{
			radius = es.radius;	  // �ڐG���a
			return es.effectR;	// �G�t�F�N�g���X�g�ɂ�����
		}
	}

	radius = 0;	 	  // �ڐG���a�͂O��

	Effekseer::EffectRef eff;
	eff = LoadEffectEffectR(str.c_str());	// �G�t�F�N�g���X�g�ɂȂ��̂œǂݍ���
	if (eff == nullptr)
	{
		std::string msg = "DXEffekseerManager::effectList()   " + str;
		MessageBox(nullptr, msg.c_str(), _T("������ �w��̃G�t�F�N�g���̃G�t�F�N�g�̓G�t�F�N�g���X�g��t�@�C���ɂ���܂��� ������"), MB_OK);
	}
	return eff;
}

DXEffekseerEffect* DXEffekseerManager::Spawn(std::string name, const VECTOR3& posIn, const bool& isLoopIn)
{
	DXEffekseerEffect* obj = Instantiate<DXEffekseerEffect>();
	if (obj == nullptr)  return nullptr;
	obj->DXEffekseerEffect::Spawn(name, posIn, isLoopIn);
	return obj;
}

DXEffekseerEffect* DXEffekseerManager::Spawn(std::string name, const VECTOR3& posIn, const VECTOR3& rotIn, const bool& isLoopIn)
{
	DXEffekseerEffect* obj = Instantiate<DXEffekseerEffect>();
	if (obj == nullptr)  return nullptr;
	obj->DXEffekseerEffect::Spawn(name, posIn, rotIn, isLoopIn);
	return obj;
}

DXEffekseerEffect* DXEffekseerManager::SpawnFromNormal(std::string name, const VECTOR3& posIn, const VECTOR3& normalIn, const bool& isLoopIn)
{
	DXEffekseerEffect* obj = Instantiate<DXEffekseerEffect>();
	if (obj == nullptr)  return nullptr;
	obj->DXEffekseerEffect::SpawnFromNormal(name, posIn, normalIn, isLoopIn);
	return obj;
}


void DXEffekseerManager::MakeEffekseer(TextReader* txt, int n)
{
	VECTOR3 pos, rot;
	pos.x = txt->GetFloat(n, 1);
	pos.y = txt->GetFloat(n, 2);
	pos.z = txt->GetFloat(n, 3);
	rot.x = txt->GetFloat(n, 4);
	rot.y = txt->GetFloat(n, 5);
	rot.z = txt->GetFloat(n, 6);
	transform.position = pos;
	transform.rotation = rot * DegToRad;	// ���W�A���p�ɂ���
	std::string name = txt->GetString(n, 8);
	DXEffekseerEffect* ee = Spawn( name, transform.position, transform.rotation, true);
	ee->SetTag(txt->GetString(n, 7));
}

