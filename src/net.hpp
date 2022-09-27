#pragma once

#define htonll(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) ((1==ntohl(1)) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))

#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string>
#include <unistd.h>

struct Host {
    std::string address;
    uint16_t port;
};

struct UrlParsed {
    Host host;
    std::string resource;
};

int tcp_connect(Host host);
void tcp_disconnect(int sd);
bool tcp_send(int sd, std::vector<uint8_t>& msg);
std::optional<std::vector<uint8_t>> tcp_recv(int sd, size_t n);