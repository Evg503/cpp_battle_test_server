#pragma once
#include "GameField.hpp"
#include "Units/Hunter.hpp"
#include "Units/Swordsman.hpp"
#include "Util/SubscriptedLogger.hpp"

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
#include <unordered_map>
#include <vector>

template <typename Logger>
class Game : public FieldNodifier
{
	using PItem = std::shared_ptr<Item>;

public:
	Game(Logger& log) :
			_log(log)
	{
		_gamefield.subscribe(&_log);
	}

	template <class TEvent>
	void log(TEvent&& event)
	{
		_log.log(std::forward<TEvent>(event));
	}

	void notify(sw::io::UnitMoved&& event) override
	{
		auto& item = getFieldPoint(event.from);
		placeToField(item);
		item.reset();
	}

	void notify(sw::io::UnitDied&& event) override
	{
		getFieldPoint(event.pos).reset();
	}

	void notify(sw::io::UnitSpawned&& event) override
	{
		// because logger is not subscribed yet
		log(std::forward<sw::io::UnitSpawned>(event));
	}

	void createField(const sw::io::CreateMap& command)
	{
		createField(command.width, command.height);
	}

	void createField(Coord_t width, Coord_t height)
	{
		_gamefield.create(width, height);
	}

	void spawnSwordsman(const sw::io::SpawnSwordsman& command)
	{
		spawnSwordsman(command.unitId, command.x, command.y, command.hp, command.strength);
	}

	void spawnSwordsman(UID_t uid, Coord_t x, Coord_t y, Health_t hp, Health_t strength)
	{
		auto item = std::make_shared<Swordsman>(this, &_gamefield, uid, x, y, hp, strength);
		item->subscribe(&_log);
		_gamefield.spawn(item);
	}

	void spawnHunter(const sw::io::SpawnHunter& command)
	{
		spawnHunter(command.unitId, command.x, command.y, command.hp, command.agility, command.strength, command.range);
	}

	void spawnHunter(UID_t uid, Coord_t x, Coord_t y, Health_t hp, Health_t agility, Health_t strength, Coord_t range)
	{
		auto item = std::make_shared<Hunter>(this, &_gamefield, uid, x, y, hp, strength, agility, range);
		item->subscribe(&_log);
		_gamefield.spawn(item);
	}

	PItem& getFieldPoint(Point pos)
	{
		return _gamefield.getFieldPoint(pos);
	}

	void placeToField(PItem item)
	{
		_gamefield.placeToField(item);
	}

	void march(const sw::io::March& command)
	{
		march(command.unitId, command.targetX, command.targetY);
	}

	void march(UID_t uid, Coord_t target_x, Coord_t target_y)
	{
		auto item = _gamefield.getItem(uid);
		item->march(Point{target_x, target_y});
	}

	std::vector<PItem> getNeighbors(const PItem main_item, Coord_t min_d, Coord_t max_d)
	{
		return _gamefield.getNeighbors(main_item.get(), min_d, max_d);
	}

	PItem getItem(UID_t uid)
	{
		return _gamefield.getItem(uid);
	}

	bool update()
	{
		time += 1;
		_log.update(time);
		bool change_detected = false;

		_gamefield.update(
			[&change_detected, this](PItem& item)
			{
				bool item_changes = item->doActions();
				change_detected = change_detected || item_changes;
			});
		_gamefield.relax();
		return change_detected && _gamefield.count() > 1;
	}

private:
	Time_t time = 1;
	SubcriptedLogger<Logger> _log;
	GameField _gamefield;
};
