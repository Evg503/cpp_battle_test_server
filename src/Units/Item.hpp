#pragma once

#include "Units/types.hpp"
#include "notifier.hpp"

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

using GameNotifier = Notifier<sw::io::MarchStarted&&>;

template <typename Game>
struct Item : public Sender<GameNotifier>
{
	UID_t uid;
	Point pos;
	Point target;
	Health_t hp;
	Health_t strength;

	Item(GameNotifier* game, UID_t uid, Coord_t x, Coord_t y, Health_t hp, Health_t strength) :
			uid(uid),
			pos{x, y},
			target{x, y},
			hp(hp),
			strength(strength)
	{
		subscribe(game);
	}

	virtual ~Item() {}

	void march(Point new_target)
	{
		if (target != new_target)
		{
			target = new_target;
			notify(sw::io::MarchStarted{uid, pos.x, pos.y, target.x, target.y});
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
			game.notify(sw::io::UnitDied{uid, pos});
		}
	}

	void attack(Game& game, Item* victim, Health_t damage)
	{
		if (hp > 0)
		{
			victim->attaked(damage);
			game.notify(sw::io::UnitAttacked{uid, victim->uid, damage, victim->hp});
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
				auto oldpos = pos;
				pos.x += dx;
				pos.y += dy;
				game.notify(sw::io::UnitMoved{uid, oldpos, pos});
				if (pos == target)
				{
					game.notify(sw::io::MarchEnded{uid, pos.x, pos.y});
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

	virtual Coord_t getRange()
	{
		return 0;
	}

	virtual Coord_t getAgility()
	{
		return 0;
	}
};

template <typename Game>
struct Hunter : Item<Game>
{
	Health_t agility;
	Coord_t range;

	Hunter(GameNotifier *game,UID_t uid, Coord_t x, Coord_t y, Health_t hp, Health_t strength, Health_t agility, Health_t range) :
			Item<Game>(game,uid, x, y, hp, strength),
			agility(agility),
			range(range)
	{}

	Coord_t getRange() override
	{
		return range;
	}

	Coord_t getAgility() override
	{
		return agility;
	}
};

template <typename Game>
struct Swordsman : Item<Game>
{
	Swordsman(GameNotifier *game, UID_t uid, Coord_t x, Coord_t y, Health_t hp, Health_t strength) :
			Item<Game>(game,uid, x, y, hp, strength)
	{}
};