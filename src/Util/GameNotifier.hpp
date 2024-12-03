#pragma once

#include "Util/notifier.hpp"

#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>

using MovedNodifier = Notifier<sw::io::UnitMoved>;
using ExistNodifier = Notifier<sw::io::UnitDied, sw::io::UnitSpawned>;

class FieldNodifier : public MovedNodifier, public ExistNodifier
{
public:
	using ExistNodifier::notify;
	using MovedNodifier::notify;
};

using MarchNotifier = Notifier<sw::io::MarchStarted, sw::io::MarchEnded>;

using OterNotifier = Notifier<sw::io::UnitAttacked, sw::io::MapCreated>;

class GameNotifier : public FieldNodifier, public OterNotifier, public MarchNotifier
{
public:
	using FieldNodifier::notify;
	using OterNotifier::notify;
	using MarchNotifier::notify;
};
