#include "netsocket.hpp"

#include "../platform.hpp"
#include <istream>
#include <ostream>

#ifdef MLIB_PLATFORM_WIN32
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#endif

namespace mlib::net::sock
{

#ifdef MLIB_PLATFORM_WIN32
	struct Winsock
	{
		Winsock()
		{
			WSADATA wsa;
			WSAStartup(MAKEWORD(2, 2), &wsa);
		}
		~Winsock()
		{
			WSACleanup();
		}
	};

	static Winsock WinsockInitializer;

	static void closeSocket(SocketHandle handle)
	{
		closesocket(handle);
	}

	static auto lastError()
	{
		return WSAGetLastError();
	}
	static bool wouldBlock()
	{
		return WSAGetLastError() == WSAEWOULDBLOCK || WSAGetLastError() == WSAEALREADY;
	}
	static bool disconnected()
	{
		return WSAGetLastError() == WSAECONNRESET || WSAECONNABORTED;
	}
	static bool refused()
	{
		return WSAGetLastError() == WSAECONNREFUSED;
	}
	static bool timeout()
	{
		return WSAGetLastError() == WSAETIMEDOUT;
	}
	[[noreturn]] static void throwError(const char *msg)
	{
		auto err = WSAGetLastError();
		throw PreciseError(msg, err);
	}
#else
	static void closeSocket(SocketHandle handle)
	{
		close(handle);
	}

	static auto lastError()
	{
		return errno;
	}

	static bool wouldBlock()
	{
		return errno == EWOULDBLOCK || errno == EAGAIN || errno == EINPROGRESS;
	}
	static bool disconnected()
	{
		return errno == ECONNRESET;
	}
	static bool refused()
	{
		return errno == ECONNREFUSED;
	}
	static bool timeout()
	{
		return errno == ETIMEDOUT;
	}
	[[noreturn]] static void throwError(const char *msg)
	{
		throw PreciseError(msg, errno);
	}
#endif

static void populateError(ErrorCode *c)
{
	if(c)
		*c = lastError();
}


//
// ip address
//

bool IP::operator==(const IP &rhs) const
{
    if(Type == rhs.Type)
    {
        if(Type == Version::v4)
            return memcmp(v4.Address, rhs.v4.Address, sizeof(v4.Address)) == 0;
        else
            return memcmp(v6.Address, rhs.v6.Address, sizeof(v6.Address)) == 0;
    }
    return false;
}

//
// host resolution
//

std::vector<IP> resolveHost(const std::string &name)
{
    std::vector<IP> ips;

    addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;

    addrinfo *result = nullptr;
	if(getaddrinfo(name.c_str(), nullptr, &hints, &result) == 0)
	{
		for(auto ptr = result; ptr != nullptr; ptr = ptr->ai_next)
		{
			IP ip;
			if(ptr->ai_family == AF_INET)
			{
				ip.Type = IP::Version::v4;

				const auto *in = reinterpret_cast<sockaddr_in*>(result->ai_addr);
				memcpy(ip.v4.Address, &in->sin_addr.s_addr, sizeof(ip.v4.Address));
				ips.push_back(ip);
			}
			else if(ptr->ai_family == AF_INET6)
			{
				ip.Type = IP::Version::v6;

				const auto *in6 = reinterpret_cast<sockaddr_in6*>(result->ai_addr);
				memcpy(ip.v6.Address, in6->sin6_addr.s6_addr, sizeof(ip.v6.Address));
				ips.push_back(ip);
			}
		}

		freeaddrinfo(result); //TODO: RAII
	}
	else
		throwError(".resolvehost");

    return ips;
}

std::optional<IPv4> parseIPv4(const std::string &str)
{
    in_addr addr;
    if(inet_pton(AF_INET, str.c_str(), &addr) > 0)
    {
        IPv4 ip;
        memcpy(ip.Address, &addr.s_addr, sizeof(ip.Address));
        return ip;
    }
    else
        return std::nullopt;
}

std::optional<IPv6> parseIPv6(const std::string & str)
{
    in6_addr addr;
    if(inet_pton(AF_INET6, str.c_str(), &addr) > 0)
    {
        IPv6 ip;
        memcpy(ip.Address, addr.s6_addr, sizeof(ip.Address));
        return ip;
    }
    return std::nullopt;
}

std::vector<IP> examineHost(const std::string &str)
{
    auto ip6 = parseIPv6(str);
    if(ip6)
        return { *ip6 };

    auto ip4 = parseIPv4(str);
    if(ip4)
        return { *ip4 };

    return resolveHost(str);
}

const IP *preferIPv6(const std::vector<IP> &ips)
{
    const IP *found4 = nullptr;
    for(const auto &ip : ips)
    {
        if(ip.Type == IP::Version::v6)
            return &ip;
        else if(ip.Type == IP::Version::v4)
            found4 = &ip;
    }

    return found4;
}

std::string stringIPv4(const IPv4 &ip)
{
    char buf[32];
    in_addr addr;

    memcpy(&addr.s_addr, ip.Address, sizeof(in_addr));
    if(inet_ntop(AF_INET, &addr, buf, sizeof(buf)) > 0)
        return buf;

    throwError(".inet_ntop");
}

std::string stringIPv6(const IPv6 &ip)
{
    char buf[64];
    in6_addr addr;

    memcpy(&addr.s6_addr, ip.Address, sizeof(in6_addr));
    if(inet_ntop(AF_INET6, &addr, buf, sizeof(buf)) > 0)
        return buf;

    throwError(".inet_ntop");
}

std::string stringIP(const IP &ip)
{
    return ip.Type == IP::Version::v4 ? stringIPv4(ip.v4) : stringIPv6(ip.v6);
}

//
// utilities
//

static socklen_t makeAddress(sockaddr_storage &addr, const IP &ip, unsigned int port)
{
    if(ip.Type == IP::Version::v4)
    {
        auto *addr4 = reinterpret_cast<sockaddr_in*>(&addr);
        memset(addr4, 0, sizeof(*addr4));

        addr4->sin_family = AF_INET;
        addr4->sin_port = htons(static_cast<unsigned short>(port));
        memcpy(&addr4->sin_addr.s_addr, ip.v4.Address, sizeof(addr4->sin_addr));
        return sizeof(*addr4);
    }
    else
    {
        auto *addr6 = reinterpret_cast<sockaddr_in6*>(&addr);
        memset(addr6, 0, sizeof(*addr6));

        addr6->sin6_family = AF_INET6;
        addr6->sin6_port = htons(static_cast<unsigned short>(port));
        memcpy(&addr6->sin6_addr.s6_addr, ip.v6.Address, sizeof(addr6->sin6_addr.s6_addr));
        return sizeof(*addr6);
    }
}

static void extractAddress(const sockaddr_storage &addr, IP &ip, unsigned int &port)
{
    if(addr.ss_family == AF_INET)
    {
        auto *addr4 = reinterpret_cast<const sockaddr_in*>(&addr);
        memcpy(ip.v4.Address, &addr4->sin_addr.s_addr, sizeof(in_addr));
        port = static_cast<unsigned int>(ntohs(addr4->sin_port));

        ip.Type = IP::Version::v4;
    }
    else
    {
        auto *addr6 = reinterpret_cast<const sockaddr_in6*>(&addr);
        memcpy(ip.v6.Address, &addr6->sin6_addr.s6_addr, sizeof(in6_addr));
        port = static_cast<unsigned int>(ntohs(addr6->sin6_port));

        ip.Type = IP::Version::v6;
    }
}

//
// generic socket wrapper
//

Socket::Socket() : Handle(InvalidSocket), CurrentState(State::Disconnected)
{
}
Socket::Socket(SocketHandle sock, IP::Version ipver, State state)
            : Handle(sock), IpVersion(ipver), CurrentState(state)
{
}
Socket::Socket(IP::Version ipver, Proto proto) : IpVersion(ipver)
{
    newSocket(ipver == IP::Version::v4 ? AF_INET : AF_INET6, proto == Proto::Tcp ? SOCK_STREAM : SOCK_DGRAM, 0);
}
void Socket::newSocket(int af, int type, int proto)
{
    if(Handle != InvalidSocket)
        closeSocket(Handle);

    CurrentState = State::Disconnected;

    Handle = socket(af, type, proto);

    if(Handle == InvalidSocket)
        throwError(".createsocket");

#ifdef MLIB_PLATFORM_WIN32 //disable ICMP error when remote udp port is not opened
    if(type == SOCK_DGRAM)
    {
        BOOL newBehavior = FALSE;
        DWORD ret = 0;
        WSAIoctl(Handle, _WSAIOW(IOC_VENDOR, 12), &newBehavior, sizeof(newBehavior), nullptr, 0, &ret, nullptr, nullptr);
    }

	if(type == SOCK_STREAM)
	{
		int yes = 1; //disable buffering
		setsockopt(Handle, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&yes), sizeof(yes));

		DWORD t = 15000;
		setsockopt(Handle, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&t), sizeof(t));
		setsockopt(Handle, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&t), sizeof(t));
	}
#else
	if(type == SOCK_STREAM)
	{
		timeval t;
		t.tv_usec = 0;
		t.tv_sec = 15;
		setsockopt(Handle, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&t), sizeof(t));
		setsockopt(Handle, SOL_SOCKET, SO_SNDTIMEO, reinterpret_cast<const char*>(&t), sizeof(t));
	}
#endif
}
Socket::~Socket()
{
    if(Handle != InvalidSocket)
        closeSocket(Handle);
}

Socket::Socket(Socket &&rhs) noexcept
{
    Handle = rhs.Handle;
    rhs.Handle = InvalidSocket;
    CurrentState = rhs.CurrentState;
    rhs.CurrentState = State::Disconnected;
}
Socket &Socket::operator=(Socket &&rhs) noexcept
{
    Handle = rhs.Handle;
    rhs.Handle = InvalidSocket;
    CurrentState = rhs.CurrentState;
    rhs.CurrentState = State::Disconnected;
    return *this;
}

auto Socket::state() const -> State
{
    if(CurrentState == State::Connecting)
    {
        fd_set writefds;
        FD_ZERO(&writefds);
        FD_SET(Handle, &writefds);

        timeval tim = { 0, 0 };
        select(2, nullptr, &writefds, nullptr, &tim);

        if(FD_ISSET(Handle, &writefds))
        {
            int optval = -1;
            socklen_t optlen = sizeof(optval);

            getsockopt(Handle, SOL_SOCKET, SO_ERROR, reinterpret_cast<char*>(&optval), &optlen);

            CurrentState = optval == 0 ? State::Ready : State::Disconnected;
        }
    }

    return CurrentState;
}

auto Socket::connect(const IP &dest, unsigned int port) -> ConnectStatus
{
    sockaddr_storage addr;
    auto addrlen = makeAddress(addr, dest, port);

    auto ret = ::connect(Handle, reinterpret_cast<sockaddr*>(&addr), addrlen);
    if(ret < 0)
    {
        if(wouldBlock())
        {
            CurrentState = State::Connecting;
			return ConnectStatus::InProgress;
        }

        CurrentState = State::Disconnected;

		if(refused())
			return ConnectStatus::Refused;
		else if(timeout())
			return ConnectStatus::Timeout;
		else
			throwError(".connect");
    }

	CurrentState = State::Ready;
    return ConnectStatus::Success;
}

auto Socket::send(const char *src, size_t len) -> SendStatus
{
    if(::send(Handle, src, len, 
#ifdef MLIB_PLATFORM_WIN32
		0
#else
		MSG_NOSIGNAL 
#endif
	) < 0)
    {
        if(wouldBlock())
            return SendStatus::Success;

		CurrentState = State::Disconnected;

		if(disconnected())
			return SendStatus::Disconnected;
		else if(timeout())
			return SendStatus::Timeout;
		else
			throwError(".send");
    }

    return SendStatus::Success;
}

auto Socket::receive(char *dest, size_t maxlen, size_t &actuallen) -> ReceiveStatus
{
    auto ret = recv(Handle, dest, maxlen, 0);
    if(ret <= 0)
    {
        if(wouldBlock())
            return ReceiveStatus::NoData;

		CurrentState = State::Disconnected;

		if(disconnected())
			return ReceiveStatus::Disconnected;
		else if(timeout())
			return ReceiveStatus::Timeout;
		else
			throwError(".receive");
    }

    actuallen = static_cast<size_t>(ret);

    return ReceiveStatus::Available;
}

size_t Socket::available() const
{
	u_long bytesAvail = 0;
#ifdef MLIB_PLATFORM_WIN32
	ioctlsocket(Handle, FIONREAD, &bytesAvail);
#else
	ioctl(Handle, FIONREAD, &bytesAvail);
#endif
	return static_cast<size_t>(bytesAvail);
}

void Socket::block(bool b)
{
#ifdef MLIB_PLATFORM_WIN32
    u_long blocking = b ? 0 : 1;
    ioctlsocket(Handle, FIONBIO, &blocking);
#else
    const auto status = fcntl(Handle, F_GETFL);
    fcntl(Handle, F_SETFL, b ? (status & ~O_NONBLOCK) : (status | O_NONBLOCK));
#endif
}

bool Socket::bind(unsigned int port)
{
    sockaddr_in addr4;
    sockaddr_in6 addr6;
    socklen_t addrlen;
    sockaddr *addr;

    if(IpVersion == IP::Version::v4)
    {
        memset(&addr4, 0, sizeof(addr4));

        addr4.sin_family = AF_INET;
        addr4.sin_port = htons(static_cast<unsigned short>(port));
        addr4.sin_addr.s_addr = INADDR_ANY;
        addr = reinterpret_cast<sockaddr*>(&addr4);
        addrlen = sizeof(addr4);
    }
    else
    {
        memset(&addr6, 0, sizeof(addr6));

        addr6.sin6_family = AF_INET6;
        addr6.sin6_port = htons(static_cast<unsigned short>(port));
        memcpy(&addr6.sin6_addr.s6_addr, &in6addr_any, sizeof(in6addr_any));
        addr = reinterpret_cast<sockaddr*>(&addr6);
        addrlen = sizeof(addr6);
    }
    return ::bind(Handle, addr, addrlen) == 0;
}

void Socket::listen()
{
    if(::listen(Handle, 0) < 0)
        throwError(".listen");

    CurrentState = State::Listening;
}

void Socket::sendto(const char *src, size_t len, const IP &dest, unsigned int port)
{
    sockaddr_storage addr;
    auto addrlen = makeAddress(addr, dest, port);

    if(::sendto(Handle, src, static_cast<int>(len), 0, reinterpret_cast<sockaddr*>(&addr), addrlen) < 0)
        throwError(".sendto");
}

bool Socket::recvfrom(char *dest, size_t maxlen, size_t &reclen, IP &src, unsigned int &port)
{
    sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);

    auto ret = ::recvfrom(Handle, dest, static_cast<size_t>(maxlen), 0, reinterpret_cast<sockaddr*>(&addr), &addrlen);

    if(ret < 0)
    {
        if(wouldBlock())
            return false;

        throwError(".recvfrom");
    }

    extractAddress(addr, src, port);
    reclen = static_cast<size_t>(ret);

    return true;
}

bool Socket::accept(Socket &sock, IP &src, unsigned int &port)
{
    sockaddr_storage addr;
    socklen_t addrlen = sizeof(addr);

    auto ret = ::accept(Handle, reinterpret_cast<sockaddr*>(&addr), &addrlen);
    if(ret == InvalidSocket)
    {
        if(wouldBlock())
            return false;

        throwError(".accept");
    }

    extractAddress(addr, src, port);
    sock = Socket(ret, src.Type, State::Ready);

    return true;
}

}