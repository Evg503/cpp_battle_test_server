#pragma once

#include "Units/types.hpp"
#include "Util/GameNotifier.hpp"

#include <Actions/AttackAction.hpp>
#include <Actions/MoveToTargetAction.hpp>
#include <algorithm>

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
	Health_t hp;
	MoveToTarget move_action;

	Item(FieldNodifier* game, UID_t uid, Coord_t x, Coord_t y, Health_t hp) :
			uid(uid),
			pos{x, y},
			hp(hp),
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

	virtual bool doActions()
	{
		if (isAlive())
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
};
