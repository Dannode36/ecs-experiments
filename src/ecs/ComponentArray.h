#pragma once
#include <assert.h>
#include <unordered_map>
#include <array>
#include "Core.h"
#include "Components.h"

namespace ecs {

#pragma once
	template<typename T>
	class ComponentArray
	{
	public:
		void Insert(size_t entity, T component)
		{
			// Put new entry at end and update the maps
			size_t newIndex = mSize;
			mEntityToIndexMap[entity] = newIndex;
			mIndexToEntityMap[newIndex] = entity;
			mArray[newIndex] = component;
			++mSize;
		}

		void Remove(size_t entity)
		{
			// Copy element at end into deleted element's place to maintain density
			size_t indexOfRemovedEntity = mEntityToIndexMap[entity];
			size_t indexOfLastElement = mSize - 1;
			mArray[indexOfRemovedEntity] = mArray[indexOfLastElement];

			// Update map to point to moved spot
			Entity entityOfLastElement = mIndexToEntityMap[indexOfLastElement];
			mEntityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
			mIndexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

			mEntityToIndexMap.erase(entity);
			mIndexToEntityMap.erase(indexOfLastElement);

			--mSize;
		}

		T& Get(size_t entity)
		{
			// Return a reference to the entity's component
			return mArray[mEntityToIndexMap[entity]];
		}

		void Destroyed(size_t entity)
		{
			if (mEntityToIndexMap.find(entity) != mEntityToIndexMap.end())
			{
				// Remove the entity's component if it existed
				Remove(entity);
			}
		}

	private:
		// The packed array of components (of generic type T),
		// set to a specified maximum amount, matching the maximum number
		// of entities allowed to exist simultaneously, so that each entity
		// has a unique spot.
		std::array<T, ecs::MAX_ENTITIES> mArray;

		// Total size of valid entries in the array.
		size_t mSize{};

		// TileMap from an entity ID to an array index.
		std::unordered_map<size_t, size_t> mEntityToIndexMap;

		// TileMap from an array index to an entity ID.
		std::unordered_map<size_t, size_t> mIndexToEntityMap;
	};
}
