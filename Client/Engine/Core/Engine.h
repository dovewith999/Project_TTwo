#pragma once
#include "Core.h"
#include "Input.h"
#include <Windows.h>

// 엔진 설정 구조체
struct EngineSettings
{
	// 콘솔 화면 가로 크기
	int width = 0;

	// 콘솔 화면 세로 크기
	int height = 0;

	// 타겟 프레임 속도
	float framerate = 0.f;
};

class Level;
class Engine_API Engine
{
public:
	// 생성자, 소멸자
	Engine();
	virtual ~Engine();

public:
	static Engine& GetInstance();
	
public:
	// 엔진 실행함수
	void Run();

	void AddLevel(Level* newLevel);

	virtual void CleanUp();

	// 엔진 종료함수
	void Quit();

private: /*Function*/
	void BeginPlay();
	void Tick(float deltaTime = 0.0f);
	void Render();

	// 엔진 설정 로드 함수
	void LoadEngineSettings();
protected:
	// 엔진 종료 플래그
	bool isQuit = false; 
	
	// 메인 레벨
	Level* mainLevel = nullptr;

	// 입력 관리자
	Input input;

	// 엔진 설정
	EngineSettings settings;

	static Engine* instance;
};