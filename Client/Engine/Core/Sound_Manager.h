#pragma once
#include "Base.h"

#include "../../core/inc/fmod.hpp"
#include "../../core/inc/fmod_codec.h"
#include "../../core/inc/fmod_dsp.h"
#include "../../core/inc/fmod_errors.h"
#include "../../core/inc/fmod_output.h"
#include "../../core/inc/fmod_memoryinfo.h"
#include "io.h"
#include "playsoundapi.h"

#pragma comment (lib, "../../core/lib/x64/fmod_vc.lib")

BEGIN(Engine)


class CSound_Manager final : public CBase
{
	DECLARE_SINGLETON(CSound_Manager)

public:
	typedef enum EPlayMode {
		PLAY_MODE_ONCE,
		PLAY_MODE_ONCE_IF_NOT_PLAYING,
		PLAY_MODE_LOOP,
		PLAY_MODE_LOOP_IF_NOT_PLAYING,
		PLAY_MODE_END
	} PLAY_MODE;

private:
	CSound_Manager();
	virtual ~CSound_Manager() = default;

public:
	HRESULT Initialize();

public:
	void PlaySound(TCHAR* pSoundKey, CHANNELID eID, float fVolume);
	void PlayBGM(TCHAR* pSoundKey, float fVolume);
	void StopSound(CHANNELID eID);
	void StopAll();
	void SetChannelVolume(CHANNELID eID, float fVolume);
	void SetChannelOffsetVolume(CHANNELID eID, float fVolume);
	void SetAllChannelVolume(float fVolume);

public:
	void Play_Sound(const _tchar* _pSoundKey, CHANNELID _eChennelID, EPlayMode _ePlayMode = PLAY_MODE_ONCE, _float _fVolume = 1.f); // const _tchar* �� Play (10-30 15:49 - ����)

public:
	const float Get_BGM_Volume() const;
	const float Get_Voice_Volume() const;
	const float Get_Effect_Volume() const;
	const float Get_UI_Volume() const;

private:
	void LoadSoundFile();

private:
	// ���� ���ҽ� ������ ���� ��ü 
	std::map<TCHAR*, FMOD::Sound*> m_mapSound;

	// FMOD_CHANNEL : ����ϰ� �ִ� ���带 ������ ��ü 
	FMOD::Channel* m_pChannelArr[MAXCHANNEL];

	// ���� ,ä�� ��ü �� ��ġ�� �����ϴ� ��ü 
	//FMOD_SYSTEM* m_pSystem;
	FMOD::System* m_pSystem = nullptr;

	float m_fOffsetVolume[MAXCHANNEL] = { 1.f };
	float m_fOriginVolume[MAXCHANNEL] = { 1.f };

	float m_fTotalVolume = 1.f;

public:
	virtual void Free() override;
};

END

