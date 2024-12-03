#pragma once

#include "Units/types.hpp"
#include "Util/GameNotifier.hpp"

#include <algorithm>

template <typename T>
int sign(T v)
{
	return (T(0) < v) - (v < T(0));
}


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

	void attaked(UID_t attacker_uid, Health_t damage)
	{
		hp = std::max(0, hp - damage);
		notify(sw::io::UnitAttacked{attacker_uid, uid, damage, hp});
		if (!isAlive())
		{
			notify(sw::io::UnitDied{uid, pos});
		}
	}

	void attack(Item* victim, Health_t damage)
	{
		if (isAlive())
		{
			victim->attaked(uid, damage);
		}
	}

	bool move()
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
				notify(sw::io::UnitMoved{uid, oldpos, pos});
				if (pos == target)
				{
					notify(sw::io::MarchEnded{uid, pos.x, pos.y});
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
