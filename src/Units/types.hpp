#pragma once

#include <algorithm>
#include <cstdint>

using Coord_t = std::int32_t;
using Health_t = std::int32_t;
using Time_t = std::uint64_t;
using UID_t = std::uint32_t;

struct Point
{
	Coord_t x;
	Coord_t y;
};

Coord_t distance(const Point& lhs, const Point& rhs)
{
	return std::max(std::abs(rhs.x - lhs.x), std::abs(rhs.y - lhs.y));
}

bool less_eq3(Coord_t min, Coord_t v, Coord_t max)
{
	return min <= v && v <= max;
}