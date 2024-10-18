#pragma once
#include "defines.h"

#include <functional>

template <typename T, typename... Rest>
void hashCombine(u64 seed, const T& v, const Rest&... rest) {
	seed ^= std::hash<T>{}(v) + 0x8e3779b9 + (seed << 6) + (seed >> 2);
	(hashCombine(seed, rest), ...);
}
