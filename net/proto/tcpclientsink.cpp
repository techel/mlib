#include "tcpclientsink.hpp"

#include <cassert>

namespace mlib::net::tcp::client::async
{

Sink::Sink()
{
}
Sink::~Sink()
{
}
void Sink::sink(ISink &destination)
{
	Destination = &destination;
}
void Sink::observe(IListener &l)
{
	Listener = &l;
}
void Sink::connect(const Sock::IP &p, unsigned int port)
{
	MySocket = std::make_unique<Sock::Socket>(p.Type, Sock::Socket::Proto::Tcp);
	MySocket->block(false);

	const auto ret = MySocket->connect(p, port);
	assert(ret == Sock::Socket::ConnectStatus::InProgress);

	Connecting = true;
}
void Sink::writeSink(const char *source, size_t len)
{
	assert(MySocket && MySocket->getState() == Sock::Socket::State::Ready);

	auto res = MySocket->send(source, len);
	if(res == Sock::Socket::SendStatus::Disconnected)
	{
		if(Listener)
			Listener->onTcpDisconnected();
	}
}
void Sink::process()
{
	if(MySocket)
	{
		const auto state = MySocket->getState();
		if(state == Sock::Socket::State::Ready)
		{
			if(Connecting)
			{
				Connecting = false;
				if(Listener)
					Listener->onTcpConnected(true);
			}

			auto bytesAvail = MySocket->available();
			if(bytesAvail == 0)
				bytesAvail = 1;

			if(static_cast<size_t>(bytesAvail) > InputBuffer.size())
				InputBuffer.resize(bytesAvail);

			size_t recvlen = 0;
			auto res = MySocket->receive(&InputBuffer[0], bytesAvail, recvlen);
			if(res == Sock::Socket::ReceiveStatus::Disconnected)
			{
				if(Listener)
					Listener->onTcpDisconnected();
			}
			else if(res == Sock::Socket::ReceiveStatus::Timeout)
			{
				if(Listener)
					Listener->onTcpTimeout();
			}
			else if(res == Sock::Socket::ReceiveStatus::Available)
			{
				assert(Destination);
				Destination->writeSink(InputBuffer.data(), recvlen);
			}
		}
		else if(state == Sock::Socket::State::Disconnected && Connecting)
		{
			Connecting = false;
			if(Listener)
				Listener->onTcpConnected(false);
		}
	}
}

}}}}}