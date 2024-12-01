#pragma once

#include <cstdint>
#include <iosfwd>
#include "Units/types.hpp"

namespace sw::io
{
	struct CreateMap
	{
		constexpr static const char* Name = "CREATE_MAP";

		Coord_t width{};
		Coord_t height{};

		template <typename Visitor>
		void visit(Visitor& visitor)
		{
			visitor.visit("width", width);
			visitor.visit("height", height);
		}
	};
}
