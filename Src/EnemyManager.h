#pragma once
#include "Animator.h"
#include "TextReader.h"

#include "EnemyBase.h"
#include <string>
#include <list>

#include "EnemyRS.h"
#include "EnemyGolem.h"
#include "EnemyBd.h"
#include "EnemyAndroid.h"
#include "EnemyLava.h"

class EnemyManager : public EnemyBase
{
private:
	struct meshstruct{
		std::string name;
		CFbxMesh* mesh;
		meshstruct()
		{
			mesh = nullptr;
		}
	};

public:
	EnemyManager();
	~EnemyManager();

	CFbxMesh* MeshList(std::string str);

	void Spawn(TextReader* txt, int n);

	/// <summary>
	/// �G�̐��𐔂���
	/// �G�}�l�[�W���[(this)�ƓG�E�n�������
	/// </summary>
	/// <returns>�G�̐�</returns>
	int GetNumber();

private:
	std::list<meshstruct> meshList;
};