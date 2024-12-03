#pragma once
#include "Item.hpp"

struct Swordsman : Item
{
	Swordsman(FieldNodifier* game, UID_t uid, Coord_t x, Coord_t y, Health_t hp, Health_t strength) :
			Item(game, uid, x, y, hp, strength)
	{
		notify_all(sw::io::UnitSpawned{uid, "Swordsman", pos.x, pos.y});
    }
};