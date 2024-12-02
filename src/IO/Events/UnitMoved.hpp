#pragma once

#include "Units/types.hpp"

namespace sw::io
{
	struct UnitMoved
	{
		constexpr static const char* Name = "UNIT_MOVED";

		UID_t unitId{};
		Point from{};
		Point to{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
			visitor.visit("x", to.x);
			visitor.visit("y", to.y);
		}
	};
}
