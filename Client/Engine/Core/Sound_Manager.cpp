#include "..\Public\Sound_Manager.h"

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager()
{
}

HRESULT CSound_Manager::Initialize()
{
	//// 사운드를 담당하는 대표객체를 생성하는 함수
	//FMOD_System_Create(&m_pSystem, FMOD_VERSION);

	//// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
	//FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	if (FMOD_OK != FMOD::System_Create(&m_pSystem))
		return E_FAIL;

	if (FMOD_OK != m_pSystem->init(32, FMOD_INIT_NORMAL, NULL))
		return E_FAIL;

	LoadSoundFile();

	return S_OK;
}

void CSound_Manager::PlaySound(TCHAR * pSoundKey, CHANNELID eID, float fVolume)
{
	m_fOriginVolume[eID] = fVolume;

	map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	_bool bPlay = false;
	m_pChannelArr[eID]->isPlaying(&bPlay);
	if (!bPlay)
	{
		m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[eID]);
	}

	m_pChannelArr[eID]->setVolume(m_fOriginVolume[eID]);
	m_pSystem->update();
}

void CSound_Manager::PlayBGM(TCHAR * pSoundKey, float fVolume)
{
	map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
	{
		return !lstrcmp(pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[SOUND_BGM]);
	m_pChannelArr[SOUND_BGM]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[SOUND_BGM]->setVolume(fVolume);
	m_pSystem->update();

	/*FMOD_System_PlaySound(m_pSystem, iter->second ,nullptr, FALSE, &m_pChannelArr[SOUND_BGM]);
	FMOD_Channel_SetMode(m_pChannelArr[SOUND_BGM], FMOD_LOOP_NORMAL);
	FMOD_Channel_SetVolume(m_pChannelArr[SOUND_BGM], fVolume);
	FMOD_System_Update(m_pSystem);*/
}


void CSound_Manager::StopSound(CHANNELID eID)
{
	m_pChannelArr[eID]->stop();
}

void CSound_Manager::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		m_pChannelArr[i]->stop();
}

void CSound_Manager::SetChannelVolume(CHANNELID eID, float fVolume)
{
	m_pChannelArr[eID]->setVolume(fVolume);
	m_pSystem->update();
}

void CSound_Manager::SetChannelOffsetVolume(CHANNELID eID, float fVolume)
{
	m_fOffsetVolume[eID] = fVolume;

	m_pChannelArr[eID]->setVolume(m_fOriginVolume[eID] * m_fOffsetVolume[eID] * m_fTotalVolume);
	m_pSystem->update();
}

void CSound_Manager::SetAllChannelVolume(float fVolume)
{
	m_fTotalVolume = fVolume;
}

void CSound_Manager::Play_Sound(const _tchar * _pSoundKey, CHANNELID _eChennelID, EPlayMode _ePlayMode, _float _fVolume)
{
	m_fOriginVolume[_eChennelID] = _fVolume;

	map<TCHAR*, FMOD::Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
	{
		return !lstrcmp(_pSoundKey, iter.first);
	});

	if (iter == m_mapSound.end())
		return;

	switch (_ePlayMode)
	{
	case Engine::CSound_Manager::PLAY_MODE_ONCE:
	{
		m_pChannelArr[_eChennelID]->stop();
		m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[_eChennelID]);
	}
	break;
	case Engine::CSound_Manager::PLAY_MODE_ONCE_IF_NOT_PLAYING:
	{
		_bool bIsPlaying = false;
		m_pChannelArr[_eChennelID]->isPlaying(&bIsPlaying);

		if (false == bIsPlaying)
		{
			m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[_eChennelID]);
		}
	}
	break;
	case Engine::CSound_Manager::PLAY_MODE_LOOP:
	{
		m_pChannelArr[_eChennelID]->stop();
		m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[_eChennelID]);
		m_pChannelArr[_eChennelID]->setMode(FMOD_LOOP_NORMAL);
	}
	break;
	case Engine::CSound_Manager::PLAY_MODE_LOOP_IF_NOT_PLAYING:
	{
		_bool bIsPlaying = false;
		m_pChannelArr[_eChennelID]->isPlaying(&bIsPlaying);

		if (false == bIsPlaying)
		{
			m_pSystem->playSound(iter->second, 0, false, &m_pChannelArr[_eChennelID]);
		}
		m_pChannelArr[_eChennelID]->setMode(FMOD_LOOP_NORMAL);
	}
	break;
	default:
		break;
	}

	m_pChannelArr[_eChennelID]->setVolume(m_fOriginVolume[_eChennelID]);
	m_pSystem->update();
}

const float CSound_Manager::Get_BGM_Volume() const
{
	float fVolume = 0.f;
	m_pChannelArr[SOUND_BGM]->getVolume(&fVolume);

	return fVolume;
}

const float CSound_Manager::Get_Voice_Volume() const
{
	float fVolume = 0.f;
	//m_pChannelArr[SOUND_VOICE]->getVolume(&fVolume);

	return fVolume;
}

const float CSound_Manager::Get_Effect_Volume() const
{
	float fVolume = 0.f;
	//m_pChannelArr[SOUND_EFFECT]->getVolume(&fVolume);

	return fVolume;
}

const float CSound_Manager::Get_UI_Volume() const
{
	float fVolume = 0.f;
	//m_pChannelArr[SOUND_UI]->getVolume(&fVolume);

	return fVolume;
}

void CSound_Manager::LoadSoundFile()
{
	string pathFirst = "../../Client/Bin/Resource/Sound";
	string dirpath = pathFirst + "/*.*";
	int checkDirFile = 0;
	struct _finddata_t fdF;
	intptr_t handleT;
	if ((handleT = _findfirst(dirpath.c_str(), &fdF)) == -1L)
	{
	#ifdef _DEBUG
		cout << "No file in directory!" << endl;
	#endif // _DEBUG
	}
	do
	{
		if (fdF.attrib & _A_SUBDIR && fdF.name[0] != '.')
		{
			string pathA = pathFirst + "/" + fdF.name + "/*.*";
			string pathB = pathFirst + "/" + fdF.name + "/";

			// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
			_finddata_t fd;

			// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
			intptr_t handle = _findfirst(pathA.c_str(), &fd);

			if (handle == -1)
				return;

			int iResult = 0;

			char szFullPath[128] = "";

			while (-1 != iResult)
			{
				strcpy_s(szFullPath, pathB.c_str());

				// "../ Sound/Success.wav"
				strcat_s(szFullPath, fd.name);

				FMOD::Sound* pSound = nullptr;

				FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_DEFAULT, 0, &pSound);

				if (eRes == FMOD_OK)
				{
					int iLength = (int)strlen(fd.name) + 1;

					TCHAR* pSoundKey = new TCHAR[iLength];
					ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);


					// 아스키 코드 문자열을 유니코드 문자열로 변환시켜주는 함수
					MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

					m_mapSound.emplace(pSoundKey, pSound);
				}
				//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 더이상 없다면 -1을 리턴v 

				iResult = _findnext(handle, &fd);
			}

			_findclose(handle);
		}
	} while (_findnext(handleT, &fdF) == 0);
	_findclose(handleT);


	m_pSystem->update();
}

void CSound_Manager::Free()
{
	for (auto& pair : m_mapSound)
	{
		delete[] pair.first;
		pair.second->release();
	}
	m_mapSound.clear();

	m_pSystem->release();
	m_pSystem->close();
}