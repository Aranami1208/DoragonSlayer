#include "MapManager.h"

namespace {
	const float MAP_SKYROTSPEED = 0.005f;
};

MapSky::MapSky()
{
	ObjectManager::SetDrawOrder(this, 10000);   // ˆê”Ô‰œ‚É•`‰æ‚·‚é

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
	// ‹ó‚ÌXV@@ƒJƒƒ‰‚ð’†S‚Æ‚µ‚½ˆÊ’u‚É’u‚«A‚xŽ²‰ñ“]‚³‚¹‚é
	transform.position = GameDevice()->m_vEyePt;
	transform.rotation.y += MAP_SKYROTSPEED * DegToRad * 60 * SceneManager::DeltaTime();	   // ƒ‰ƒWƒAƒ“Šp‚É‚·‚é
	if (transform.rotation.y >= 2 * XM_PI) transform.rotation.y -= 2 * XM_PI;
}

void MapSky::DrawDepth()
{
	// ‰e‚ðì‚ç‚È‚¢
}
void MapSky::DrawScreen()
{
	VECTOR3 save = GameDevice()->m_vLightDir;
	GameDevice()->m_vLightDir = VECTOR3(0, 0, 0);  // ‰A‰e‚Ì‚È‚¢•`‰æ‚É‚·‚é
	GameDevice()->m_pD3D->SetZBuffer(false); // Zƒoƒbƒtƒ@‚ð–³Œø‰»

	Object3D::DrawScreen();

	GameDevice()->m_pD3D->SetZBuffer(true); // Zƒoƒbƒtƒ@‚ð—LŒø‰»
	GameDevice()->m_vLightDir = save;		// ‰A‰e‚Ì‚ ‚é•`‰æ‚É–ß‚·
}