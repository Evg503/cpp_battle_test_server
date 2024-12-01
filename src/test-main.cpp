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

		auto result = str.str();

		if (_expected != result)
		{
			std::cerr << "Expected \"" << _expected << "\" != \"" << result << "\"" << std::endl;
			assert(false);
		}
		_expected.clear();
	}

	~AssertEventLog()
	{
		assert(_expected.empty());
	}

	void visit();

	void expect(const char* exp)
	{
		_expected = exp;
	}

private:
	std::string _expected;
};

template <typename Game, typename TestLogger>
void testMapSize()
{
	TestLogger log;
	Game game(log);

	log.expect("[1] MAP_CREATED width=4 height=9 ");
	game.createField(4, 9);
}

template <typename Game, typename TestLogger>
void testCreateGame()
{
	TestLogger log;
	Game game(log);

	log.expect("[1] MAP_CREATED width=10 height=10 ");
	game.createField(10, 10);

	log.expect("[1] UNIT_SPAWNED unitId=1 unitType=Swordsman x=0 y=0 ");
	game.spawn("SWORDMAN", 1, 0, 0, 5, 2);

	log.expect("[1] UNIT_SPAWNED unitId=2 unitType=Hunter x=9 y=0 ");
	game.spawn("HUNTER", 2, 9, 0, 10, 5, 1, 4);

	log.expect("[1] MARCH_STARTED unitId=1 x=0 y=0 targetX=9 targetY=0 ");
	game.march(1, 9, 0);
	log.expect("[1] MARCH_STARTED unitId=2 x=9 y=0 targetX=0 targetY=0 ");
	game.march(2, 0, 0);
	log.expect("[1] UNIT_SPAWNED unitId=3 unitType=Swordsman x=0 y=9 ");
	game.spawn("SWORDMAN", 3, 0, 9, 10, 2);
	log.expect("[1] MARCH_STARTED unitId=3 x=0 y=9 targetX=0 targetY=0 ");
	game.march(3, 0, 0);
}

using Coord_t = uint32_t;
using Time_t = int32_t;
using UID_t = uint32_t;

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
		_log.log(1, sw::io::MapCreated{W, H});
	}

	void spawn(const char* klass, UID_t uid, Coord_t x, Coord_t y, ...)
	{
		_items[uid] = Item{x, y};
		_log.log(1, sw::io::UnitSpawned{uid, klasses[klass], x, y});
	}

	void march(UID_t uid, Coord_t target_x, Coord_t target_y)
	{
		auto [x, y] = _items[uid];
		_log.log(1, sw::io::MarchStarted{uid, x, y, target_x, target_y});
	}

private:
	Logger& _log;

	struct Item
	{
		Coord_t x;
		Coord_t y;
	};

	std::unordered_map<UID_t, Item> _items;
};

int main(int argc, char** argv)
{
	testMapSize<Game<AssertEventLog>, AssertEventLog>();
	testCreateGame<Game<AssertEventLog>, AssertEventLog>();

	std::cout << "TESTS PASSED!!!\n";
}
