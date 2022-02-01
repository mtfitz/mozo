#pragma once

#include <arpa/inet.h>
#include <benc.hpp>
#include <chrono>
#include <netdb.h>
#include <optional>
#include <random>
#include <string>
#include <vector>

#include "net.hpp"

#define CONNECT_TIMEOUT 15
#define CONNECTION_ID_EXPIRE 60

class TrackerManager {
private:
    std::string url;
    uint16_t port;

    int sd;
    sockaddr_in server_addr;
    sockaddr_in client_addr;

    uint64_t connection_id;
    std::chrono::system_clock::time_point last_connect_time;

    std::mt19937 rng;

    static constexpr std::chrono::duration<int> connect_timeout{CONNECT_TIMEOUT};
    static constexpr std::chrono::duration<int> connection_id_expire{CONNECTION_ID_EXPIRE};
    static constexpr uint64_t MAGIC_NUMBER = 0x41727101980;

    uint64_t get_connection_id();

public:
    TrackerManager(std::string url, uint16_t port) : url(url), port(port), sd(-1), 
        connection_id(MAGIC_NUMBER), rng(std::chrono::system_clock::now().time_since_epoch().count()) {}

    ~TrackerManager();

    bool init();
    bool connect();
    std::optional<std::string> announce(std::vector<benc::byte_t>& src);
};