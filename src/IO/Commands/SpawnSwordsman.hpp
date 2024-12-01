#pragma once

#include <iosfwd>
#include "Units/types.hpp"

namespace sw::io
{
	struct SpawnSwordsman
	{
		constexpr static const char* Name = "SPAWN_SWORDSMAN";

		UID_t unitId{};
		Coord_t x{};
		Coord_t y{};
		Health_t hp{};
		Health_t strength{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
			visitor.visit("x", x);
			visitor.visit("y", y);
			visitor.visit("hp", hp);
			visitor.visit("strength", strength);
		}
	};
}
