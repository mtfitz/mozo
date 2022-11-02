#pragma once

#define htonll(x) ((1 == htonl(1)) ? (x) : ((uint64_t)htonl((x)&0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) ((1 == ntohl(1)) ? (x) : ((uint64_t)ntohl((x)&0xFFFFFFFF) << 32) | ntohl((x) >> 32))

#include <boost/asio.hpp>
#include <string>
#include <unistd.h>

#include "util.hpp"

struct Host
{
    std::string address;
    std::string port;
};

struct UrlParsed
{
    Host host;
    std::string resource;
};

enum ConnectionType
{
    TCP,
    UDP
};

template <ConnectionType T> struct Connection;

Result<Connection<TCP>> tcp_connect(Host& host);
void tcp_disconnect(Connection<TCP>& conn);
bool tcp_send(Connection<TCP>& conn, std::vector<uint8_t>& msg);
Result<std::vector<uint8_t>> tcp_recv(Connection<TCP>& conn, size_t n);

Result<Connection<UDP>> udp_connect(Host& host);
void udp_disconnect(Connection<UDP>& conn);
bool udp_send(Connection<UDP>& conn, std::vector<uint8_t>& msg);
Result<std::vector<uint8_t>> udp_recv(Connection<UDP>& conn, size_t n);

template <ConnectionType T>
struct Connection
{
    /*ConnectionType conn_type;
    //template <typename U>
    //boost::asio::basic_stream_socket<U> socket;

    Connection() = delete;
    //Connection(boost::asio::basic_stream_socket &socket, ConnectionType conn_type = TCP) = delete;
    //Connection(boost::asio::basic_stream_socket &socket) = delete;

    ~Connection() = delete;

    bool send(std::vector<uint8_t>& msg) = delete;

    Result<std::vector<uint8_t>> recv(size_t n) = delete;*/

    //Connection(boost::asio::basic_stream_socket &socket, ConnectionType conn_type = TCP)
        //: conn_type(conn_type), socket(socket) {}

    
    /*{
        if constexpr (std::is_same_v<T, ConnectionType::TCP>) {
            tcp_disconnect(*this);
        }
        if constexpr (std::is_same_v<T, ConnectionType::UDP>) {
            udp_disconnect(*this);
        }
    }*/
    
    template <typename U>
    Connection(boost::asio::basic_stream_socket<U> &socket) = delete;

};

template <>
struct Connection<TCP>
{
    boost::asio::ip::tcp::socket socket;

    Connection(/*boost::asio::basic_stream_socket*/ boost::asio::ip::tcp::socket &socket)
        : socket(std::move(socket)) {}
    
    ~Connection()
    {
        tcp_disconnect(*this);
    }

    bool send(std::vector<uint8_t>& msg)
    {
        return tcp_send(*this, msg);
    }

    Result<std::vector<uint8_t>> recv(size_t n)
    {
        return tcp_recv(*this, n);
    }
};