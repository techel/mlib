#pragma once
#include <memory>
#include <stdexcept>
#include <vector>
#include <memory>

#include "netsocket.hpp"
#include "../../stream/sinkinterface.hpp>

namespace mlib::net::tcp::client::async
{

using mlib::stream::ISink;

//
// exceptions
//

struct Error : public std::runtime_error
{
	Error(const std::string &s) : runtime_error("net.tcp.client.async" + s) {}
};

//
// listener interface
//

struct IListener
{
	virtual ~IListener() = default;
	virtual void onTcpConnected(bool c) {}
	virtual void onTcpDisconnected() {}
	virtual void onTcpTimeout() {}
};

//
// async TCP client sink
//

class Sink : private ISink
{
public:
	Sink();
	~Sink();

	ISink &sink() { return *this; }
	void sink(ISink &destination);

	void observe(IListener &l);

	void connect(const Sock::IP &p, unsigned int port);

	void writeSink(const char *source, size_t len);
	void process();

private:
	ISink *Destination = nullptr;
	IListener *Listener = nullptr;
	std::unique_ptr<Sock::Socket> MySocket;
	std::vector<char> InputBuffer;
	bool Connecting;
};

}}}}}
