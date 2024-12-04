#include <Actions/AttackAction.hpp>

bool AttackAction::operator()()
{
	auto victim = _field->getRandomVictim(_item, _minDistance, _maxDistance);
	if (victim)
	{
		if (_item->isAlive())
		{
			victim->attaked(_item->uid, _damage);
		}
		return true;
	}
	return false;
}
