#include "Game.hpp"

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
#include <fstream>
#include <iostream>

int main(int argc, char** argv)
{
	using namespace sw;

	if (argc != 2)
	{
		throw std::runtime_error("Error: No file specified in command line argument");
	}

	std::ifstream file(argv[1]);
	if (!file)
	{
		throw std::runtime_error("Error: File not found - " + std::string(argv[1]));
	}

	EventLog eventLog;
	Game game(eventLog);

	io::CommandParser parser;
	parser.add<io::CreateMap>([&game](auto command) { game.createField(command); })
		.add<io::SpawnSwordsman>([&game](auto command) { game.spawnSwordsman(command); })
		.add<io::SpawnHunter>([&game](auto command) { game.spawnHunter(command); })
		.add<io::March>([&game](auto command) { game.march(command); });

	parser.parse(file);
	while (game.update())
	{
		/* pass */
	}

	return 0;
}
