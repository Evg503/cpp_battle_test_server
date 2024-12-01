#pragma once

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
