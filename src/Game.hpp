#pragma once
#include "Units/types.hpp"

#include <vector>
#include <unordered_map>

#include <IO/Commands/CreateMap.hpp>
#include <IO/Commands/March.hpp>
#include <IO/Commands/SpawnHunter.hpp>
#include <IO/Commands/SpawnSwordsman.hpp>
#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/EventLog.hpp>
#include <IO/System/PrintDebug.hpp>


template <typename T>
int sign(T v)
{
	return (T(0) < v) - (v < T(0));
}

template <typename Logger>
class Game
{
	inline static std::unordered_map<std::string, std::string> klasses = {
		{"SWORDMAN", "Swordsman"},
		{"HUNTER", "Hunter"},
	};

public:
	Game(Logger& log) :
			_log(log)
	{}

	void createField(Coord_t W, Coord_t H)
	{
		_log.log(time, sw::io::MapCreated{W, H});
	}

	void spawn(const char* klass, UID_t uid, Coord_t x, Coord_t y, ...)
	{
		_items_index[uid] = _items.size();
		auto& item = _items.emplace_back(uid, x, y, x, y);
		_log.log(time, sw::io::UnitSpawned{item.uid, klasses[klass], item.x, item.y});
	}

	void march(UID_t uid, Coord_t target_x, Coord_t target_y)
	{
		auto idx = _items_index[uid];
		auto& item = _items[idx];
		item.target_x = target_x;
		item.target_y = target_y;
		_log.log(time, sw::io::MarchStarted{item.uid, item.x, item.y, item.target_x, item.target_y});
	}

	void update()
	{
		time += 1;
		for (auto& item : _items)
		{
			move(item);
		}
	}

private:
	struct Item
	{
		UID_t uid;
		Coord_t x;
		Coord_t y;
		Coord_t target_x;
		Coord_t target_y;

		Item(UID_t uid, Coord_t x, Coord_t y, Coord_t target_x, Coord_t target_y) :
				uid(uid),
				x(x),
				y(y),
				target_x(target_x),
				target_y(target_y)
		{}
	};

	void move(Item& item)
	{
		Coord_t dx = sign(item.target_x - item.x);
		Coord_t dy = sign(item.target_y - item.y);
		if (dx != 0 || dy != 0)
		{
			item.x += dx;
			item.y += dy;
			_log.log(time, sw::io::UnitMoved{item.uid, item.x, item.y});
		}
	}

	Time_t time = 1;
	Logger& _log;
	std::vector<Item> _items;
	std::unordered_map<UID_t, size_t> _items_index;
};
