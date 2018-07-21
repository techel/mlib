#pragma once

#include <cassert>
#include <utility>
#include <optional>

namespace mlib{namespace util
{

template<class ResetParameter, void(*ResetFunction)(ResetParameter)>
class MinimalFunctionTicket
{
	typedef std::decay_t<ResetParameter> ResetType;

	std::optional<ResetType> ResetValue;

public:
	MinimalFunctionTicket(ResetType resetValue) : ResetValue(std::move(resetValue)) {}

	~MinimalFunctionTicket()
	{
		if(ResetValue)
			ResetFunction(std::move(*ResetValue));
	}

	MinimalFunctionTicket(MinimalFunctionTicket&&) = default;
	MinimalFunctionTicket &operator=(MinimalFunctionTicket&&) = default;

	MinimalFunctionTicket(const MinimalFunctionTicket&) = delete;
	MinimalFunctionTicket &operator=(const MinimalFunctionTicket&) = delete;
};


template<class OwnerClass, class ResetParameter, void(OwnerClass::*ResetMethod)(ResetParameter)>
class MinimalTicket
{
	typedef std::decay_t<ResetParameter> ResetType;

	OwnerClass *Owner;
	ResetType ResetValue;

public:
	MinimalTicket(OwnerClass &own, ResetType resetValue)
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
};

}}