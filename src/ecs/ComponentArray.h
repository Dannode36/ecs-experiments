#pragma once
#include <assert.h>
#include <unordered_map>
#include <array>
#include "Core.h"
#include "Components.h"

namespace ecs {

#pragma once

	template <typename T>
	class SparseSet {
	private:

		std::vector<size_t> m_sparse;
		std::vector<T> m_dense;
		std::vector<Entity> m_denseToEntity; // 1:1 vector where dense index == Entity Index

		void SetDenseIndex(Entity id, size_t index) {
			if (id >= m_sparse.size()) {
				m_sparse.resize((size_t)id + 1, ecs::empty);
			}
			m_sparse[id] = index;
		}

		size_t GetDenseIndex(Entity id) {
			if (id < m_sparse.size()) {
				return m_sparse[id];
			}
			return ecs::empty;
		}

	public:

		SparseSet() {
			// Avoids initial copies/allocation, feel free to alter size
			m_dense.reserve(100);
		}

		T* set(Entity id, T obj) {
			// If index already exists, then simply overwrite
			// that element in dense list, no need to delete
			size_t index = GetDenseIndex(id);
			if (index != ecs::empty) {
				m_dense[index] = obj;
				m_denseToEntity[index] = id;

				return &m_dense[index];
			}

			// New index will be the back of the dense list
			SetDenseIndex(id, m_dense.size());

			m_dense.push_back(obj);
			m_denseToEntity.push_back(id);

			return &m_dense.back();
		}

		T* get(Entity id) {
			size_t index = GetDenseIndex(id);
			return (index != ecs::empty) ? &m_dense[index] : nullptr;
		}

		bool tryGet(Entity id, T* out) {
			size_t index = GetDenseIndex(id);
			out = (index != ecs::empty) ? &m_dense[index] : nullptr;
			return out != nullptr;
		}

		void erase(Entity id) {

			size_t deletedIndex = GetDenseIndex(id);

			SetDenseIndex(m_denseToEntity.back(), deletedIndex);
			SetDenseIndex(id, ecs::empty);

			std::swap(m_dense.back(), m_dense[deletedIndex]);
			std::swap(m_denseToEntity.back(), m_denseToEntity[deletedIndex]);

			m_dense.pop_back();
			m_denseToEntity.pop_back();
		}

		void clear() {
			m_dense.clear();
			m_sparse.clear();
			m_denseToEntity.clear();
		}

		bool isEmpty() const {
			return m_dense.empty();
		}

		// Read-only dense list
		const std::vector<T>& data() const {
			return m_dense;
		}
	};

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
