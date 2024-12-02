#pragma once

#include "Units/types.hpp"

#include <string>

namespace sw::io
{
	struct UnitSpawned
	{
		constexpr static const char* Name = "UNIT_SPAWNED";

		UID_t unitId{};
		std::string unitType{};
		Coord_t x{};
		Coord_t y{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
			visitor.visit("unitType", unitType);
			visitor.visit("x", x);
			visitor.visit("y", y);
		}
	};
}
