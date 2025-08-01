#pragma once

#pragma region FMOD
#include "../../Lib/inc/fmod.hpp"
#include "io.h"
#pragma comment (lib, "../../Lib/inc/fmod_vc.lib")
#pragma endregion

#include "../Utils/Define.h"
#include "Etc/Singleton.h"
#include "Engine.h"

#include <map>
#include <string>
#include <iostream>
#include <algorithm>
#include <memory>

// Unicode 관련
#include <tchar.h>
#include <Windows.h>

constexpr size_t SOUND_CHANNEL_COUNT = static_cast<size_t>(Define::ESoundChannelID::MAXCHANNEL);

/// <summary>
/// 사운드 파일 관리 및 재생을 전반적으로 관리할 클래스
/// 작성자 : 임희섭
/// 작성일 : 2025/08/01
/// </summary>
class Engine_API SoundManager  : public SafeSingleton<SoundManager>
{
	friend class SafeSingleton<SoundManager>;

public:
	enum class EPlayMode 
	{
		PLAY_MODE_ONCE,
		PLAY_MODE_ONCE_IF_NOT_PLAYING,
		PLAY_MODE_LOOP,
		PLAY_MODE_LOOP_IF_NOT_PLAYING,
		PLAY_MODE_END
	};

public:
	explicit SoundManager();
	virtual ~SoundManager();

public:
	void Initialize();

public:
	void PlaySound(const TCHAR* pSoundKey, Define::ESoundChannelID eID, float fVolume);
	void PlaySoundSimple(const TCHAR* pSoundKey, Define::ESoundChannelID eID, float fVolume);
	void StopSound(Define::ESoundChannelID eID);
	void StopAll();
	void SetChannelVolume(Define::ESoundChannelID eID, float fVolume);
	void SetChannelOffsetVolume(Define::ESoundChannelID eID, float fVolume);
	void SetAllChannelVolume(float fVolume);

public:
	const float Get_BGM_Volume() const;
	const float Get_Voice_Volume() const;
	const float Get_Effect_Volume() const;

private:
	void LoadSoundFile();

private:
	// 사운드 리소스 정보를 갖는 객체 
	std::map<std::wstring, FMOD::Sound*> mapSound;

	// FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
	FMOD::Channel* channelArr[SOUND_CHANNEL_COUNT];

	// 사운드 ,채널 객체 및 장치를 관리하는 객체 
	FMOD::System* system = nullptr;

	float offsetVolume[SOUND_CHANNEL_COUNT] = { 1.f };
	float originVolume[SOUND_CHANNEL_COUNT] = { 1.f };

	float totalVolume = 1.f;

public:
	void Free();
};

