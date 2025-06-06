#pragma once
#include "Object3D.h"
#include "Collision.h"

class MapBase : public Object3D
{
public:
	MapBase();
	virtual ~MapBase();

	CFbxMesh* Mesh() { return mesh; }
	CCollision* ColMoveMesh() { return colMoveMesh; }
	virtual void CreateMeshcolliderFromMesh(CFbxMesh* mesh);
	Transform GetTransform() { return transform; }

protected:
	CCollision* colMoveMesh;	// �ړ��R���W�������b�V��
};