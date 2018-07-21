#pragma once

#include <functional>
#include <list>
#include <utility>
#include <iterator>

#include "../util/ticket.hpp"
#include "../util/minticket.hpp"

namespace mlib{namespace glender
{

//
// value host
//
// Holds modifiable object of type T which clients may observe when changed.
//

template<class T>
class ValueHost
{
public:
    using Listener = std::function<void(const T&)>;

private:
    T Value;
    std::list<Listener> Listeners;

    void change(const T&v)
    {
        Value = v;
    }
    void notifyObservers()
    {
        for(auto o : Listeners)
            o(Value);
    }

public:
    ValueHost() = default;

    template<class... Args>
    ValueHost(Args &&...args) : Value(std::forward<Args>(args)...) {}

    using ListenerTicket = util::Ticket;
    ListenerTicket observe(Listener listener)
    {
        Listeners.emplace_back(std::move(listener));
        auto it = std::prev(Listeners.end());
        return ListenerTicket([this, it]()
        {
            Listeners.erase(it);
        });
    }

    const T &value() const { return Value; }
    void value(const T &newvalue)
    {
        change(newvalue);
        notifyObservers();
    }

    using ModifyTicket = util::MinimalTicket<ValueHost<T>, const T&, &ValueHost<T>::change>;
    ModifyTicket modify(const T &tempval)
    {
        ModifyTicket ticket(*this, Value);
        value(tempval);
        return ticket;
    }

    ValueHost(ValueHost&&) = delete;
    ValueHost(const ValueHost&) = delete;
    ValueHost &operator=(ValueHost&&) = delete;
    ValueHost &operator=(const ValueHost&) = delete;
};

}}