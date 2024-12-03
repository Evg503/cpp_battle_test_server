#pragma once

#include "Util/notifier.hpp"

#include <IO/Events/MapCreated.hpp>
#include <IO/Events/MarchEnded.hpp>
#include <IO/Events/MarchStarted.hpp>
#include <IO/Events/UnitAttacked.hpp>
#include <IO/Events/UnitDied.hpp>
#include <IO/Events/UnitMoved.hpp>
#include <IO/Events/UnitSpawned.hpp>

using FieldNodifier = Notifier<sw::io::UnitMoved, sw::io::UnitDied, sw::io::UnitSpawned>;

using OterNotifier = Notifier<
	sw::io::MarchStarted,
	sw::io::MarchEnded,
	sw::io::UnitAttacked,
	sw::io::MapCreated>;

class GameNotifier :public FieldNodifier, public OterNotifier
{
    public:
    using FieldNodifier::notify;
    using OterNotifier::notify;

};
