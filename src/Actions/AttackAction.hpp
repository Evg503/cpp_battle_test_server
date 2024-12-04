#pragma once
#include <Actions/Action.hpp>
#include <IField.hpp>
#include <Units/Item.hpp>

class AttackAction : public Action
{
public:
	AttackAction(FieldInterface* field, Item* item, Health_t damage, Coord_t minDistance, Coord_t maxDistance) :
			_field(field),
			_item(item),
			_damage(damage),
			_minDistance(minDistance),
			_maxDistance(maxDistance)
	{}

	bool operator()() override;

private:
	Item* _item;
	FieldInterface* _field;
	Health_t _damage;
	Coord_t _minDistance;
	Coord_t _maxDistance;
};
