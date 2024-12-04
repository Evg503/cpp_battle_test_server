#pragma once
#include "Action.hpp"

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
