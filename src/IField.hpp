#pragma once

#include <Units/types.hpp>
#include <random>

class FieldInterface
{
private:
	virtual std::vector<PItem> getNeighbors(const Item* main_item, Coord_t min_d, Coord_t max_d) = 0;

public:
	PItem getRandomVictim(const Item* main_item, Coord_t min_d, Coord_t max_d)
	{
		auto neighbors = getNeighbors(main_item, min_d, max_d);
		if (!neighbors.empty())
		{
			std::uniform_int_distribution<> distrib(0, neighbors.size() - 1);
			auto victim = neighbors[distrib(gen)];
			return victim;
		}
		return PItem();
	}

	virtual ~FieldInterface() = default;

private:
	std::mt19937 gen;
};
