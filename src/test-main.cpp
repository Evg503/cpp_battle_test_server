#include "Units/types.hpp"

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
#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>

class PrintFieldVisitor
{
private:
	std::ostream& _stream;

public:
	explicit PrintFieldVisitor(std::ostream& stream) :
			_stream(stream)
	{}

	template <typename T>
	void visit(const char* name, const T& value)
	{
		_stream << name << "=" << value << ' ';
	}
};

class AssertEventLog
{
public:
	template <class TEvent>
	void log(uint64_t tick, TEvent&& event)
	{
		std::stringstream str;
		str << "[" << tick << "] " << TEvent::Name << " ";
		PrintFieldVisitor visitor(str);
		event.visit(visitor);

		_last_lines.push_back(str.str());
	}

	void expect(const char* exp)
	{
		if (_last_lines.empty())
		{
			std::cerr << "Expected \"" << exp << "\" but no lines" << std::endl;
			assert(false);
		}

		if (_last_lines.front() != exp)
		{
			std::cerr << "Expected \"" << exp << "\" != \"" << _last_lines.front() << "\"" << std::endl;
			assert(false);
		}
		_last_lines.pop_front();
	}

	void forget()
	{
		_last_lines.clear();
	}
    ~AssertEventLog()
    {
        assert(_last_lines.empty());
    }
private:
	std::deque<std::string> _last_lines;
};

template <typename Game, typename TestLogger>
void testMapSize()
{
	TestLogger log;
	Game game(log);

	game.createField(4, 9);
	log.expect("[1] MAP_CREATED width=4 height=9 ");
}

template <typename Game, typename TestLogger>
void testCreateGame()
{
	TestLogger log;
	Game game(log);

	game.createField(10, 10);
	log.expect("[1] MAP_CREATED width=10 height=10 ");

	game.spawn("SWORDMAN", 1, 0, 0, 5, 2);
	log.expect("[1] UNIT_SPAWNED unitId=1 unitType=Swordsman x=0 y=0 ");

	game.spawn("HUNTER", 2, 9, 0, 10, 5, 1, 4);
	log.expect("[1] UNIT_SPAWNED unitId=2 unitType=Hunter x=9 y=0 ");

	game.march(1, 9, 0);
	log.expect("[1] MARCH_STARTED unitId=1 x=0 y=0 targetX=9 targetY=0 ");
	game.march(2, 0, 0);
	log.expect("[1] MARCH_STARTED unitId=2 x=9 y=0 targetX=0 targetY=0 ");
	game.spawn("SWORDMAN", 3, 0, 9, 10, 2);
	log.expect("[1] UNIT_SPAWNED unitId=3 unitType=Swordsman x=0 y=9 ");
	game.march(3, 0, 0);
	log.expect("[1] MARCH_STARTED unitId=3 x=0 y=9 targetX=0 targetY=0 ");
}

template <typename Game, typename TestLogger>
void testGameLoop()
{
	TestLogger log;
	Game game(log);

	game.createField(10, 10);
	game.spawn("SWORDMAN", 1, 0, 0, 5, 2);
	game.spawn("HUNTER", 2, 9, 0, 10, 5, 1, 4);
	game.march(1, 9, 0);
	game.march(2, 0, 0);
	game.spawn("SWORDMAN", 3, 0, 9, 10, 2);
	game.march(3, 0, 0);

	log.forget();
	game.update();
	log.expect("[2] UNIT_MOVED unitId=1 x=1 y=0 ");
	log.expect("[2] UNIT_MOVED unitId=2 x=8 y=0 ");
	log.expect("[2] UNIT_MOVED unitId=3 x=0 y=8 ");

	game.update();
	log.expect("[3] UNIT_MOVED unitId=1 x=2 y=0 ");
	log.expect("[3] UNIT_MOVED unitId=2 x=7 y=0 ");
	log.expect("[3] UNIT_MOVED unitId=3 x=0 y=7 ");

    /*
[4] UNIT_MOVED unitId=1 x=3 y=0 
[4] UNIT_ATTACKED attackerUnitId=2 targetUnitId=1 damage=5 targetHp=0 
[4] UNIT_DIED unitId=1 
[4] UNIT_MOVED unitId=3 x=0 y=6 
[5] UNIT_MOVED unitId=2 x=6 y=0 
[5] UNIT_MOVED unitId=3 x=0 y=5 
[6] UNIT_MOVED unitId=2 x=5 y=0 
[6] UNIT_MOVED unitId=3 x=0 y=4 
[7] UNIT_ATTACKED attackerUnitId=2 targetUnitId=3 damage=5 targetHp=5 
[7] UNIT_MOVED unitId=3 x=0 y=3 
[8] UNIT_ATTACKED attackerUnitId=2 targetUnitId=3 damage=5 targetHp=0 
[8] UNIT_DIED unitId=3 

     */
}

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

int main(int argc, char** argv)
{
	testMapSize<Game<AssertEventLog>, AssertEventLog>();
	testCreateGame<Game<AssertEventLog>, AssertEventLog>();
	testGameLoop<Game<AssertEventLog>, AssertEventLog>();

	std::cout << "TESTS PASSED!!!\n";
}
