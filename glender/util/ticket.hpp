#pragma once

#include <functional>

namespace mlib{namespace glender{namespace util
{

class Ticket
{
public:
	Ticket() = default;
	Ticket(std::function<void()> destructor) : Destructor(std::move(destructor)) {}

	Ticket(const Ticket&) = delete;
	Ticket &operator=(const Ticket&) = delete;

	void move(Ticket &&rhs)
	{
		release();
		Destructor = std::move(rhs.Destructor);
	}

	Ticket(Ticket &&rhs)
	{
		move(std::move(rhs));
	}
	Ticket &operator=(Ticket &&rhs)
	{
		move(std::move(rhs));
		return *this;
	}

	void release()
	{
		if(Destructor)
			Destructor();
		Destructor = nullptr;
	}

	~Ticket()
	{
		if(Destructor)
			Destructor();
	}

private:
	std::function<void()> Destructor;
};

}}}