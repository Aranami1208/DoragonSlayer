#include "AudioManager.h"
#include "GameMain.h"
#include <algorithm>

namespace {
	struct AudioObject {
		CXAudioSource* audioSource;
		std::string name;
		AudioObject() : audioSource(nullptr) {}
	};
	std::list<AudioObject> audioObjects;
};

void AudioManager::Start()
{
	//   �I�[�f�B�I�ʖ�   �I�[�f�B�I�t�@�C����      �d�ˍ��킹�Đ���
	Push("SeShot"  ,_T("Data/Sound/M_FIRE4.wav"), 2);
	Push("SeLaser" ,_T("Data/Sound/Lazer.wav"), 2);
	Push("SeDamage",_T("Data/Sound/Dead.wav"));
	Push("SeNitro" ,_T("Data/Sound/Nitro2.wav"), 2);
	Push("SeGetit" ,_T("Data/Sound/KI_BH005.wav"));
	Push("SePowerUp" ,_T("Data/Sound/POWER_UP.wav"));
	Push("Bgm1"    ,_T("Data/Sound/DO_HT204.WAV"));
}

void AudioManager::Release()
{
	for (auto it = audioObjects.begin(); it != audioObjects.end();) {
		AudioObject& node = *it;
		SAFE_DELETE( node.audioSource);
		it = audioObjects.erase(it);
	}
	audioObjects.clear();
}

void AudioManager::Push(const std::string name, const TCHAR* audioName, int n)
{
	for (AudioObject obj : audioObjects) {
		if (obj.name == name)
		{
			MessageBox(0, _T("�I�[�f�B�I�ʖ����_�u���Ă��܂�!"), name.c_str(), MB_OK);
			return;
		}
	}
	AudioObject node;
	node.audioSource = new CXAudioSource(audioName, n);
	node.name = name;
	audioObjects.emplace_back(node);
}

bool AudioManager::Delete( std::string name)
{
	bool ret = false;
	for (auto it = audioObjects.begin(); it != audioObjects.end();) {
		AudioObject& node = *it;
		if (node.name == name)
		{
			SAFE_DELETE(node.audioSource);
			it = audioObjects.erase(it);
			ret = true;
			//return ret;
		}
		else {
			it++;
		}
	}
	return ret;
}

 CXAudioSource* AudioManager::Audio( std::string name)
{
	 for (AudioObject obj : audioObjects) {
		 if (obj.name == name) return obj.audioSource;
	 }
	 MessageBox(0, _T("�I�[�f�B�I�\�[�X������܂���!"), name.c_str(), MB_OK);
	 assert(false);
	 return nullptr;
 }
