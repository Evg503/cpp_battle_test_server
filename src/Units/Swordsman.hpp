#pragma once
#include "Item.hpp"

struct Swordsman : Item
{
	Swordsman(
		FieldNodifier* game, FieldInterface* field, UID_t uid, Coord_t x, Coord_t y, Health_t hp, Health_t strength) :
			Item(game, uid, x, y, hp),
			nearFight(field, this, strength, 1, 1)
	{
		notify_all(sw::io::UnitSpawned{uid, "Swordsman", pos.x, pos.y});
	}

	virtual bool doActions() override
	{
		if (isAlive())
		{
			return nearFight() || move_action();
		}
		return false;
	}

private:
	AttackAction nearFight;
};