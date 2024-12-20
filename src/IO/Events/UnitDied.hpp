#pragma once

#include "Units/types.hpp"

namespace sw::io
{
	struct UnitDied
	{
		constexpr static const char* Name = "UNIT_DIED";

		UID_t unitId{};
		Point pos;

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("unitId", unitId);
		}
	};
}
