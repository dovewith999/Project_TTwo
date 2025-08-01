#pragma once
#include "Level/Level.h"
#include "Actor/Actor.h"
#include <vector>

struct MenuItem
{
	// 함수포인터
	// typedef void (*OnSelected)();
	using OnSelected = void(*)();

public:
	MenuItem(const char* text, OnSelected onSelected)
		: onSelected(onSelected)
	{
		size_t length = strlen(text) + 1;
		menuText = new char[length];
		strcpy_s(menuText, length, text);
	}

	~MenuItem()
	{
		SafeDeleteArray(menuText);
	}

public:
	// 메뉴 텍스트
	char* menuText = nullptr;

	// 메뉴 선택 시 실행할 동작
	OnSelected onSelected = nullptr;

};

class MenuLevel : public Level
{
	RTTI_DECLARATIONS(MenuLevel, Level)

public:
	MenuLevel();
	virtual ~MenuLevel();

public:
	virtual void Tick(float deltaTime) override;
	virtual void Render() override;
	
private:
	// 현재 선택된 아이템의 인덱스,
	int currentIndex = 0;

	// 아이템 선택 색상
	Color selectedColor = Color::Green;
	
	// 아이템 선택 안됐을 때 색상
	Color unselectedColor = Color::White;
	
	// 아이템 배열
	std::vector<MenuItem*> items;

	// 메뉴 아이템 수
	int length = 0;
};