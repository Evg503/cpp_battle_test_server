#pragma once

#include <iosfwd>

#include "Units/types.hpp"

namespace sw::io
{
	struct March
	{
		constexpr static const char* Name = "MARCH";

		UID_t unitId{};
		Coord_t targetX{};
		Coord_t targetY{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
			visitor.visit("targetX", targetX);
			visitor.visit("targetY", targetY);
		}
	};
}
