#include "Level.h"
#include "Actor/Actor.h"
#include "Utils/Utils.h"
#include <iostream>

Level::Level()
{
}

Level::~Level()
{
	for (Actor* actor : actors)
	{
		SafeDelete(actor);
	}

	actors.clear();
}

void Level::AddActor(Actor* newActor)
{
	if (newActor == nullptr)
	{
		return;
	}

	addRequestedActors.emplace_back(newActor);
}

void Level::DestroyActor(Actor* destroyedActor)
{
	if (destroyedActor == nullptr)
	{
		return;
	}

	destroyRequestedActors.emplace_back(destroyedActor);
}

void Level::BeginPlay()
{
	
	for (Actor* const actor : actors)
	{
		if (actor->GetExpired() || !actor->GetIsActive())
		{
			continue;
		}

		if (actor->HasBeganPlay())
		{
			continue;
		}

		actor->BeginPlay();
	}
}


void Level::Tick(float deltaTime)
{
	for (Actor* const actor : actors)
	{
		if (actor->GetExpired() || !actor->GetIsActive())
		{
			continue;
		}

		actor->Tick(deltaTime);
	}
}

void Level::Render()
{
	// 그리기 전에 정렬 순서 기준으로 재배치(정렬)
	SortActorsBySortingOrder();
	
	// Render Pass
	for (Actor* const actor : actors)
	{
		if (actor->GetExpired() || !actor->GetIsActive())
		{
			continue;
		}

		Actor* searchedActor = nullptr;

		// 검사 (같은 위치에 정렬 순서 높은 액터가 있는 지 판단.)
		for (Actor* const otherActor : actors)
		{
			// 같은 액터면 무시.
			if (actor == otherActor)
			{
				continue;
			}

			if (actor->GetPosition() == otherActor->GetPosition())
			{
				if (actor->GetSortringOrder() < otherActor->GetSortringOrder())
				{
					searchedActor = otherActor;
					break;
				}
			}
		}
		// 드로우 콜

		if (searchedActor != nullptr)
		{
			continue;
		}
		actor->Render();
	}
}

void Level::ProcessAddAndDestroyActors()
{
	for (auto iter = actors.begin(); iter != actors.end();)
	{
		if ((*iter)->GetExpired())
		{
			iter = actors.erase(iter);
			continue;
		}

		++iter;
	}

	for (auto* actor : destroyRequestedActors)
	{
		// 액터가 그려졌던 곳 지우기
		Utils::SetConsoleCursorPosition(actor->GetPosition());

		//콘솔에 빈 문자 출력해서 지우기
		for (int i = 0; i < actor->GetWidth(); ++i)
		{
			std::cout <<  " ";
		}

		SafeDelete(actor);
	}

	destroyRequestedActors.clear();

	for (auto& actor : addRequestedActors)
	{
		//오너쉽 설정
		actor->SetOwner(this);
		actors.emplace_back(actor);
	}

	addRequestedActors.clear();
}

void Level::SortActorsBySortingOrder()
{
	// 버블 정렬
	for (int i = 0; i < static_cast<int>(actors.size()); ++i)
	{
		for (int j = 0; j < static_cast<int>(actors.size()) - 1; ++j)
		{
			if (actors[j]->GetSortringOrder() > actors[j + 1]->GetSortringOrder())
			{
				std::swap(actors[j], actors[j + 1]);
			}
		}
	}
}
