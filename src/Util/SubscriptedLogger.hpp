#pragma once

#include "Units/Item.hpp"

template <typename Logger>
class SubcriptedLogger : public GameNotifier
{
public:
	SubcriptedLogger(Logger& log) :
			time(1),
			_log(log)
	{}

	void update(Time_t t)
	{
		time = t;
	}

	template <class TEvent>
	void log(TEvent&& event)
	{
		_log.log(time, std::forward<TEvent>(event));
	}

private:
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
		log(std::forward<sw::io::UnitMoved>(event));
	}

	void notify(sw::io::UnitDied&& event) override
	{
		log(std::forward<sw::io::UnitDied>(event));
	}

	void notify(sw::io::UnitSpawned&& event) override
	{
		log(std::forward<sw::io::UnitSpawned>(event));
	}

	void notify(sw::io::MapCreated&& event) override
	{
		log(std::forward<sw::io::MapCreated>(event));
	}

private:
	Time_t time;
	Logger& _log;
};
