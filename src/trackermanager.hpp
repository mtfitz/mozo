#pragma once

#include <arpa/inet.h>
#include <benc/benc.hpp>
#include <chrono>
#include <netdb.h>
#include <optional>
#include <random>
#include <string>
#include <vector>

#include "net.hpp"

#define CONNECT_TIMEOUT 15
#define CONNECTION_ID_EXPIRE 60
#define LEN_PEER_ID 20

class TrackerManager {
private:
    //Connection<TCP> conn;
    Host host;
    std::string peer_id;

public:
    TrackerManager(Host host);
    TrackerManager(std::string url, std::string port);

    std::optional<std::vector<uint8_t>> get(std::vector<uint8_t>& info_hash);

    //std::optional<std::string> announce(std::vector<benc::byte_t>& src);
};