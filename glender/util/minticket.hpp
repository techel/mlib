#pragma once

#include <cassert>
#include <utility>
#include <optional>

namespace mlib{namespace glender{namespace util
{

template<class ResetParameter, void(*ResetFunction)(ResetParameter)>
class MinimalFunctionTicket
{
public:
	MinimalFunctionTicket(ResetParameter resetValue) : ResetValue(std::move(resetValue)) {}

	~MinimalFunctionTicket()
	{
		if(ResetValue)
			ResetFunction(std::move(*ResetValue));
	}

	MinimalFunctionTicket(MinimalFunctionTicket&&) = default;
	MinimalFunctionTicket &operator=(MinimalFunctionTicket&&) = default;

	MinimalFunctionTicket(const MinimalFunctionTicket&) = delete;
	MinimalFunctionTicket &operator=(const MinimalFunctionTicket&) = delete;

private:
	typedef std::decay_t<ResetParameter> ResetType;

	std::optional<ResetType> ResetValue;
};


template<class OwnerClass, class ResetParameter, void(OwnerClass::*ResetMethod)(ResetParameter)>
class MinimalTicket
{
public:
	MinimalTicket(OwnerClass &own, ResetParameter resetValue)
		: Owner(&own), ResetValue(std::move(resetValue)) {}

	MinimalTicket(MinimalTicket &&rhs)
	{
		ResetValue = rhs.ResetValue;
		Owner = rhs.Owner;
		rhs.Owner = nullptr;
	}
	MinimalTicket &operator=(MinimalTicket &&rhs)
	{
		ResetValue = rhs.ResetValue;
		Owner = rhs.Owner;
		rhs.Owner = nullptr;
		return *this;
	}
	~MinimalTicket()
	{
		if(Owner)
			(Owner->*ResetMethod)(std::move(ResetValue));
	}

	MinimalTicket(const MinimalTicket&) = delete;
	MinimalTicket &operator=(const MinimalTicket&) = delete;

private:
	typedef std::decay_t<ResetParameter> ResetType;

	OwnerClass *Owner;
    std::decay_t<ResetParameter> ResetValue;
};

}}}