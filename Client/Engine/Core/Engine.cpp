#include "Engine.h"
#include <iostream>
#include "Level/Level.h"
#include <Windows.h>
#include "Utils/Utils.h"
#include "Input.h"

Engine* Engine::instance = nullptr;

BOOL WINAPI ConsoleMessageProcedure(DWORD CtrlType)
{
	switch (CtrlType)
	{
	case CTRL_CLOSE_EVENT:
		// Engine의 메모리 해제.
		Engine::GetInstance().CleanUp();
		return false;
		break;
	default:
		break;
	}

	return false;;
}

Engine::Engine()
{
	instance = this;

	// 콘솔 커서 끄기
	CONSOLE_CURSOR_INFO curInfo;
	curInfo.bVisible = false;
	curInfo.dwSize = 1;

	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);

	// 콘솔 창 이벤트 등록
	SetConsoleCtrlHandler(ConsoleMessageProcedure, TRUE);

	LoadEngineSettings();
}

Engine::~Engine()
{
	CleanUp();
}

Engine& Engine::GetInstance()
{
	return *instance;
}

void Engine::Run()
{
	// 밀리 세컨드 단위로 시간을 알려줌
	// float currentTime = timeGetTime();

	LARGE_INTEGER currentTime;	// 현재 시간
	LARGE_INTEGER previousTime; // 이전 시간		
	QueryPerformanceCounter(&currentTime); // 현재 시간의 카운트
	previousTime = currentTime; // 이전 시간 초기화	

	// 하드웨어 시계의 정밀도(주파수) 가져오기
	// 나중에 초로 변환하기 위해
	LARGE_INTEGER frequency; // 카운트의 주파수
	QueryPerformanceFrequency(&frequency); // 카운트의 주파수	

	// 타겟 프레임 지정
	float targetFrameRate = settings.framerate == 0.f ? 60.f : settings.framerate;

	// 타겟 한 프레임 시간
	float oneFrameTime = 1.0f / targetFrameRate;

	while (!isQuit)
	{
		// 프레임 시간 계산
		// 현재 시간 - 이전 시간 / 주파수 = 초 단위의 시간.
		QueryPerformanceCounter(&currentTime);

		// 프레임 시간
		float deltaTime = (currentTime.QuadPart - previousTime.QuadPart) / static_cast<float>(frequency.QuadPart);

		input.ProcessInput();

		if (oneFrameTime <= deltaTime)
		{
			BeginPlay();
			Tick(deltaTime); // deltaTime can be calculated based on frame time
			Render();

			// 제목에 FPS 출력
			//char title[50] = {};
			//sprintf_s(title, 50, "FPS : %f", (1.f / deltaTime));
			//SetConsoleTitleA(title);

			// 시간 갱신
			previousTime = currentTime;

			input.SavePreviouseKeyStates();
		}
	}

	// Engine 정리
	// 텍스트 색상 원래대로 돌려놓기
	Utils::SetConsoleTextColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED); // 흰 색으로 설정
}

void Engine::AddLevel(Level* newLevel)
{
	if (newLevel == nullptr)
	{
		return;
	}

	// 기존에 있던 레벨 제거

	if (mainLevel != nullptr)
	{
		delete mainLevel;
		mainLevel = nullptr;
	}

	mainLevel = newLevel;
}

void Engine::Quit()
{
	isQuit = true;
}

void Engine::BeginPlay()
{
	if (mainLevel == nullptr)
	{
		return;
	}

	mainLevel->BeginPlay();
}

void Engine::Tick(float deltaTime)
{
	if (mainLevel == nullptr)
	{
		return;
	}

	//std::cout << "DeltaTime: " << deltaTime << " FPS: " << (1.0f / deltaTime) << '\n';
	
	// 알파벳은 소문자는 입력이 안됨. 그냥 대문자로 체크해야 함
	//if (GetKeyDown('A'))
	//{
	//	std::cout << "Key Down\n";
	//}
	//if (GetKey('A'))
	//{
	//	std::cout << "Key\n";
	//}
	//if (GetKeyUp('A'))
	//{
	//	std::cout << "Key Up\n";
	//}

	//레벨 업데이트
	mainLevel->Tick(deltaTime);

	//if (GetKeyDown(VK_ESCAPE))
	//{
	//	Quit();
	//}
}

void Engine::Render()
{
	if (mainLevel == nullptr)
	{
		return;
	}

	Utils::SetConsoleTextColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

	mainLevel->Render();
}

void Engine::LoadEngineSettings()
{
	FILE* file = nullptr;
	fopen_s(&file, "../Settings/EngineSettings.txt", "rt");

	if (file == nullptr)
	{
		std::cout << "Failed to load engine settings\n";
		__debugbreak;
		return;
	}

	// 로드
	// FP(File Pointer)를 가장 뒤로 옮기기
	fseek(file, 0, SEEK_END);

	// 이 위치 구하기
	size_t fileSize = ftell(file);

	// 다시 첫 위치로 되돌리기
	rewind(file);

	// 파일 내용을 저장할 버퍼 할당
	char* buffer = new char[fileSize + 1];
	memset(buffer, 0, fileSize + 1);

	size_t readSize = fread(buffer, sizeof(char), fileSize, file);

	// 파싱
	char* context = nullptr;
	char* token = nullptr;
	token = strtok_s(buffer, "\n", &context); // 문자열을 잘라주는 함수

	// 구문 분석
	while (token != nullptr)
	{
		// 키/값 분리
		char header[10] = {};
		// 아래 구문이 제대로 동작하려면 키와 값 사이의 빈칸이 있어야 함
		sscanf_s(token, "%s", header, 10);

		if (strcmp(header, "framerate") == 0)
		{
			sscanf_s(token, "framerate = %f", &settings.framerate);
		}

		else if (strcmp(header, "width") == 0)
		{
			sscanf_s(token, "width = %d", &settings.width);
		}

		else if (strcmp(header, "height") == 0)
		{
			sscanf_s(token, "height = %d", &settings.height);
		}

		// 그 다음 줄 분리
		token = strtok_s(nullptr, "\n", &context); // 문자열을 잘라주는 함수
	}


	SafeDeleteArray(buffer);

	// 파일 닫기
	fclose(file);
}

void Engine::CleanUp()
{
	SafeDelete(mainLevel);
}