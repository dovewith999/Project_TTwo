#include "Level.h"
#include "Actor/Actor.h"
#include "Utils/Utils.h"

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
	// 예외처리(중복 여부 확인) 필요함.

	// 대기 배열에 추가.
	addRequstedActors.emplace_back(newActor);

	// 오너십 설정.
	//newActor->SetOwner(this);
}

void Level::DestroyActor(Actor* destroyedActor)
{
	// 중복 검사.
	if (destroyRequstedActors.size() > 0)
	{
		for (const Actor* const actor : destroyRequstedActors)
		{
			if (actor == destroyedActor)
			{
				return;
			}
		}
	}

	// 대기 배열에 추가.
	destroyRequstedActors.emplace_back(destroyedActor);
}

void Level::BeginPlay()
{
	for (Actor* const actor : actors)
	{
		if (actor->HasBeganPlay())
		{
			continue;
		}

		actor->BeginPlay();
	}

	for (Actor* const actor : actors)
	{
		// 액터 처리 여부 확인.
		if (!actor->GetActive() || actor->GetExpired())
		{
			continue;
		}

		// 이미 호출된 개체는 건너뛰기.
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
		actor->Tick(deltaTime);
	}
}

void Level::Render()
{
	// Render Pass.
	for (Actor* const actor : actors)
	{
		// 드로우 콜.
		actor->Render();
	}
}

void Level::ProcessAddAndDestroyActors()
{
	// actors 배열에서 제외 처리.
	for (auto iterator = actors.begin(); iterator != actors.end();)
	{
		// 삭제 요청된 액터인지 확인 후 배열에서 제외
		if ((*iterator)->GetExpired())
		{
			// erase 함수를 사용하면 iterator가 무효화되기 때문에
			// 반환되는 값을 저장해야함.
			iterator = actors.erase(iterator);
			continue;
		}

		++iterator;
	}

	// destroyRequstedActors 배열을 순회하면서 액터 delete.
	for (auto*& actor : destroyRequstedActors)
	{
		// 액터가 그렸던 곳 지우기.
		Utils::SetConsoleCursorPosition(actor->GetPosition());

		// 콘솔에 빈문자 출력해서 지우기.
		//for (int ix = 0; ix < actor->width; ++ix)
		//{
		//	std::cout << " ";
		//}

		// 리소스 해제.
		SafeDelete(actor);
	}

	// 배열 초기화 -> 크기가 0.
	destroyRequstedActors.clear();

	// addRequstedActors 배열을 순회하면서 새로운 액터 추가.
	for (auto* const actor : addRequstedActors)
	{
		actors.emplace_back(actor);
		actor->SetOwner(this);
	}

	// 배열 초기화.
	addRequstedActors.clear();
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
