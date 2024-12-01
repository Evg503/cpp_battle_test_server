#pragma once
#include "Units/Item.hpp"

#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/EventLog.hpp>
#include <IO/System/PrintDebug.hpp>
#include <random>
#include <unordered_map>
#include <vector>

template <typename Logger>
class Game
{
	using Item = Item<Game>;

public:
	Game(Logger& log) :
			_log(log)
	{}

	template <class TEvent>
	void log(TEvent&& event)
	{
		_log.log(time, std::forward<TEvent>(event));
	}

	void createField(Coord_t W, Coord_t H)
	{
		log(sw::io::MapCreated{W, H});
	}

	void spawnSwordsman(UID_t uid, Coord_t x, Coord_t y, Health_t hp, Health_t strength)
	{
		_items_index[uid] = _items.size();
		auto& item = _items.emplace_back(uid, x, y, hp, strength);
		log(sw::io::UnitSpawned{item.uid, "Swordsman", item.pos.x, item.pos.y});
	}

	void spawnHunter(UID_t uid, Coord_t x, Coord_t y, Health_t hp, Health_t agility, Health_t strength, Coord_t range)
	{
		_items_index[uid] = _items.size();
		auto& item = _items.emplace_back(uid, x, y, hp, strength, agility, range);
		log(sw::io::UnitSpawned{item.uid, "Hunter", item.pos.x, item.pos.y});
	}

	void march(UID_t uid, Coord_t target_x, Coord_t target_y)
	{
		auto idx = _items_index[uid];
		auto& item = _items[idx];
		item.march(*this, Point{target_x, target_y});
	}

	Coord_t distance(const Item& lhs, const Item& rhs)
	{
		return ::distance(lhs.pos, rhs.pos);
	}

	Item& getItem(UID_t uid)
	{
		return _items[_items_index[uid]];
	}

	std::vector<Item*> getNeighbors(const Item& main_item, Coord_t min_d, Coord_t max_d)
	{
		std::vector<Item*> result;
		for (auto& item : _items)
		{
			if (&item != &main_item && less_eq3(min_d, distance(main_item, item), max_d))
			{
				result.push_back(&item);
			}
		}
		return result;
	}

	bool loAttack(Item& item)
	{
		auto neighbors = getNeighbors(item, 1, 1);
		if (neighbors.empty())
		{
			return false;
		}
		std::uniform_int_distribution<> distrib(0, neighbors.size() - 1);
		auto victim = neighbors[distrib(gen)];
		item.attack(*this, victim, item.strength);
		return true;
	}

	bool hiAttack(Item& item)
	{
		auto neighbors = getNeighbors(item, 2, item.range);
		if (neighbors.empty())
		{
			return false;
		}
		std::uniform_int_distribution<> distrib(0, neighbors.size() - 1);
		auto victim = neighbors[distrib(gen)];
		item.attack(*this, victim, item.agility);
		return true;
	}

	bool attack(Item& item)
	{
		return loAttack(item) || hiAttack(item);
	}

	void update()
	{
		time += 1;
		for (auto& item : _items)
		{
			if (!attack(item))
			{
				item.move(*this);
			}
		}
	}

private:
	std::mt19937 gen;
	Time_t time = 1;
	Logger& _log;
	std::vector<Item> _items;
	std::unordered_map<UID_t, size_t> _items_index;
};
