#pragma once
#include <vector> // 크기가 알아서 변경되는 동적 배열
#include "RTTI.h"

/// <summary>
/// 언리얼의 레벨(맵), 유니티의 씬과 같은 역할을 할 클래스
/// 코드의 활동이 일어나는 곳
/// 강사님 코드를 거진 그대로 가져옴
/// 작성자 : 임희섭
/// 작성일 : 25/08/01
/// </summary>
class Actor;
class Engine_API Level : public RTTI
{
	RTTI_DECLARATIONS(Level, RTTI)

public:
	Level();
	virtual ~Level();

	// 엔진 이벤트 함수
	virtual void BeginPlay();
	virtual void Tick(float deltaTime);
	virtual void Render();
	virtual void Exit();

protected:
	//레벨 자체의 BeginPlay가 호출되어야 할 때 필요한 변수
	bool calledBeginPlay = false;
};

