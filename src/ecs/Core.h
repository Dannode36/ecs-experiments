#pragma once
#include <set>
#include <bitset>
#include <cstdint>

namespace ecs {
	constexpr int MAX_ENTITIES = 1024;
	constexpr int MAX_COMPONENTS = 1024;

	typedef uint32_t Entity;
	typedef std::bitset<MAX_COMPONENTS> Signature;
	static constexpr size_t empty = std::numeric_limits<size_t>::max();
}