#include "MapManager.h"

MapWave::MapWave()
{
	ObjectManager::SetDrawOrder(this, -200);   // �����F�͒x�߂ɕ`�悷��

	wave = nullptr;
}

MapWave::~MapWave()
{
	SAFE_DELETE(wave);
}

void MapWave::MakeWaveMap(TextReader* txt, int n)
{
	VECTOR2 size;
	VECTOR3 pos;
	size.x = txt->GetFloat(n, 4);
	size.y = txt->GetFloat(n, 5);
	pos.x = txt->GetFloat(n, 1) - size.x/2;	  // ���ʂ̍���O�̈ʒu���w�肷��
	pos.y = txt->GetFloat(n, 2);
	pos.z = txt->GetFloat(n, 3) - size.y/2;	  // ���ʂ̍���O�̈ʒu���w�肷��
	transform.position = pos;
	std::string normalName = txt->GetString(n, 6);
	wave = new CWave(size.x, size.y, normalName.c_str());

	if (txt->GetColumns(n) == 11)
	{
		VECTOR4 col;
		col.x = txt->GetFloat(n, 7);
		col.y = txt->GetFloat(n, 8);
		col.z = txt->GetFloat(n, 9);
		col.w = txt->GetFloat(n, 10);
		wave->m_vDiffuse = col;
	}

}

void MapWave::DrawDepth()
{
	// �e�����Ȃ�
}
void MapWave::DrawScreen()
{
	wave->Render(transform.matrix());
}