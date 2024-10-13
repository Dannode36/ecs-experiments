#pragma once
#include <vector>
#include "Core.h"
#include "Components.h"

namespace ecs {
	class ISparseSet {
		virtual void erase(size_t id) = 0;
		virtual void clear() = 0;
	};

	template <typename T>
	class SparseSet : public ISparseSet {
	private:
		std::vector<size_t> m_sparse;
		std::vector<T> m_dense;

		void setDenseIndex(size_t id, size_t index) {
			if (id >= m_sparse.size()) {
				m_sparse.resize(id + 1, ecs::empty);
			}
			m_sparse[id] = index;
		}

		size_t getDenseIndex(size_t id) {
			if (id < m_sparse.size()) {
				return m_sparse[id];
			}
			return ecs::empty;
		}

	public:
		using iterator = typename std::vector<T>::iterator;
		using const_iterator = typename std::vector<T>::const_iterator;

		iterator begin() { return m_dense.begin(); }
		const_iterator begin() const { return m_dense.begin(); }

		iterator end() { return m_dense.begin() + m_dense.size(); }
		const_iterator end() const { return m_dense.begin() + m_dense.size(); }

		SparseSet() {
			// Avoids initial copies/allocation, feel free to alter size
			m_dense.reserve(100);
		}

		T* set(size_t id, T obj) {
			// If index already exists, then simply overwrite
			// that element in dense list, no need to delete
			size_t index = getDenseIndex(id);
			if (index != ecs::empty) {
				m_dense[index] = obj;
				m_sparse[id] = index;

				return &m_dense[index];
			}

			// New index will be the back of the dense list
			setDenseIndex(id, m_dense.size());

			m_dense.push_back(obj);
			m_sparse.push_back(index);

			return &m_dense.back();
		}

		T* get(size_t id) {
			size_t index = getDenseIndex(id);
			return (index != ecs::empty) ? &m_dense[index] : nullptr;
		}

		bool tryGet(size_t id, T* out) {
			size_t index = getDenseIndex(id);
			out = (index != ecs::empty) ? &m_dense[index] : nullptr;
			return out != nullptr;
		}

		void erase(size_t id) override {

			size_t deletedIndex = getDenseIndex(id);

			setDenseIndex(m_sparse.back(), deletedIndex);
			setDenseIndex(id, ecs::empty);

			std::swap(m_dense.back(), m_dense[deletedIndex]);
			std::swap(m_sparse.back(), m_sparse[deletedIndex]);

			m_dense.pop_back();
			m_sparse.pop_back();
		}

		void clear() override {
			m_dense.clear();
			m_sparse.clear();
		}

		bool isEmpty() const {
			return m_dense.empty();
		}

		// Read-only dense list
		const std::vector<T>& data() const {
			return m_dense;
		}
	};
}
