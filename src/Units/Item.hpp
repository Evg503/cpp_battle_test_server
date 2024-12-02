#pragma once

#include "Units/types.hpp"

#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <algorithm>

template <typename T>
int sign(T v)
{
	return (T(0) < v) - (v < T(0));
}

template <typename Game>
struct Item
{
	UID_t uid;

	Point pos;

	Point target;

	Health_t hp;

	Health_t strength;

	Health_t agility;
	Coord_t range;

	Item(UID_t uid, Coord_t x, Coord_t y, Health_t hp, Health_t strength, Health_t agility = 0, Health_t range = 0) :
			uid(uid),
			pos{x, y},
			target{x, y},
			hp(hp),
			strength(strength),
			agility(agility),
			range(range)
	{}

	void march(Game& game, Point new_target)
	{
		if (target != new_target)
		{
			target = new_target;
			game.log(sw::io::MarchStarted{uid, pos.x, pos.y, target.x, target.y});
		}
	}

	void attaked(Health_t damage)
	{
		hp = std::max(0, hp - damage);
	}

	void checkHealth(Game& game)
	{
		if (hp == 0)
		{
			game.log(sw::io::UnitDied{uid});
			game.getFieldPoint(pos) = nullptr;
		}
	}

	void attack(Game& game, Item* victim, Health_t damage)
	{
		if (hp > 0)
		{
			victim->attaked(damage);
			game.log(sw::io::UnitAttacked{uid, victim->uid, damage, victim->hp});
			victim->checkHealth(game);
		}
	}

	bool move(Game& game)
	{
		if (hp > 0)
		{
			Coord_t dx = sign(target.x - pos.x);
			Coord_t dy = sign(target.y - pos.y);
			if (dx != 0 || dy != 0)
			{
				pos.x += dx;
				pos.y += dy;
				game.log(sw::io::UnitMoved{uid, pos.x, pos.y});
				if (pos == target)
				{
					game.log(sw::io::MarchEnded{uid, pos.x, pos.y});
				}
				return true;
			}
		}
		return false;
	}

	bool isAttacable()
	{
		return hp > 0;
	}

	bool isAlive()
	{
		return hp > 0;
	}
};
