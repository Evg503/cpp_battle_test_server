#pragma once

#include "Units/Item.hpp"
#include "Util/GameNotifier.hpp"

#include <memory>
#include <vector>

using PItem = std::shared_ptr<Item>;

class Neighbor {
    public:
    	virtual std::vector<PItem> getNeighbors(const PItem main_item, Coord_t min_d, Coord_t max_d) =0;

};

class GameField : public Sender<GameNotifier>, public Neighbor
{
private:
	std::vector<PItem> _items;
	std::vector<PItem> _field;
	Point _fieldsize;

public:
	void create(Coord_t width, Coord_t height)
	{
		if (!_field.empty())
		{
			throw std::runtime_error("Field already created");
		}
		_fieldsize = {width, height};
		_field.resize(width * height);
		notify(sw::io::MapCreated{width, height});
	}

	void spawn(PItem item)
	{
		_items.push_back(item);
			placeToField(item);
	}

	void placeToField(PItem item)
	{
		auto& cell = getFieldPoint(item->pos);
		if (cell != nullptr)
		{
			throw std::runtime_error("Cell already used");
		}
		cell = item;
	}

	Coord_t distance(const PItem lhs, const PItem rhs)
	{
		return ::distance(lhs->pos, rhs->pos);
	}

	PItem& getFieldPoint(Point pos)
	{
		auto x = pos.x;
		auto y = pos.y;
		if (!less_eq3(0, x, _fieldsize.x - 1) || !less_eq3(0, y, _fieldsize.y - 1))
		{
			throw std::runtime_error("Position out of field");
		}
		auto idx = x + y * _fieldsize.x;
		return _field[idx];
	}

	PItem getItem(UID_t uid)
	{
		auto it = std::find_if(_items.begin(), _items.end(), [uid](auto& item) { return item->uid == uid; });
		if (it != _items.end())
		{
			return *it;
		}
		throw std::runtime_error("Item not found");
	}

	std::vector<PItem> getNeighbors(const PItem main_item, Coord_t min_d, Coord_t max_d)
	{
		std::vector<PItem> result;
		auto begin_x = std::clamp(0, main_item->pos.x - max_d, _fieldsize.x - 1);
		auto end_x = std::clamp(0, main_item->pos.x + max_d, _fieldsize.x - 1);
		auto begin_y = std::clamp(0, main_item->pos.y - max_d, _fieldsize.y - 1);
		auto end_y = std::clamp(0, main_item->pos.y + max_d, _fieldsize.y - 1);
		for (Coord_t y = begin_y; y <= end_y; ++y)
		{
			for (Coord_t x = begin_x; x <= end_x; ++x)
			{
				auto item = getFieldPoint({x, y});
				if (item && item != main_item && item->isAttacable()
					&& less_eq3(min_d, distance(main_item, item), max_d))
				{
					result.push_back(item);
				}
			}
		}
		return result;
	}

	void relax()
	{
		clear_deaded();
	}

    size_t count(){
        return _items.size();
    }
	void clear_deaded()
	{
		_items.erase(
			std::remove_if(_items.begin(), _items.end(), [](auto& item) { return !item->isAlive(); }), _items.end());
	}

    void update(auto P)
    {
        for(auto& item:_items)
        {
            P(item);
        }
    }

	~GameField();
};

GameField::~GameField() {}
