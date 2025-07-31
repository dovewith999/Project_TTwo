#pragma once
#include <vector> // 크기가 알아서 변경되는 동적 배열
#include "RTTI.h"

class Actor;
class Engine_API Level : public RTTI
{
	RTTI_DECLARATIONS(Level, RTTI)

public:
	Level();
	virtual ~Level();

	// 레벨에 액터를 추가할 때 사용
	void AddActor(Actor* newActor);
	
	// 액터 삭제 함수
	void DestroyActor(Actor* destroyedActor);

	// 엔진 이벤트 함수
	virtual void BeginPlay();
	virtual void Tick(float deltaTime);
	virtual void Render();

	// 추가 및 삭제 요청된 액터를 처리하는 함수
	void ProcessAddAndDestroyActors();

private:
	void SortActorsBySortingOrder();

protected:
	// 자식에서는 접근할 수 있어야 하기 때문에 protected
	std::vector<Actor*> actors;

	// 추가 요청된 액터를 관리하는 배열
	std::vector<Actor*> addRequestedActors;

	// 삭제 요청된 액터를 관리하는 배열
	std::vector<Actor*> destroyRequestedActors;
};