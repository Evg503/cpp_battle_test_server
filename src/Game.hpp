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
#include <memory>
#include <random>
#include <unordered_map>
#include <vector>

template <typename Logger>
class Game : public GameNotifier
{
	using PItem = std::shared_ptr<Item>;

public:
	Game(Logger& log) :
			_log(log)
	{}

	template <class TEvent>
	void log(TEvent&& event)
	{
		_log.log(time, std::forward<TEvent>(event));
	}

	void notify(sw::io::UnitAttacked&& event) override
	{
		log(std::forward<sw::io::UnitAttacked>(event));
	}

	void notify(sw::io::MarchStarted&& event) override
	{
		log(std::forward<sw::io::MarchStarted>(event));
	}

	void notify(sw::io::MarchEnded&& event) override
	{
		log(std::forward<sw::io::MarchEnded>(event));
	}

	void notify(sw::io::UnitMoved&& event) override
	{
		auto item = getFieldPoint(event.from);
		placeToField(item);

		log(std::forward<sw::io::UnitMoved>(event));
	}

	void notify(sw::io::UnitDied&& event) override
	{
		getFieldPoint(event.pos).reset();

		log(std::forward<sw::io::UnitDied>(event));
	}

	void createField(const sw::io::CreateMap& command)
	{
		createField(command.width, command.height);
	}

	void createField(Coord_t width, Coord_t height)
	{
		if (!_field.empty())
		{
			throw std::runtime_error("Field already created");
		}
		_fieldsize = {width, height};
		_field.resize(width * height);
		log(sw::io::MapCreated{width, height});
	}

	void spawnSwordsman(const sw::io::SpawnSwordsman& command)
	{
		spawnSwordsman(command.unitId, command.x, command.y, command.hp, command.strength);
	}

	void spawnSwordsman(UID_t uid, Coord_t x, Coord_t y, Health_t hp, Health_t strength)
	{
		auto item = std::make_shared<Swordsman>(this, uid, x, y, hp, strength);
		_items.push_back(item);
		log(sw::io::UnitSpawned{item->uid, "Swordsman", item->pos.x, item->pos.y});
	}

	void spawnHunter(const sw::io::SpawnHunter& command)
	{
		spawnHunter(command.unitId, command.x, command.y, command.hp, command.agility, command.strength, command.range);
	}

	void spawnHunter(UID_t uid, Coord_t x, Coord_t y, Health_t hp, Health_t agility, Health_t strength, Coord_t range)
	{
		auto item = std::make_shared<Hunter>(this, uid, x, y, hp, strength, agility, range);
		_items.push_back(item);
		log(sw::io::UnitSpawned{item->uid, "Hunter", item->pos.x, item->pos.y});
	}

	PItem& getFieldPoint(Point pos)
	{
		auto x = pos.x;
		auto y = pos.y;
		if (!less_eq3(0, x, _fieldsize.x - 1) || !less_eq3(0, y, _fieldsize.y - 1))
		{
			throw std::runtime_error("Position out of field");
		}
		auto idx = x + y * _fieldsize.x;
		return _field[idx];
	}

	void placeToField(PItem item)
	{
		auto& cell = getFieldPoint(item->pos);
		if (cell != nullptr)
		{
			throw std::runtime_error("Cell already used");
		}
		cell = item;
	}

	void march(const sw::io::March& command)
	{
		march(command.unitId, command.targetX, command.targetY);
	}

	void march(UID_t uid, Coord_t target_x, Coord_t target_y)
	{
		auto item = getItem(uid);
		item->march(Point{target_x, target_y});
	}

	Coord_t distance(const PItem lhs, const PItem rhs)
	{
		return ::distance(lhs->pos, rhs->pos);
	}

	PItem getItem(UID_t uid)
	{
		auto it = std::find_if(_items.begin(), _items.end(), [uid](auto& item) { return item->uid == uid; });
		if (it != _items.end())
		{
			return *it;
		}
		throw std::runtime_error("Item not found");
	}

	std::vector<PItem> getNeighbors(const PItem main_item, Coord_t min_d, Coord_t max_d)
	{
		std::vector<PItem> result;
		auto begin_x = std::clamp(0, main_item->pos.x - max_d, _fieldsize.x - 1);
		auto end_x = std::clamp(0, main_item->pos.x + max_d, _fieldsize.x - 1);
		auto begin_y = std::clamp(0, main_item->pos.y - max_d, _fieldsize.y - 1);
		auto end_y = std::clamp(0, main_item->pos.y + max_d, _fieldsize.y - 1);
		for (Coord_t y = begin_y; y <= end_y; ++y)
		{
			for (Coord_t x = begin_x; x <= end_x; ++x)
			{
				auto item = getFieldPoint({x, y});
				if (item && item != main_item && item->isAttacable()
					&& less_eq3(min_d, distance(main_item, item), max_d))
				{
					result.push_back(item);
				}
			}
		}
		return result;
	}

	bool loAttack(PItem item)
	{
		auto neighbors = getNeighbors(item, 1, 1);
		if (neighbors.empty())
		{
			return false;
		}
		std::uniform_int_distribution<> distrib(0, neighbors.size() - 1);
		auto victim = neighbors[distrib(gen)];
		item->attack(victim.get(), item->strength);
		return true;
	}

	bool hiAttack(PItem item)
	{
		auto range = item->getRange();
		if (range != 0)
		{
			auto neighbors = getNeighbors(item, 2, range);
			if (!neighbors.empty())
			{
				std::uniform_int_distribution<> distrib(0, neighbors.size() - 1);
				auto victim = neighbors[distrib(gen)];
				item->attack(victim.get(), item->getAgility());
				return true;
			}
		}
		return false;
	}

	bool attack(PItem item)
	{
		return loAttack(item) || hiAttack(item);
	}

	bool move(PItem item)
	{
		return item->move();
	}

	void prepareField()
	{
		_field.assign(_field.size(), nullptr);
		for (auto item : _items)
		{
			placeToField(item);
		}
	}

	bool update()
	{
		prepareField();
		bool change_detected = false;
		time += 1;
		int unit_count = 0;
		for (auto& item : _items)
		{
			bool item_changes = attack(item) || move(item);
			change_detected = change_detected || item_changes;
		}
		_field.assign(_field.size(), nullptr);
		if (change_detected)
		{
			clear_deaded();
			return _items.size() > 1;
		}
		return change_detected;
	}

	void clear_deaded()
	{
		_items.erase(
			std::remove_if(_items.begin(), _items.end(), [](auto& item) { return !item->isAlive(); }), _items.end());
	}

private:
	std::mt19937 gen;
	Time_t time = 1;
	Logger& _log;
	std::vector<PItem> _items;
	std::vector<PItem> _field;
	Point _fieldsize;
};
