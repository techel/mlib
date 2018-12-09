#pragma once

namespace mlib::interfaces
{

//
// key-value map interface
//

template<class K, class V>
struct IKeyvalue
{
    virtual ~IKeyvalue() = default;

    //stores value at key. Existing value will be overwritten.
    virtual void set(K key, V value) = 0;

    //returns pointer to value at key if exists or nullptr.
    virtual const V *find(const K &key) const = 0;
    virtual V *find(const K &key) = 0;
};

}