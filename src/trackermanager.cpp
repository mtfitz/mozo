#include <chrono>
#include <iostream>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#include <benc/benc.hpp>
#include <openssl/sha.h>

#include "logger.hpp"
#include "net.hpp"
#include "trackermanager.hpp"

using namespace std;

std::string generate_peer_id()
{
    static std::mt19937_64 rng(std::time(nullptr));
    
    uint64_t r = rng();
    std::ostringstream out;
    out.precision(20);
    out << std::fixed << r;
    std::string out_str = out.str();
    assert(out_str.length() == 20);
    return out_str;
}

TrackerManager::TrackerManager(string url, uint16_t port) : url(url), port(port), peer_id(generate_peer_id()) {}

std::optional<std::vector<uint8_t>> TrackerManager::get(vector<uint8_t>& info_hash)
{
    int sd = tcp_connect(host);
    if (sd == -1) return {};
    if (tcp_send(sd, info_hash)) return {};
    vector<uint8_t> bytes_got = tcp_recv(sd);
    tcp_disconnect(sd);
    return bytes_got;
}