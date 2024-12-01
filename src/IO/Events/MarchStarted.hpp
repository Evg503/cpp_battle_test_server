#pragma once

#include "Units/types.hpp"

namespace sw::io
{
	struct MarchStarted
	{
		constexpr static const char* Name = "MARCH_STARTED";

		UID_t unitId{};
		Coord_t x{};
		Coord_t y{};
		Coord_t targetX{};
		Coord_t targetY{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
			visitor.visit("x", x);
			visitor.visit("y", y);
			visitor.visit("targetX", targetX);
			visitor.visit("targetY", targetY);
		}
	};
}
