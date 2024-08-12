#pragma once
#include "Core.h"

constexpr int MAX_ENTITIES = 1024;
constexpr int MAX_COMPONENTS = 1024;

typedef uint32_t Entity;
typedef std::bitset<MAX_COMPONENTS> Signature;

template <typename T>
class System {
	std::set<Entity> mEntities;
};
