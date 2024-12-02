#pragma once
#include "Units/types.hpp"

#include <cassert>
#include <deque>
#include <iostream>
#include <sstream>
#include <string>

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
	void log(Time_t tick, TEvent&& event)
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
			return;
		}

		if (_last_lines.front() != exp)
		{
			std::cerr << "Expected \"" << exp << "\" != \"" << _last_lines.front() << "\"" << std::endl;
			assert(false);
			return;
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
