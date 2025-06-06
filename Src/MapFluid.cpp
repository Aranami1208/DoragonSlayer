#include "MapManager.h"
#include "FluidManager.h"
#include "Fluid.h"

MapFluid::MapFluid()
{
	ObjectManager::SetDrawOrder(this, -200);   // �����F�͒x�߂ɕ`�悷��

	m_tag = "";					// �Ή����̂̃^�O�B�����l��""(����)
	m_fCubeSize = 3.0f;			// �\���p�����̂̈�ӂ̑傫��
	m_fColorNo = 1;				// ���̂̐F�R�[�h�i�O�`�S�j
	m_fDensity = 2.0f;			// ���x
	m_fVelocity = 300;			// ���x
	m_fRandom = 0.3f;			// ���x���������_����
	m_nAddVoxelWidth = 0;		// �����ӏ����甭��������Ƃ��̃T�C�Y�i�{�N�Z�����j�B�P�J���̏ꍇ�͂O�ɂ���
}

MapFluid::~MapFluid()
{
	;
}

void MapFluid::MakeFluidMap(TextReader* txt, int n)
{
	VECTOR3 pos;
	pos.x = txt->GetFloat(n, 1);
	pos.y = txt->GetFloat(n, 2);
	pos.z = txt->GetFloat(n, 3);
	transform.position = pos;
	transform.rotation = VECTOR3(0,0,0);
	m_fCubeSize = txt->GetFloat(n, 4);			// �\���p�����̂̈�ӂ̑傫��
	m_fColorNo = txt->GetFloat(n,5);			// ���̂̐F�R�[�h�i�O�`�S�j
	m_fDensity = txt->GetFloat(n,6);			// ���x
	m_fVelocity = txt->GetFloat(n,7);			// ���x
	m_fRandom = txt->GetFloat(n,8);			// ���x���������_����
	m_nAddVoxelWidth = txt->GetFloat(n,9);		// �����ӏ����甭��������Ƃ��̃T�C�Y�i�{�N�Z�����j�B�P�J���̏ꍇ�͂O�ɂ���

}
void MapFluid::Update()
{
	ObjectManager::FindGameObject<FluidManager>()->SpawnY(transform, m_fCubeSize, m_fColorNo, 
										m_fDensity, m_fVelocity, m_fRandom, m_nAddVoxelWidth, m_tag);
}

void MapFluid::DrawDepth()
{
	// �e�����Ȃ�
}
void MapFluid::DrawScreen()
{
	Fluid* obj = ObjectManager::FindGameObjectWithTag<Fluid>(m_tag);
	if (obj != nullptr)
	{
		obj->DrawScreen();
	}
}