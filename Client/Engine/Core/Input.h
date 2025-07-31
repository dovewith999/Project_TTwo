#pragma once
#include "Core.h"

const int MAX_KEY_NUMBER = 256;

class Engine_API Input
{	
	friend class Engine;
	
	/// <summary>
	/// 키입력 확인을 위한 구조체
	/// </summary>
	struct KeyState
	{
		public:
			bool isKeyDown = false;		  // 현재 프레임에 키가 눌렸는지 여부
			bool previousKeyDown = false; // 이전 프레임에 키가 눌렸는지 여부
	};

public:
	Input();

public:
	// 키 확인 함수
	FORCEINLINE bool GetKey(int keyCode) const;
	FORCEINLINE bool GetKeyDown(int keyCode) const;
	FORCEINLINE bool GetKeyUp(int keyCode) const;

	static Input& GetInstance();

private:
	void ProcessInput();
	void SavePreviouseKeyStates();

private:
	static Input* instance;

	KeyState keyStates[MAX_KEY_NUMBER] = {}; // 키 입력 정보 관리 컨테이너
};