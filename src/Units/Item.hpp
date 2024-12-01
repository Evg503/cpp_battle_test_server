#pragma once

#include "Units/types.hpp"

#include <IO/Events/UnitMoved.hpp>

template <typename T>
int sign(T v)
{
	return (T(0) < v) - (v < T(0));
}

template <typename Logger>
struct Item
{
	UID_t uid;
	Point pos;
	Point target;

	Item(UID_t uid, Coord_t x, Coord_t y) :
			uid(uid),
			pos{x, y},
			target{x, y}
	{}

	void move(Time_t time, Logger& logger)
	{
		Coord_t dx = sign(target.x - pos.x);
		Coord_t dy = sign(target.y - pos.y);
		if (dx != 0 || dy != 0)
		{
			pos.x += dx;
			pos.y += dy;
			logger.log(time, sw::io::UnitMoved{uid, pos.x, pos.y});
		}
	}
};
