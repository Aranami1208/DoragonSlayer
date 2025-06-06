#include "Item.h"
#include "MapManager.h"

namespace {
	const float RotSpeed = 0.03f;
}

Item::Item()
{
	radius = 0.0f;
	rotUp = VECTOR3(0, 0, 0);
}

Item::~Item()
{
	SAFE_DELETE(mesh);
	SAFE_DELETE(meshCol);
}

void Item::MakeItem(TextReader* txt, int n)
{
	VECTOR3 pos, rot;
	int colmn = txt->GetColumns(n);

	pos.x = txt->GetFloat(n, 1);
	pos.y = txt->GetFloat(n, 2);
	pos.z = txt->GetFloat(n, 3);
	transform.position = pos;

	if ( colmn == 7 || colmn == 8)
	{
		std::string str = txt->GetString(n, 4);
		//if (std::all_of(str.cbegin(), str.cend(), isdigit))	   // �S�Đ�����
		if (std::any_of(str.cbegin(), str.cend(), isdigit))	   // �������܂�ł��邩
		{
			rot.x = 0;
			rot.y = txt->GetFloat(n, 4);
			rot.z = 0;
			transform.rotation = rot * DegToRad;	// ���W�A���p�ɂ���
		}
		else {
			if (str == "RotX")
			{
				rotUp.x = RotSpeed;
			}
			else if (str == "RotY")
			{
				rotUp.y = RotSpeed;
			}
			else if (str == "RotZ")
			{
				rotUp.z = RotSpeed;
			}
			else if (str == "RotXY")
			{
				rotUp.x = RotSpeed;
				rotUp.y = RotSpeed;
			}
			else if (str == "RotXZ")
			{
				rotUp.x = RotSpeed;
				rotUp.z = RotSpeed;
			}
		}

		std::string tag = txt->GetString(n, 5);
		SetTag(tag);

		std::string meshName = txt->GetString(n, 6);
		if (meshName != "")
		{
			mesh = new CFbxMesh();
			mesh->Load(meshName.c_str());
		}
		if (colmn > 7)
		{
			std::string colName = txt->GetString(n, 7);
			if (colName != "")
			{
				meshCol = new MeshCollider();
				meshCol->MakeFromFile(colName);
			}
		}
	}
	else if (colmn == 5 || colmn == 6)
	{
		radius = txt->GetFloat(n, 4);	  // ���a�̐ݒ�
		if (colmn > 5)
		{
			std::string tag = txt->GetString(n, 5);
			SetTag(tag);
		}
	}
}

void Item::Update()
{
	transform.rotation += rotUp;
	if (transform.rotation.x > XM_PI * 2)	transform.rotation.x -= XM_PI * 2;
	if (transform.rotation.y > XM_PI * 2)	transform.rotation.y -= XM_PI * 2;
	if (transform.rotation.z > XM_PI * 2)	transform.rotation.z -= XM_PI * 2;
}

void Item::DrawScreen()
{
	Object3D::DrawScreen();

}
