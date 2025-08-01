#pragma once
#include "Level/Level.h"
#include "Actor/Actor.h"

struct TitleItem
{
	// 함수포인터
	// typedef void (*OnSelected)();
	using OnSelected = void(*)();

public:
	TitleItem(const char* text, OnSelected onSelected)
		: onSelected(onSelected)
	{
		size_t length = strlen(text) + 1;
		itemText = new char[length];
		strcpy_s(itemText, length, text);
	}

	~TitleItem()
	{
		SafeDeleteArray(itemText);
	}

public:
	// 메뉴 텍스트
	char* itemText = nullptr;

	// 메뉴 선택 시 실행할 동작
	OnSelected onSelected = nullptr;

};
class TitleLevel final : public Level
{
	RTTI_DECLARATIONS(TitleLevel, Level)

public:
	explicit TitleLevel();
	virtual ~TitleLevel();

public:
	//virtual void BeginPlay() override;
	virtual void Tick(float deltaTime) override;
	virtual void Render() override;

private:
	void ReadTitleFile();

private:
	// 현재 선택된 아이템의 인덱스,
	int currentIndex = 0;

	// 아이템 선택 색상
	Color selectedColor = Color::Green;

	// 아이템 선택 안됐을 때 색상
	Color unselectedColor = Color::White;

	// 아이템 배열
	std::vector<TitleItem*> items;

	// 메뉴 아이템 수
	int length = 0;


};