#pragma once
#include "Units/Item.hpp"

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
#include <IO/Events/UnitSpawned.hpp>
#include <IO/System/CommandParser.hpp>
#include <IO/System/EventLog.hpp>
#include <IO/System/PrintDebug.hpp>

template <typename Logger>
class Game
{
	inline static std::unordered_map<std::string, std::string> klasses = {
		{"SWORDMAN", "Swordsman"},
		{"HUNTER", "Hunter"},
	};
    using Item = Item<Logger>;
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
		auto& item = _items.emplace_back(uid, x, y);
		_log.log(time, sw::io::UnitSpawned{item.uid, klasses[klass], item.pos.x, item.pos.y});
	}

	void march(UID_t uid, Coord_t target_x, Coord_t target_y)
	{
		auto idx = _items_index[uid];
		auto& item = _items[idx];
		item.target.x = target_x;
		item.target.y = target_y;
		_log.log(time, sw::io::MarchStarted{item.uid, item.pos.x, item.pos.y, item.target.x, item.target.y});
	}

	void update()
	{
		time += 1;
		for (auto& item : _items)
		{
			item.move(time, _log);
		}
	}

private:

	Time_t time = 1;
	Logger& _log;
	std::vector<Item> _items;
	std::unordered_map<UID_t, size_t> _items_index;
};
