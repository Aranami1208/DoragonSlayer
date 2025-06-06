#include "MapManager.h"
#include "HDRManager.h"

MapManager::MapManager()
{
	ObjectManager::SetVisible(this, false);		// ���͕̂\�����Ȃ�

	colMesh = new CCollision();     // �R���W�����}�b�v�̐���
}

MapManager::~MapManager()
{
	SAFE_DELETE(colMesh);

}
void MapManager::MakeMap(TextReader* txt, int n)
{
	std::string str = txt->GetString(n, 0);

	if (str == "MapStage")
	{
		MapStage* msObj = Instantiate<MapStage>();
		msObj->MakeStageMap(txt, n);
	}
	else if (str == "MapMove")
	{
		MapMove* msObj = Instantiate<MapMove>();
		msObj->MakeMoveMap(txt, n);
	}
	else if (str == "MapSky")
	{
		MapSky* msObj = Instantiate<MapSky>();
		msObj->MakeSkyMap(txt, n);
	}
	else if (str == "MapWave")
	{
		MapWave* msObj = Instantiate<MapWave>();
		msObj->MakeWaveMap(txt, n);
	}
	else if (str == "MapTree")
	{
		MapTree* msObj = Instantiate<MapTree>();
		msObj->MakeTreeMap(txt, n);
	}
	else if (str == "MapFog")
	{
		int   fog        = txt->GetInt(n, 1);
		float fogVolume  = txt->GetFloat(n, 2);
		float fogDensity  = txt->GetFloat(n, 3);
		float fogHeight  = txt->GetFloat(n, 4);
		VECTOR4 fogColor;
		fogColor.x = txt->GetFloat(n, 5);
		fogColor.y = txt->GetFloat(n, 6);
		fogColor.z = txt->GetFloat(n, 7);
		fogColor.w = txt->GetFloat(n, 8);

		// �t�H�O�̐ݒ�  SetConstantBufferFog�֐� 
		GameDevice()->m_pShader->SetConstantBufferFog(fog, fogVolume, fogDensity, fogHeight, fogColor);
	}
	else if (str == "MapHDR")
	{
		// HDR�̐ݒ�
		ObjectManager::FindGameObject<HDRManager>()->SetLimitForce(txt->GetFloat(n, 1), txt->GetFloat(n, 2));
	}
	else if (str == "MapFluid")
	{
		MapFluid* msObj = Instantiate<MapFluid>();
		msObj->MakeFluidMap(txt, n);
	}
}

bool MapManager::IsCollisionLay(const VECTOR3& startIn, const VECTOR3& endIn, VECTOR3& hit, VECTOR3& normal)
{
	bool ret = false, retMove = false;

	// �ړ��}�b�v�Ƃ̐ڐG����
	std::list<MapMove*> mml = ObjectManager::FindGameObjects<MapMove>();
	for (MapMove*& mm : mml)
	{
		if (mm->ColMoveMesh() && mm->ActiveOn())
		{
			retMove = mm->ColMoveMesh()->IsCollisionLay(startIn, endIn, hit, normal);
			if (retMove == true) break;   // �ړ��}�b�v�ƐڐG�����Ƃ�
		}
	}

	// �ʏ�̃}�b�v�Ƃ̐ڐG����
	if (colMesh)
	{
		ret = colMesh->IsCollisionLay(startIn, endIn, hit, normal);
	}

	if (retMove == true)  // �ړ��}�b�v�ƐڐG���Ă����Ƃ�
	{
		return retMove;
	}
	else {
		return ret;
	}
}
bool MapManager::IsCollisionSphere(const VECTOR3& startIn, const VECTOR3& endIn, const float& radius, VECTOR3& hit, VECTOR3& normal)
{
	bool ret = false, retMove = false;

	// �ړ��}�b�v�Ƃ̐ڐG����
	std::list<MapMove*> mml = ObjectManager::FindGameObjects<MapMove>();
	for (MapMove*& mm : mml)
	{
		if (mm->ColMoveMesh() && mm->ActiveOn())
		{
			retMove = mm->ColMoveMesh()->IsCollisionSphere(startIn, endIn, radius, hit, normal);
			if (retMove == true) break;   // �ړ��}�b�v�ƐڐG�����Ƃ�
		}
	}

	// �ʏ�̃}�b�v�Ƃ̐ڐG����
	if (colMesh)
	{
		ret = colMesh->IsCollisionSphere(startIn, endIn, radius, hit, normal);
	}

	if (retMove == true)  // �ړ��}�b�v�ƐڐG���Ă����Ƃ�
	{
		return retMove;
	}
	else {
		return ret;
	}
}
bool MapManager::IsCollisionMove(const VECTOR3& positionOld, VECTOR3& position, float radius)
{
	VECTOR3 hit, normal;
	return IsCollisionMove(positionOld, position, hit, normal, radius);
}
bool MapManager::IsCollisionMove(const VECTOR3& positionOld, VECTOR3& position, VECTOR3& hit, VECTOR3& normal, float radius)
{
	bool ret = false, retMove =false;

	// �ړ��}�b�v�Ƃ̐ڐG����ƈړ�
	std::list<MapMove*> mml = ObjectManager::FindGameObjects<MapMove>();
	for (MapMove*& mm : mml)
	{
		if (mm->ColMoveMesh() && mm->ActiveOn())
		{
			retMove = mm->ColMoveMesh()->IsCollisionMove(positionOld, position, hit, normal, radius);
			if (retMove == true) break;   // �ړ��}�b�v�ƐڐG�����Ƃ�
		}
	}

	// �ʏ�̃}�b�v�Ƃ̐ڐG����ƈړ�
	if (colMesh)
	{
		ret = colMesh->IsCollisionMove(positionOld, position, hit, normal, radius);
	}

	if (retMove == true)  // �ړ��}�b�v�ƐڐG���Ă����Ƃ�
	{
		return retMove;
	}
	else {
		return ret;
	}
}

CollRet  MapManager::IsCollisionMoveGravity(const VECTOR3& positionOld, VECTOR3& position, float radius)
{
	VECTOR3 hit, normal;
	return IsCollisionMoveGravity(positionOld, position, hit, normal, radius);
}
CollRet  MapManager::IsCollisionMoveGravity(const VECTOR3& positionOld, VECTOR3& position, VECTOR3& hit, VECTOR3& normal, float radius)
{
	CollRet ret = clError, retMove = clError;

	// �ړ��}�b�v�Ƃ̐ڐG����ƈړ�
	std::list<MapMove*> mml = ObjectManager::FindGameObjects<MapMove>();
	for (MapMove* &mm : mml)
	{
		if (mm->ColMoveMesh() && mm->ActiveOn())
		{
			retMove = mm->ColMoveMesh()->IsCollisionMoveGravity(positionOld, position, hit, normal, radius);
			if (retMove == clMove || retMove == clLand)
				break;   // �ړ��}�b�v�ƐڐG�����Ƃ�
		}
	}

	// �ʏ�̃}�b�v�Ƃ̐ڐG����ƈړ�
	if (colMesh)
	{
		ret = colMesh->IsCollisionMoveGravity(positionOld, position, hit, normal, radius);
	}

	if (retMove == clMove || retMove == clLand)  // �ړ��}�b�v�ƐڐG���Ă����Ƃ�
	{
		return retMove;
	}
	else {
		return ret;
	}
}


