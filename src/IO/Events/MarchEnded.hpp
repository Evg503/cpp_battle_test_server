#pragma once

#include "Units/types.hpp"

namespace sw::io
{
	struct MarchEnded
	{
		constexpr static const char* Name = "MARCH_ENDED";

		UID_t unitId{};
		Coord_t x{};
		Coord_t y{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
			visitor.visit("x", x);
			visitor.visit("y", y);
		}
	};
}
