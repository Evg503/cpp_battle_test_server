#pragma once

#include "Units/types.hpp"

namespace sw::io
{
	struct SpawnHunter
	{
		constexpr static const char* Name = "SPAWN_HUNTER";

		UID_t unitId{};
		Coord_t x{};
		Coord_t y{};
		Health_t hp{};
		Health_t agility{};
		Health_t strength{};
		Coord_t range{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
			visitor.visit("x", x);
			visitor.visit("y", y);
			visitor.visit("hp", hp);
			visitor.visit("agility", agility);
			visitor.visit("strength", strength);
			visitor.visit("range", range);
		}
	};
}
