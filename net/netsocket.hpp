#pragma once

#include <cstdint>
#include <cstddef>
#include <memory>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstring>
#include <optional>
#include <cstddef>

#include <mlib/platform.hpp>

namespace mlib::net::sock
{

//
// platform specific
//

#ifdef MLIB_PLATFORM_WIN32
using SocketHandle = uintptr_t;
static constexpr SocketHandle InvalidSocket = (SocketHandle)~0;
using ErrorCode = int;
#else
using SocketHandle = int;
static constexpr SocketHandle InvalidSocket = -1;
using ErrorCode = int;
#endif

//
// exceptions
//

struct Error : public std::runtime_error
{
    Error(const std::string &e) : runtime_error(".net.sock" + e) {}
};

struct PreciseError : public Error
{
	ErrorCode Code;
	PreciseError(const std::string &msg, ErrorCode c) : Error(msg + " [" + std::to_string(c) + "]"), Code(c) {}
};

//
// IP address, IPv4 and IPv6. The bytes are in network order.
//

struct IPv4
{
    uint8_t Address[4];
};

struct IPv6
{
    uint8_t Address[16];
};

struct IP
{
    enum class Version { v4, v6 };
    Version Type = Version::v4;

    union
    {
        IPv4 v4;
        IPv6 v6;
    };

    IP() = default;
    IP(const IPv4 &ip4) : Type(Version::v4), v4(ip4) {}
    IP(const IPv6 &ip6) : Type(Version::v6), v6(ip6) {}

    bool operator==(const IP &rhs) const;
};

//
// host resolution
//

std::vector<IP> resolveHost(const std::string &name);
std::optional<IPv4> parseIPv4(const std::string &str);
std::optional<IPv6> parseIPv6(const std::string &str);

//tries to parse as IPv6-string, then IPv4-string, then resolves the hostname
std::vector<IP> examineHost(const std::string &str);

//tries to find an IPv6-address in the list, then IPv4-address, then returns nullptr
const IP *preferIPv6(const std::vector<IP> &ips);

std::string stringIPv4(const IPv4 &ip);
std::string stringIPv6(const IPv6 &ip);
std::string stringIP(const IP &ip);

//
// generic socket wrapper
//

class Socket
{
public:
    enum class State { Disconnected, Ready, Listening, Connecting };
    State state() const;

    enum class Proto { Tcp, Udp };

    Socket();
    Socket(IP::Version ipver, Proto proto);
    Socket(SocketHandle sock, IP::Version ipver, State state);
    ~Socket();

    Socket(const Socket&) = delete;
    Socket &operator=(const Socket&) = delete;

    Socket(Socket &&rhs) noexcept;
    Socket &operator=(Socket &&rhs) noexcept;

    void block(bool b);
    bool bind(unsigned int portr);
    void listen();

    void sendto(const char *src, size_t len, const IP &dest, unsigned int port);
    bool recvfrom(char *dest, size_t maxlen, size_t &reclen, IP &src, unsigned int &port);
    bool accept(Socket &sock, IP &src, unsigned int &port);

    enum class ConnectStatus { Success, Refused, Timeout, InProgress};
    ConnectStatus connect(const IP &dest, unsigned int port);

    enum class SendStatus { Success, Disconnected, Timeout };
    SendStatus send(const char *src, size_t len);

    enum class ReceiveStatus { Available, NoData, Disconnected, Timeout };
    ReceiveStatus receive(char *dest, size_t maxlen, size_t &actuallen);

	size_t available() const;

private:
    IP::Version IpVersion;
    mutable SocketHandle Handle;
    mutable State CurrentState;

    void newSocket(int af, int type, int proto);
};

}