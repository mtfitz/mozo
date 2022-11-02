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
    out.precision(LEN_PEER_ID);
    out << std::fixed << r;
    std::string out_str = out.str();
    assert(out_str.length() == LEN_PEER_ID);
    return out_str;
}

TrackerManager::TrackerManager(Host host) : host(host), peer_id(generate_peer_id()) {}
TrackerManager::TrackerManager(string url, std::string port) : host{url, port}, peer_id(generate_peer_id()) {}

std::optional<std::vector<uint8_t>> TrackerManager::get(vector<uint8_t>& info_hash)
{
    // create connection to tracker
    auto conn_res = tcp_connect(this->host);
    if (!conn_res) {
        logger(to_string(conn_res.status), LogType::ERROR);
        return {};
    }
    Connection<TCP>&& conn = conn_res.get();

    // send info hash
    if (conn.send(info_hash)) return {};
    auto bytes_res = conn.recv(1);
    vector<uint8_t> bytes_got = bytes_res.get();
    return bytes_got;
}