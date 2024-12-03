#pragma once

#include "Action.hpp"
#include "Units/types.hpp"
#include "Util/GameNotifier.hpp"

#include <algorithm>

template <typename T>
int sign(T v)
{
	return (T(0) < v) - (v < T(0));
}

class MoveToTarget : public Action, public Sender<MarchNotifier>, public Sender<MovedNodifier>
{
public:
	using Sender<MovedNodifier>::subscribe;
	using Sender<MarchNotifier>::subscribe;

	using Sender<MovedNodifier>::notify_all;
	using Sender<MarchNotifier>::notify_all;

private:
	UID_t uid;
	Point target;
	Point& pos;

public:
	MoveToTarget(UID_t uid, Point& pos) :
			uid(uid),
			target(pos),
			pos(pos) {};

	void subscribe(GameNotifier* obj)
	{
		Sender<MovedNodifier>::subscribe(obj);
		Sender<MarchNotifier>::subscribe(obj);
	}

	bool set_target(Point new_target)
	{
		if (target != new_target)
		{
			target = new_target;
			notify_all(sw::io::MarchStarted{uid, pos.x, pos.y, target.x, target.y});
			return true;
		}
		return false;
	}

	bool operator()() override
	{
		Coord_t dx = sign(target.x - pos.x);
		Coord_t dy = sign(target.y - pos.y);
		if (dx != 0 || dy != 0)
		{
			auto oldpos = pos;
			pos.x += dx;
			pos.y += dy;
			notify_all(sw::io::UnitMoved{uid, oldpos, pos});
			if (pos == target)
			{
				notify_all(sw::io::MarchEnded{uid, pos.x, pos.y});
			}
			return true;
		}
		return false;
	}
};

class Attack : public Action
{
	Attack();
};

struct Item : public Sender<FieldNodifier>, public Sender<OterNotifier>, public Sender<MarchNotifier>
{
	using Sender<FieldNodifier>::subscribe;
	using Sender<OterNotifier>::subscribe;
	using Sender<MarchNotifier>::subscribe;

	using Sender<FieldNodifier>::notify_all;
	using Sender<OterNotifier>::notify_all;
	using Sender<MarchNotifier>::notify_all;
	UID_t uid;
	Point pos;
	Point target;
	Health_t hp;
	Health_t strength;
	MoveToTarget move_action;

	Item(FieldNodifier* game, UID_t uid, Coord_t x, Coord_t y, Health_t hp, Health_t strength) :
			uid(uid),
			pos{x, y},
			target{x, y},
			hp(hp),
			strength(strength),
			move_action(uid, pos)
	{
		subscribe(game);
        move_action.subscribe(game);
	}

	void subscribe(GameNotifier* obj)
	{
		Sender<FieldNodifier>::subscribe(obj);
		Sender<OterNotifier>::subscribe(obj);
		Sender<MarchNotifier>::subscribe(obj);
		move_action.subscribe(obj);
	}

	virtual ~Item() {}

	void march(Point new_target)
	{
		move_action.set_target(new_target);
	}

	void attaked(UID_t attacker_uid, Health_t damage)
	{
		hp = std::max(0, hp - damage);
		notify_all(sw::io::UnitAttacked{attacker_uid, uid, damage, hp});
		if (!isAlive())
		{
			notify_all(sw::io::UnitDied{uid, pos});
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
			return move_action();
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
