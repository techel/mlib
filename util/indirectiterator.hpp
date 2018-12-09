#pragma once
#include <utility>
#include <iterator>

namespace mlib{namespace util
{

//
// indirect iterator
//
// Wraps an iterator of a container which contains pointers.
//

template<class IterType>
class IndirectIterator
{
	IterType Iterator;

public:
	IndirectIterator(IterType i) : Iterator(std::move(i)) {}

    using value_type = typename IterType::value_type;
    using iterator_category = typename IterType::iterator_category;
    using difference_type = typename IterType::difference_type;
    using pointer = typename IterType::pointer;
    using reference = typename IterType::reference;

	IndirectIterator &operator++()
    {
        ++Iterator;
        return *this;
    }
    IndirectIterator &operator--()
    {
        --Iterator;
        return *this;
    }
	IndirectIterator operator++(int) { return IndirectIterator(Iterator++); }
	IndirectIterator operator--(int) { return IndirectIterator(Iterator--); }

    bool operator==(const IndirectIterator &rhs) const { return Iterator == rhs.Iterator; }
    bool operator!=(const IndirectIterator &rhs) const { return Iterator != rhs.Iterator; }
    bool operator<(const IndirectIterator &rhs) const { return Iterator < rhs.Iterator; }
    bool operator>(const IndirectIterator &rhs) const { return Iterator > rhs.Iterator; }
    bool operator>=(const IndirectIterator &rhs) const { return Iterator >= rhs.Iterator; }
    bool operator<=(const IndirectIterator &rhs) const { return Iterator <= rhs.Iterator; }

	auto &operator*() { return **Iterator; }
	const auto &operator*() const { return **Iterator; }

	auto *operator->() { return &**this; }
	const auto *operator->() const { return &**this; }
};

//
// Wraps an iterator of a container which contains tuples
//

template<size_t TupleIndex, class IterType>
class TupleIterator
{
    IterType Iterator;

public:
    TupleIterator(IterType i) : Iterator(std::move(i)) {}

    using value_type = typename IterType::value_type;
    using iterator_category = typename IterType::iterator_category;
    using difference_type = typename IterType::difference_type;
    using pointer = typename IterType::pointer;
    using reference = typename IterType::reference;

    TupleIterator &operator++()
    {
        ++Iterator;
        return *this;
    }
    TupleIterator &operator--()
    {
        --Iterator;
        return *this;
    }
    TupleIterator operator++(int) { return TupleIterator(Iterator++); }
    TupleIterator operator--(int) { return TupleIterator(Iterator--); }

    bool operator==(const TupleIterator &rhs) const { return Iterator == rhs.Iterator; }
    bool operator!=(const TupleIterator &rhs) const { return Iterator != rhs.Iterator; }
    bool operator<(const TupleIterator &rhs) const { return Iterator < rhs.Iterator; }
    bool operator>(const TupleIterator &rhs) const { return Iterator > rhs.Iterator; }
    bool operator>=(const TupleIterator &rhs) const { return Iterator >= rhs.Iterator; }
    bool operator<=(const TupleIterator &rhs) const { return Iterator <= rhs.Iterator; }

    auto &operator*()
    {
        using std::get;
        return get<TupleIndex>(*Iterator);
    }
    const auto &operator*() const
    {
        using std::get;
        return get<TupleIndex>(*Iterator);
    }

    auto *operator->() { return &**this; }
    const auto *operator->() const { return &**this; }
};


}}