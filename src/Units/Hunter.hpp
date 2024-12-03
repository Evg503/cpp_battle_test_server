#pragma once

#include "Item.hpp"

struct Hunter : Item
{
	Health_t agility;
	Coord_t range;

	Hunter(
		FieldNodifier* game,
		UID_t uid,
		Coord_t x,
		Coord_t y,
		Health_t hp,
		Health_t strength,
		Health_t agility,
		Health_t range) :
			Item(game, uid, x, y, hp, strength),
			agility(agility),
			range(range)
	{
		notify_all(sw::io::UnitSpawned{uid, "Hunter", pos.x, pos.y});
	}

	Coord_t getRange() override
	{
		return range;
	}

	Coord_t getAgility() override
	{
		return agility;
	}
};
