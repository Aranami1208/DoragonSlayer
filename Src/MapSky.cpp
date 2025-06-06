#include "MapManager.h"

namespace {
	const float MAP_SKYROTSPEED = 0.005f;
};

MapSky::MapSky()
{
	ObjectManager::SetDrawOrder(this, 10000);   // ��ԉ��ɕ`�悷��

	mesh = new CFbxMesh();
}

MapSky::~MapSky()
{
	SAFE_DELETE(mesh);
}

void MapSky::MakeSkyMap(TextReader* txt, int n)
{
	std::string meshName = txt->GetString(n, 1);
	mesh->Load(meshName.c_str());
}
void MapSky::Update()
{
	// ��̍X�V�@�@�J�����𒆐S�Ƃ����ʒu�ɒu���A�x����]������
	transform.position = GameDevice()->m_vEyePt;
	transform.rotation.y += MAP_SKYROTSPEED * DegToRad * 60 * SceneManager::DeltaTime();	   // ���W�A���p�ɂ���
	if (transform.rotation.y >= 2 * XM_PI) transform.rotation.y -= 2 * XM_PI;
}

void MapSky::DrawDepth()
{
	// �e�����Ȃ�
}
void MapSky::DrawScreen()
{
	VECTOR3 save = GameDevice()->m_vLightDir;
	GameDevice()->m_vLightDir = VECTOR3(0, 0, 0);  // �A�e�̂Ȃ��`��ɂ���
	GameDevice()->m_pD3D->SetZBuffer(false); // Z�o�b�t�@�𖳌���

	Object3D::DrawScreen();

	GameDevice()->m_pD3D->SetZBuffer(true); // Z�o�b�t�@��L����
	GameDevice()->m_vLightDir = save;		// �A�e�̂���`��ɖ߂�
}