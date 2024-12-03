#pragma once

#include <vector>

template <typename... Types>
class Notifier;

template <typename T1, typename... TRest>
class Notifier<T1, TRest...> : public Notifier<T1>, public Notifier<TRest...>
{
public:
	using Notifier<T1>::notify;
	using Notifier<TRest...>::notify;
};

template <typename T1>
class Notifier<T1>
{
public:
	virtual ~Notifier() = default;

	virtual void notify(T1) = 0;
};

template <typename Notifier>
class Sender
{
public:
	void subscribe(Notifier* notifier) {
        _items.push_back(notifier);
    }

    template<typename T>
    void notify(T&& t)
    {
        for(auto& item:_items)
        {
            item->notify(std::forward<T>(t));
        }
    }

private:
    std::vector<Notifier*> _items;
};