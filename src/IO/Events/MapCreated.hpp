#pragma once

#include "Units/types.hpp"

namespace sw::io
{
	struct MapCreated
	{
		constexpr static const char* Name = "MAP_CREATED";

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
