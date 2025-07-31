#pragma once
#include "Math/Vector2.h"
#include "Math/Color.h"
#include "RTTI.h"

// 물체가 뭘 해야할까? 를 정의
// 위치 점령. 
// 콘솔 창에 그리기(How? Whar?)
// 엔진의 이벤트 함수 호출
// BeginPlay/Tick/Draw

class Level;
class Engine_API Actor : public RTTI
{
	RTTI_DECLARATIONS(Actor, RTTI)

public:
	Actor(const char* image = "", Color color = Color::White, const Vector2& position = Vector2::Zero);
	virtual ~Actor();
	
public:
	// 이벤트 함수
	//객체 생애주기(LifeTime)에 1번만 호출됨(초기화 목적)
	virtual void BeginPlay();
	// 프레임마다 호출(반복성, 지속성이 필요한 작업)
	virtual void Tick(float deltaTime);
	//그리기 함수
	virtual void Render();

	FORCEINLINE const bool HasBeganPlay() const { return hasBeganPlay; }

	// 충돌 확인 요청 함수 (간단한 AABB 로직)
	bool TestIntersect(const Actor* const other);

	// 객체 삭제 함수
	void Destroy();

	// 게임 종료 요청 함수
	void QuitGame() const;

public:/*Getter & Setter*/
	FORCEINLINE void SetPosition(const Vector2& newPosition);
	FORCEINLINE Vector2 GetPosition() const;

	FORCEINLINE void SetSortingOrder(unsigned int sortingOrder);
	//friend class Level; 로도 해결 가능함. 어떤 클래스에게 예외를 주고 싶은 상황에 이용 가능
	FORCEINLINE unsigned int GetSortringOrder() const;

	FORCEINLINE void SetOwner(Level* newOwner);
	FORCEINLINE Level* GetOwner() const;

	FORCEINLINE int GetWidth() const;

	FORCEINLINE bool GetExpired() const;
	FORCEINLINE bool GetIsActive() const;

private:
	// 개체의 위치
	Vector2 position;

	// 그려질 이미지 값
	char* image = nullptr;

	// 문자열 길이
	int width = 0;

	// 텍스트 색상 값
	Color color = Color::White;

	// BeginPlay 호출이 되었는지 확인.
	bool hasBeganPlay = false;

	// 정렬 순서
	unsigned int sortingOrder = 0;

	// 액터가 활성 상태인지 알려주는 변수
	bool isActive = true;

	// 삭제 요청 됐는지 알려주는 변수
	bool isExpired = false;

	//소유 레벨.(OwnerShip)
	Level* owner = nullptr;
};