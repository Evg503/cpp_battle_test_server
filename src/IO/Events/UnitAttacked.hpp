#pragma once

#include "Units/types.hpp"

namespace sw::io
{
	struct UnitAttacked
	{
		constexpr static const char* Name = "UNIT_ATTACKED";

		UID_t attackerUnitId{};
		UID_t targetUnitId{};
		Health_t damage{};
		Health_t targetHp{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("attackerUnitId", attackerUnitId);
			visitor.visit("targetUnitId", targetUnitId);
			visitor.visit("damage", damage);
			visitor.visit("targetHp", targetHp);
		}
	};
}
