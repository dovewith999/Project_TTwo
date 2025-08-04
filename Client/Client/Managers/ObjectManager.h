#pragma once
#include "Etc/Singleton.h"

class Actor;
class Vector2;
class ObjectManager final : public SafeSingleton<ObjectManager>
{
	friend class SafeSingleton<ObjectManager>;

public:
	explicit ObjectManager() = default;
	virtual ~ObjectManager() = default;

public:
	void SpawnActor(const Vector2& position);
	void DespawnActor(Actor* actor);
};

