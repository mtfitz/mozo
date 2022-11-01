#include "net.hpp"
#include "logger.hpp"
#include "util.hpp"

#include <boost/asio.hpp>
#include <optional>
#include <vector>

using namespace std;
using namespace boost::asio;
using ip::tcp;

using enum Status;

// class Connection {
// private:
//     int sd;

// public:
//     Connection();

//     bool init();
// };

// class UdpConnection : public Connection {
// private:

// public:
//     Connection() : sd(-1) {}

//     bool init()
//     {
//         // create socket
//         // INET, UDP, 0
//         int sd = socket(AF_INET, SOCK_DGRAM, 0);
//         if (sd < 0) {
//             logger("Could not create socket.");
//             return false;
//         }
//         this->sd = sd;


//     }
// };

// int tcp_connect(Host host)
// {
//     string port_str = to_string(host.port);

//     addrinfo hints;
//     addrinfo* res;
//     int sd;

//     memset(&hints, 0, sizeof(hints));
//     hints.ai_family = AF_UNSPEC;
//     hints.ai_socktype = SOCK_STREAM;
//     hints.ai_flags = 0;
//     hints.ai_protocol = 0;

//     if (getaddrinfo(host.address.c_str(), port_str.c_str(), &hints, &res)) {
//         return -1;
//     }

//     addrinfo* rp;
//     for (rp = res; rp != nullptr; rp = rp->ai_next) {
//         sd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
//         // if socket couldn't be opened (i.e. bad choice)
//         if (sd == -1) {
//             continue;
//         }
//         // if we can connect (i.e. good choice)
//         if (!connect(sd, rp->ai_addr, rp->ai_addrlen)) {
//             break;
//         }
//         close(sd);
//     }

//     freeaddrinfo(res);

//     // if no results worked
//     if (!rp) {
//         return -1;
//     }

//     return sd;
// }

Result<Connection<TCP>> tcp_connect(Host& host)
{
    string &addr = host.address;
    string &port = host.port;
    tcp::resolver::query query(addr, port);

    io_context io;
    tcp::socket socket(io);
    tcp::resolver resolver(io);
    auto res_iter(resolver.resolve(query));
    auto endpoint = res_iter->endpoint();

    socket.connect(endpoint);

    return {OK, Connection<TCP>(socket)};
}

void tcp_disconnect(Connection<TCP>& conn)
{
    try
    {
        conn.socket.close();
    }
    catch (std::exception &)
    {
    }
}

bool tcp_send(Connection<TCP>& conn, std::vector<uint8_t>& msg)
{
    conn.socket.send(msg);
}

optional<std::vector<uint8_t>> tcp_recv(int sd, size_t n)
{
    std::vector<uint8_t> b(n);

    size_t i = 0;
    while (i < n)
    {
        ssize_t j = recv(sd, b.data() + i, n - i, 0);
        if (j < 0)
            return {};
        i += j;
    }

    return b;
}

Result<Connection<UDP>> udp_connect(Host& host) { throw std::logic_error("udp_connect() is unimplemented"); }
void udp_disconnect(Connection<UDP>& conn) { throw std::logic_error("udp_disconnect() is unimplemented"); }
bool udp_send(Connection<UDP>& conn, std::vector<uint8_t>& msg) { throw std::logic_error("udp_send() is unimplemented"); }
Result<std::vector<uint8_t>> udp_recv(Connection<UDP>& conn, size_t n) { throw std::logic_error("udp_recv() is unimplemented"); }

// int open_socket()
// {
//     // parse address
//     string port_str(to_string(port));
//     addrinfo* address_info;
//     if (getaddrinfo(url.c_str(), port_str.c_str(), nullptr, &address_info) != 0) {
//         logger("Could not resolve hostname: " + url + ":" + to_string(port), ERROR);
//         return false;
//     }

//     // prepare server address and port
//     struct sockaddr_in server_addr;
//     memset(&server_addr, 0, sizeof(server_addr));
//     sockaddr_in* ip = (sockaddr_in*) address_info->ai_addr;
//     server_addr.sin_family = AF_INET;
//     //server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(* (struct in_addr*) address_info->ai_addr));
//     server_addr.sin_addr = ip->sin_addr;
//     server_addr.sin_port = htons(port);
// }

// int udp_write

// int close_socket()
// {

// }