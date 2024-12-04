#pragma once

class Action
{
public:
	virtual bool operator()() = 0;

	virtual ~Action() = default;
};

