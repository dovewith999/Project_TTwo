#include "SoundManager.h"
#include <iostream>
#include <string>

#include <stringapiset.h>

SoundManager::SoundManager()
{
    system = nullptr;

    // 배열 초기화
    for (int i = 0; i < SOUND_CHANNEL_COUNT; ++i)
    {
        channelArr[i] = nullptr;
        originVolume[i] = 1.0f;
        offsetVolume[i] = 1.0f;
    }
    totalVolume = 1.0f;

	Initialize();
}

SoundManager::~SoundManager()
{
	Free();
}

void SoundManager::Initialize()
{
	//// 사운드를 담당하는 대표객체를 생성하는 함수
//FMOD_System_Create(&m_pSystem, FMOD_VERSION);

//// 1. 시스템 포인터, 2. 사용할 가상채널 수 , 초기화 방식) 
//FMOD_System_Init(m_pSystem, 32, FMOD_INIT_NORMAL, NULL);

	if (FMOD_OK != FMOD::System_Create(&system))
	{
		return;
	}

	if (FMOD_OK != system->init(32, FMOD_INIT_NORMAL, NULL))
	{
		return;
	}

	LoadSoundFile();
}



void SoundManager::PlaySound(const TCHAR* pSoundKey, Define::ESoundChannelID eID, float fVolume)
{
    if (!system || !pSoundKey)
        return;

    // std::wstring으로 변환하여 검색
    std::wstring searchKey(pSoundKey);
    auto iter = mapSound.find(searchKey);

    if (iter == mapSound.end())
    {
#ifdef _DEBUG
        //std::wcout << L"Sound not found: " << searchKey << std::endl;

        // 디버깅을 위해 현재 맵에 있는 키들 출력
        //std::wcout << L"Available sounds:" << std::endl;
        //for (const auto& pair : mapSound)
        //{
        //    std::wcout << L"  - " << pair.first << std::endl;
        //}
#endif
        return;
    }

    size_t channelIndex = static_cast<size_t>(eID);
    originVolume[channelIndex] = fVolume;

    // 1. 기존 채널이 있는지 확인
    if (channelArr[channelIndex] != nullptr)  // nullptr 체크 먼저!
    {
        bool isPlaying = false;
        FMOD_RESULT result = channelArr[channelIndex]->isPlaying(&isPlaying);

        if (result == FMOD_OK && isPlaying)
        {
            // 이미 재생 중 - 기존 재생 정지하고 새로 재생
            channelArr[channelIndex]->stop();
            channelArr[channelIndex] = nullptr;
        }
        else
        {
            // 채널이 무효하거나 재생 종료됨
            channelArr[channelIndex] = nullptr;
        }
    }

    // 2. 새로운 채널로 사운드 재생
    FMOD::Channel* newChannel = nullptr;
    FMOD_RESULT result = system->playSound(iter->second, 0, false, &newChannel);

    if (result == FMOD_OK && newChannel != nullptr)
    {
        // 3. 새로 할당된 채널을 배열에 저장
        channelArr[channelIndex] = newChannel;

        // 4. 이제 안전하게 볼륨 설정
        newChannel->setVolume(fVolume);

#ifdef _DEBUG
        //std::wcout << L"Sound played: " << searchKey << L" on channel " << channelIndex << std::endl;
#endif
    }
    else
    {
        // 재생 실패
        channelArr[channelIndex] = nullptr;
    }

    system->update();
}

// 단순하고 안전한 버전 (항상 새로 재생)
void SoundManager::PlaySoundSimple(const TCHAR* pSoundKey, Define::ESoundChannelID eID, float fVolume)
{
    if (!system || !pSoundKey)
        return;

    // 사운드 찾기
    FMOD::Sound* targetSound = nullptr;

    std::wstring searchKey(pSoundKey);
    auto iter = mapSound.find(searchKey);

    if (iter == mapSound.end())
    {
        return;
    }

    targetSound = iter->second;

    if (!targetSound)
        return;

    size_t channelIndex = static_cast<size_t>(eID);

    // 기존 채널이 있으면 정지
    if (channelArr[channelIndex])
    {
        channelArr[channelIndex]->stop();
    }

    // 새로 재생
    FMOD::Channel* newChannel = nullptr;
    FMOD_RESULT result = system->playSound(targetSound, 0, false, &newChannel);

    if (result == FMOD_OK && newChannel)
    {
        channelArr[channelIndex] = newChannel;
        newChannel->setVolume(fVolume);
        originVolume[channelIndex] = fVolume;
    }

    system->update();
}

void SoundManager::StopSound(Define::ESoundChannelID eID)
{
    size_t index = static_cast<size_t>(eID);

    if (index >= SOUND_CHANNEL_COUNT)
        return;

    if (channelArr[index] != nullptr)
    {
        channelArr[index]->stop();
        channelArr[index] = nullptr;
    }
}

void SoundManager::StopAll()
{
    if (!system)
        return;

    for (int i = 0; i < SOUND_CHANNEL_COUNT; ++i)
    {
        if (channelArr[i] != nullptr) // ← nullptr 체크 필수!
        {
            // 추가 안전성 체크
            bool isPlaying = false;
            FMOD_RESULT result = channelArr[i]->isPlaying(&isPlaying);

            if (result == FMOD_OK && isPlaying)
            {
                channelArr[i]->stop();
            }
            channelArr[i] = nullptr;
        }
    }

    system->update();
}

void SoundManager::SetChannelVolume(Define::ESoundChannelID eID, float fVolume)
{
	system->update();
	channelArr[static_cast<size_t>(eID)]->setVolume(fVolume);
}

void SoundManager::SetChannelOffsetVolume(Define::ESoundChannelID eID, float fVolume)
{
	offsetVolume[static_cast<size_t>(eID)] = fVolume;

	channelArr[static_cast<size_t>(eID)]->setVolume(originVolume[static_cast<size_t>(eID)] * offsetVolume[static_cast<size_t>(eID)] * totalVolume);
	system->update();
}

void SoundManager::SetAllChannelVolume(float fVolume)
{
	totalVolume = fVolume;
}

const float SoundManager::Get_BGM_Volume() const
{
	float fVolume = 0.f;
	//channelArr[Define::ESoundChannelID::BGM]->getVolume(&fVolume);

	return fVolume;
}

const float SoundManager::Get_Voice_Volume() const
{
	float fVolume = 0.f;
	//channelArr[SOUND_VOICE]->getVolume(&fVolume);

	return fVolume;
}

const float SoundManager::Get_Effect_Volume() const
{
	float fVolume = 0.f;
	//channelArr[SOUND_EFFECT]->getVolume(&fVolume);

	return fVolume;
}

void SoundManager::LoadSoundFile()
{
    std::string pathFirst = "../Resources/Sounds";
    std::string dirpath = pathFirst + "/*.*";

    struct _finddata_t fdF;
    intptr_t handleT;

    if ((handleT = _findfirst(dirpath.c_str(), &fdF)) == -1L)
    {
#ifdef _DEBUG
        //std::cout << "No file in directory!" << std::endl;
#endif
        return;
    }

    do
    {
        if (fdF.name[0] == '.')
            continue;

        if (fdF.attrib & _A_SUBDIR)
        {
            // 서브디렉토리 처리 (기존 코드와 동일하지만 키 저장 방식 변경)
            std::string pathA = pathFirst + "/" + fdF.name + "/*.*";
            std::string pathB = pathFirst + "/" + fdF.name + "/";

            struct _finddata_t fd;
            intptr_t handle = _findfirst(pathA.c_str(), &fd);

            if (handle == -1)
                continue;

            do
            {
                if (!(fd.attrib & _A_SUBDIR) && fd.name[0] != '.')
                {
                    char szFullPath[256] = "";
                    strcpy_s(szFullPath, sizeof(szFullPath), pathB.c_str());
                    strcat_s(szFullPath, sizeof(szFullPath), fd.name);

                    FMOD::Sound* pSound = nullptr;
                    FMOD_RESULT eRes = system->createSound(szFullPath, FMOD_DEFAULT, 0, &pSound);

                    if (eRes == FMOD_OK)
                    {
                        // std::wstring으로 키 생성
                        int iLength = (int)strlen(fd.name) + 1;
                        TCHAR* pSoundKey = new TCHAR[iLength];
                        ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
                        MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

                        std::wstring soundKey(pSoundKey);
                        mapSound.emplace(soundKey, pSound);

                        delete[] pSoundKey; // 메모리 해제

#ifdef _DEBUG
                        //std::wcout << L"Sound loaded from subfolder: " << soundKey << std::endl;
#endif
                    }
                }
            } while (_findnext(handle, &fd) == 0);

            _findclose(handle);
        }
        else
        {
            // 직접 파일 처리
            char szFullPath[256] = "";
            strcpy_s(szFullPath, sizeof(szFullPath), pathFirst.c_str());
            strcat_s(szFullPath, sizeof(szFullPath), "/");
            strcat_s(szFullPath, sizeof(szFullPath), fdF.name);

            FMOD::Sound* pSound = nullptr;
            FMOD_RESULT eRes = system->createSound(szFullPath, FMOD_DEFAULT, 0, &pSound);

            if (eRes == FMOD_OK)
            {
                // std::wstring으로 키 생성
                int iLength = (int)strlen(fdF.name) + 1;
                TCHAR* pSoundKey = new TCHAR[iLength];
                ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);
                MultiByteToWideChar(CP_ACP, 0, fdF.name, iLength, pSoundKey, iLength);

                std::wstring soundKey(pSoundKey);
                mapSound.emplace(soundKey, pSound);

                delete[] pSoundKey; // 메모리 해제

#ifdef _DEBUG
                //std::wcout << L"Sound loaded: " << soundKey << std::endl;
#endif
            }
        }

    } while (_findnext(handleT, &fdF) == 0);

    _findclose(handleT);

    if (system)
    {
        system->update();
    }
}

void SoundManager::Free()
{
    if (!system)
        return;

    // 1. 채널 정리
    for (int i = 0; i < 5; ++i)
    {
        if (channelArr[i])
        {
            channelArr[i] = nullptr;
        }
    }

    // 2. 시스템 업데이트
    system->update();

    // 3. 사운드 리소스 해제 (const 문제 해결)
    for (auto it = mapSound.begin(); it != mapSound.end(); ++it)
    {
        // 사운드 해제
        if (it->second)
        {
            it->second->release();
            // it->second = nullptr;  // 제거 - iterator는 수정 불가
        }
    }

    // 4. 맵 클리어 (이때 모든 pair가 삭제됨)
    mapSound.clear();


    // 6. 시스템 해제
    system->close();
    system->release();
    system = nullptr;
}