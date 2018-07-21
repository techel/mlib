#pragma once
#include <tuple>

namespace mlib{namespace glender{namespace util
{

//
// deterministic tuple
//
// Unlike std::tuple, constructs every element from first to last.
//

template<class T, class... RestT>
struct TupleElement
{
	T Value;
	TupleElement<RestT...> Tail;

	template<class... Args>
	TupleElement(Args&&... args) : Value(std::forward<Args>(args)...), Tail(std::forward<Args>(args)...) {}

	template<class... Args>
	TupleElement(const TupleElement<Args...> &src) : Value(src.Value), Tail(src.Tail) {}

	template<class... Args>
	TupleElement(TupleElement<Args...> &&src) : Value(std::move(src.Value)), Tail(std::move(src.Tail)) {}
};

template<class T>
struct TupleElement<T>
{
	T Value;
	template<class... Args>
	TupleElement(Args&&... args) : Value(std::forward<Args>(args)...) {}

	template<class... Args>
	TupleElement(const TupleElement<Args...> &src) : Value(src.Value) {}

	template<class... Args>
	TupleElement(TupleElement<Args...> &&src) : Value(std::move(src.Value)) {}
};

template<class First, class... Rest>
struct DTuple
{
	template<size_t Index, size_t Current, class... T>
	static auto &get(TupleElement<T...> &src, int)
	{
		return get<Index, Current + 1>(src.Tail, std::conditional_t<Current + 1 == Index, float, int>());
	}
	
	template<size_t Index, size_t Current, class... T>
	static auto &get(TupleElement<T...> &src, float)
	{
		return src.Value;
	}

	TupleElement<First, Rest...> Elements;

public:
	template<class... Args>
	DTuple(Args&&... args) : Elements(std::forward<Args>(args)...) {}

	template<class... T>
	DTuple(const DTuple<T...> &rhs) : Elements(rhs.Elements) {}

	template<size_t Index>
	auto &get()
	{
		static_assert(Index <= sizeof...(Rest), "index out of range");
		return get<Index, 0>(Elements, std::conditional_t<Index == 0, float, int>());
	}

	template<size_t Index>
	const auto &get() const { return const_cast<DTuple<First, Rest...>*>(this)->get<Index>(); }
};

}}}