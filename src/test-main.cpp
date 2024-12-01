#include "Game.hpp"
#include "Tests/AssertLog.hpp"
#include "Units/types.hpp"

#include <cassert>
#include <deque>
#include <fstream>
#include <iostream>

template <template <typename> typename Game, typename TestLogger>
class TestSuite
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
    static int getCounter()
    {
        return testcounter;
    }
	void testMapSize()
	{
		game.createField(4, 9);
		log.expect("[1] MAP_CREATED width=4 height=9 ");
        ++testcounter;
	}

	void testCreateGame()
	{
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
        ++testcounter;
	}

	void testGameLoop()
	{
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

		// game.update();
		// log.expect("[4] UNIT_MOVED unitId=1 x=3 y=0 ");
		// log.expect("[4] UNIT_ATTACKED attackerUnitId=2 targetUnitId=1 damage=5 targetHp=0 ");
		// log.expect("[4] UNIT_DIED unitId=1 ");
		// log.expect("[4] UNIT_MOVED unitId=3 x=0 y=6 ");
		/*
[5] UNIT_MOVED unitId=2 x=6 y=0 
[5] UNIT_MOVED unitId=3 x=0 y=5 
[6] UNIT_MOVED unitId=2 x=5 y=0 
[6] UNIT_MOVED unitId=3 x=0 y=4 
[7] UNIT_ATTACKED attackerUnitId=2 targetUnitId=3 damage=5 targetHp=5 
[7] UNIT_MOVED unitId=3 x=0 y=3 
[8] UNIT_ATTACKED attackerUnitId=2 targetUnitId=3 damage=5 targetHp=0 
[8] UNIT_DIED unitId=3 

     */
        ++testcounter;
	}
};
// template <>
// int TestSuite<Game, AssertEventLog>::testcounter = 0;
template <template <typename> typename Game, typename TestLogger>
int TestSuite<Game, TestLogger>::testcounter = 0;
;
int main(int argc, char** argv)
{
	TestSuite<Game, AssertEventLog>().testMapSize();
	TestSuite<Game, AssertEventLog>().testCreateGame();
	TestSuite<Game, AssertEventLog>().testGameLoop();

	std::cout << TestSuite<Game, AssertEventLog>::getCounter() << " TESTS PASSED!!!\n";
}
