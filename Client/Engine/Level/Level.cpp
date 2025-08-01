#include "Level.h"
#include "Actor/Actor.h"

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
	//오너쉽 설정
	newActor->SetOwner(this);

	//배열 맨 뒤에 새로운 항목을 추가하는 함수
	// emplace는 이동 - move 시멘틱
	// push_back은 참조 
	// 그래서 emplace_back이 성능이 더 좋음. 그러나 현재는 push_back도 발전해서 차이가 없음
	actors.emplace_back(newActor);
	//actors.push_back(newActor);
		
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
	// 그리기 전에 정렬 순서 기준으로 재배치(정렬)
	SortActorsBySortingOrder();
	
	// Render Pass
	for (Actor* const actor : actors)
	{
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
