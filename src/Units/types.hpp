#pragma once

#include <algorithm>
#include <cstdint>
#include <memory>

using Coord_t = std::int32_t;
using Health_t = std::int32_t;
using Time_t = std::uint64_t;
using UID_t = std::uint32_t;

struct Point
{
	Coord_t x;
	Coord_t y;
};

inline bool operator==(const Point& lhs, const Point& rhs)
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline  Coord_t distance(const Point& lhs, const Point& rhs)
{
	return std::max(std::abs(rhs.x - lhs.x), std::abs(rhs.y - lhs.y));
}

inline bool less_eq3(Coord_t min, Coord_t v, Coord_t max)
{
	return min <= v && v <= max;
}

template <typename T>
int sign(T v)
{
	return (T(0) < v) - (v < T(0));
}
struct Item;
using PItem = std::shared_ptr<Item>;
