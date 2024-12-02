#include "Game.hpp"
#include "Tests/AssertLog.hpp"
#include "Units/types.hpp"

#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>

class TestSuiteBase
{
private:
	static int testcounter;

protected:
	void incCounter()
	{
		++testcounter;
	}

public:
	static int getCounter()
	{
		return testcounter;
	}
};

int TestSuiteBase::testcounter = 0;

class TestSuitePoint : public TestSuiteBase
{
public:
	void testDistance()
	{
		Point p11{1, 1};
		Point p00{0, 0};
		Point p05{0, 5};
		Point p40{4, 0};
		Point p78{7, 8};

		assert(distance(p00, p00) == 0);
		assert(distance(p11, p00) == 1);
		assert(distance(p11, p05) == 4);
		assert(distance(p05, p11) == 4);
		assert(distance(p40, p11) == 3);
		assert(distance(p11, p40) == 3);
		assert(distance(p05, p78) == 7);
		assert(distance(p40, p78) == 8);
		incCounter();
	}

	void testLessEq3()
	{
		assert(less_eq3(0, 0, 0) == true);
		assert(less_eq3(0, 0, 1) == true);
		assert(less_eq3(0, 1, 1) == true);
		assert(less_eq3(0, 2, 1) == false);
		assert(less_eq3(0, -1, 1) == false);
		assert(less_eq3(2, 2, 1) == false);
		assert(less_eq3(2, 1, 1) == false);
		assert(less_eq3(1, 2, 3) == true);
		incCounter();
	}
};

template <template <typename> typename Game, typename TestLogger>
class TestSuite : public TestSuiteBase
{
private:
	TestLogger log;
	Game<TestLogger> game;
	static int testcounter;

public:
	TestSuite() :
			log(),
			game(log)

	{}

	void testMapSize()
	{
		game.createField(4, 9);
		log.expect("[1] MAP_CREATED width=4 height=9 ");
		incCounter();
	}

	void testCreateGame()
	{
		game.createField(10, 10);
		log.expect("[1] MAP_CREATED width=10 height=10 ");

		game.spawnSwordsman(1, 0, 0, 5, 2);
		log.expect("[1] UNIT_SPAWNED unitId=1 unitType=Swordsman x=0 y=0 ");

		game.spawnHunter(2, 9, 0, 10, 5, 1, 4);
		log.expect("[1] UNIT_SPAWNED unitId=2 unitType=Hunter x=9 y=0 ");

		game.march(1, 9, 0);
		log.expect("[1] MARCH_STARTED unitId=1 x=0 y=0 targetX=9 targetY=0 ");
		game.march(2, 0, 0);
		log.expect("[1] MARCH_STARTED unitId=2 x=9 y=0 targetX=0 targetY=0 ");
		game.spawnSwordsman(3, 0, 9, 10, 2);
		log.expect("[1] UNIT_SPAWNED unitId=3 unitType=Swordsman x=0 y=9 ");
		game.march(3, 0, 0);
		log.expect("[1] MARCH_STARTED unitId=3 x=0 y=9 targetX=0 targetY=0 ");
		incCounter();
	}

	void testNeighbors()
	{
		game.createField(10, 10);
		game.spawnSwordsman(10, 0, 0, 5, 2);
		game.spawnSwordsman(11, 0, 1, 5, 2);
		game.spawnSwordsman(12, 0, 2, 5, 2);
		game.spawnSwordsman(13, 1, 0, 5, 2);
		game.spawnSwordsman(14, 1, 1, 5, 2);
		game.spawnSwordsman(15, 1, 2, 5, 2);
		game.spawnSwordsman(16, 2, 0, 5, 2);
		game.spawnSwordsman(17, 2, 1, 5, 2);
		game.spawnSwordsman(18, 2, 2, 5, 2);
		game.spawnSwordsman(19, 3, 3, 5, 2);
		game.spawnSwordsman(20, 5, 5, 5, 2);
		Item i00 = game.getItem(10);
		Item i11 = game.getItem(14);
		Item i55 = game.getItem(20);
		auto neighbors = game.getNeighbors(i55, 1, 1);
		assert(neighbors.empty());
		neighbors = game.getNeighbors(i11, 1, 1);
		assert(neighbors.size() == 8);

		log.forget();
		incCounter();
	}

	void testGameLoop()
	{
		game.createField(10, 10);
		game.spawnSwordsman(1, 0, 0, 5, 2);
		game.spawnHunter(2, 9, 0, 10, 5, 1, 4);
		game.march(1, 9, 0);
		game.march(2, 0, 0);
		game.spawnSwordsman(3, 0, 9, 10, 2);
		game.march(3, 0, 0);

		log.forget();
		assert(game.update() == true);
		log.expect("[2] UNIT_MOVED unitId=1 x=1 y=0 ");
		log.expect("[2] UNIT_MOVED unitId=2 x=8 y=0 ");
		log.expect("[2] UNIT_MOVED unitId=3 x=0 y=8 ");

		game.update();
		log.expect("[3] UNIT_MOVED unitId=1 x=2 y=0 ");
		log.expect("[3] UNIT_MOVED unitId=2 x=7 y=0 ");
		log.expect("[3] UNIT_MOVED unitId=3 x=0 y=7 ");

		game.update();
		log.expect("[4] UNIT_MOVED unitId=1 x=3 y=0 ");
		log.expect("[4] UNIT_ATTACKED attackerUnitId=2 targetUnitId=1 damage=5 targetHp=0 ");
		log.expect("[4] UNIT_DIED unitId=1 ");
		log.expect("[4] UNIT_MOVED unitId=3 x=0 y=6 ");

		game.update();
		log.expect("[5] UNIT_MOVED unitId=2 x=6 y=0 ");
		log.expect("[5] UNIT_MOVED unitId=3 x=0 y=5 ");

		game.update();
		log.expect("[6] UNIT_MOVED unitId=2 x=5 y=0 ");
		log.expect("[6] UNIT_MOVED unitId=3 x=0 y=4 ");

		game.update();
		//[7] UNIT_ATTACKED attackerUnitId=2 targetUnitId=3 damage=5 targetHp=5
		// imposible, because distance((5,0), (0,4)) == 5 (>range = 4)
		log.expect("[7] UNIT_MOVED unitId=2 x=4 y=0 ");
		log.expect("[7] UNIT_MOVED unitId=3 x=0 y=3 ");

		game.update();
		log.expect("[8] UNIT_ATTACKED attackerUnitId=2 targetUnitId=3 damage=5 targetHp=5 ");
		log.expect("[8] UNIT_MOVED unitId=3 x=0 y=2 ");

		assert(game.update() == false);
		log.expect("[9] UNIT_ATTACKED attackerUnitId=2 targetUnitId=3 damage=5 targetHp=0 ");
		log.expect("[9] UNIT_DIED unitId=3 ");

		incCounter();
	}
};

int main(int argc, char** argv)
{
	TestSuite<Game, AssertEventLog>().testMapSize();
	TestSuite<Game, AssertEventLog>().testCreateGame();
	TestSuite<Game, AssertEventLog>().testNeighbors();
	TestSuite<Game, AssertEventLog>().testGameLoop();
	TestSuitePoint().testDistance();
	TestSuitePoint().testLessEq3();

	std::cout << TestSuiteBase::getCounter() << " TESTS PASSED!!!\n";
}
