#include "ObjectManager.h"
#include "Actor/Actor.h"
#include "Utils/Utils.h"

void ObjectManager::SpawnActor(const Vector2& position)
{
	
}

void ObjectManager::DespawnActor(Actor* actor)
{
	SafeDelete(actor);
}
