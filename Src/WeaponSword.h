#pragma once
#include "WeaponBase.h"
#include "Animator.h"


class WeaponSword : public WeaponBase {
public:
	WeaponSword();
	~WeaponSword();
	void Update() override;
	void DrawScreen() override;
	void Start() override;

	/// <summary>
	/// 武器剣オブジェクトに各種設定値を設定する
	/// </summary>
	/// <param name="name">メッシュ名。メッシュがないときは""</param>
	/// <param name="meshNoIn">メッシュ番号</param>
	/// <param name="boneNoIn">ボーン番号</param>
	/// <param name="offsetIn">オフセット</param>
	/// <param name="posIn">移動調整値</param>
	/// <param name="rotIn">回転調整値</param>
	void SetWeaponSword(std::string name, int meshNoIn, int boneNoIn, VECTOR3 offsetIn = VECTOR3(0,0,0),
											VECTOR3 posIn=VECTOR3(0,0,0), VECTOR3 rotIn=VECTOR3(0,0,0));

	/// <summary>
	/// 武器剣のこのフレーム位置でのワールドマトリックスを得る
	/// </summary>
	/// <returns>武器剣のワールドマトリックス</returns>
	MATRIX4X4 GetFrameMatrix();

private:
	int  meshNo;
	int  boneNo;
	MATRIX4X4 adjustMatrix;
	MATRIX4X4 worldMatrix;
	VECTOR3 offset;
};