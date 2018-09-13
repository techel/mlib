#include "com.hpp"

#include <Objbase.h>
#include <cassert>
#include <atomic>

namespace mlib::windows::com
{

static thread_local int NumInitializations = 0;

Ticket::Ticket(Model m)
{
	if(NumInitializations++ == 0)
		CoInitializeEx(nullptr, static_cast<DWORD>(m));
}
Ticket::~Ticket()
{
	assert(NumInitializations > 0);
	if(--NumInitializations == 0)
		CoUninitialize();
}

void CoTaskMemFreeHandler::operator()(void *p)
{
	if(p)
		CoTaskMemFree(p);
}

}