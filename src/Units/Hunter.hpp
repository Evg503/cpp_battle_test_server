#pragma once

#include "Item.hpp"

struct Hunter : Item
{
	Hunter(
		FieldNodifier* game,
		FieldInterface* field,
		UID_t uid,
		Coord_t x,
		Coord_t y,
		Health_t hp,
		Health_t strength,
		Health_t agility,
		Health_t range) :
			Item(game, uid, x, y, hp),
			nearFight(field, this, strength, 1, 1),
			farFight(field, this, agility, 2, range)
	{
		notify_all(sw::io::UnitSpawned{uid, "Hunter", pos.x, pos.y});
	}

	virtual bool doActions() override
	{
		if (isAlive())
		{
			return nearFight() || farFight() || move_action();
		}
		return false;
	}

private:
	AttackAction nearFight;
	AttackAction farFight;
};
