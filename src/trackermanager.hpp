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
    Host host;
    std::string peer_id;

public:
    TrackerManager(std::string url, uint16_t port);

    std::optional<std::vector<uint8_t>> get(vector<uint8_t>& info_hash);

    //std::optional<std::string> announce(std::vector<benc::byte_t>& src);
};