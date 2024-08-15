#pragma once
#include <typeinfo>
#include <unordered_map>
#include "Core.h"
#include "Components.h"
#include "SparseSet.h"

namespace ecs {
	size_t entityCounter = 0;
	std::unordered_map<std::string, std::unique_ptr<ISparseSet>> componentSets;

	Entity newEntity() {
		return entityCounter++;
	}

	template<typename T>
	void RegisterComponent() {
		auto typeName = typeid(T).name();
	}
}
