#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <mutex>
#include <optional>
#include <random>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include <benc.hpp>
#include <netdb.h>
#include <openssl/sha.h>
#include <unistd.h>

#include "logger.hpp"
#include "trackermanager.hpp"

using namespace std;

struct UrlParsed {
    string domain;
    unsigned short port;
    string resource;
};

struct ConnectRequest {
    const uint8_t connection_id[8] = {0x00, 0x00, 0x04, 0x17, 0x27, 0x10, 0x19, 0x80};
    const uint32_t action = 0;
    uint32_t transaction_id;

    ConnectRequest(uint32_t tid) : transaction_id(tid) {}
};

struct ConnectResponse {
    uint32_t action;
    uint32_t transaction_id;
    uint64_t connection_id;
};

struct AnnounceRequestI4 {
    uint64_t connection_id[8];
    const uint32_t action = 1;
    uint32_t transaction_id;
    uint8_t info_hash[20];
    uint8_t peer_id[20];
    uint64_t downloaded;
    uint64_t left;
    uint64_t uploaded;
    uint32_t event;
    uint32_t ip_address; // set to 0, both IPv4 and IPv6
    uint32_t key;
    uint32_t num_want;
    uint16_t port;
};

uint64_t TrackerManager::get_connection_id()
{
    if (chrono::system_clock::now() - this->last_connect_time >= this->connection_id_expire) {
        this->connect();
    }
    return this->connection_id;
}

//TrackerManager::TrackerManager(string url, uint16_t port) : url(url), port(port), sd(-1), connection_id(MAGIC_NUMBER), rng(chrono::system_clock::now().time_since_epoch().count()) {}

TrackerManager::~TrackerManager()
{
    close(sd);
}

bool TrackerManager::init()
{
    if (sd >= 0)
        return true;

    //addrinfo hints;
    string port_str(to_string(port));
    addrinfo* address_info;
    if (getaddrinfo(url.c_str(), port_str.c_str(), nullptr, &address_info) != 0) {
        logger("Could not resolve hostname: " + url + ":" + to_string(port), ERROR);
        return false;
    }

    // prepare server address and port
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    sockaddr_in* ip = (sockaddr_in*) address_info->ai_addr;
    server_addr.sin_family = AF_INET;
    //server_addr.sin_addr.s_addr = inet_addr(inet_ntoa(* (struct in_addr*) address_info->ai_addr));
    server_addr.sin_addr = ip->sin_addr;
    server_addr.sin_port = htons(port);

    // dummy client address and port
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    client_addr.sin_family = AF_INET;
    client_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    client_addr.sin_port = htons(0);

    // create socket
    // INET, UDP, 0
    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
        logger("Could not create socket.");
        return false;
    }

    // bind socket to client
    if (bind(sd, (struct sockaddr *) &client_addr, sizeof(client_addr)) < 0) {
        logger("Could not bind socket.");
        return false;
    }

    this->sd = sd;
    this->server_addr = server_addr;
    this->client_addr = client_addr;

    freeaddrinfo(address_info);

    return true;
}

bool TrackerManager::connect()
{
    if (sd == -1) {
        this->init();
    }

    /* BEGIN STATIC DATA LOCK */
    static mutex gethost_mutex;
    gethost_mutex.lock();

    // this initially points to a static buffer, so we need to lock, otherwise we risk an overwrite by another thread
    string ip_str = inet_ntoa(this->server_addr.sin_addr);

    gethost_mutex.unlock();
    /* END STATIC DATA LOCK */

    logger("Connecting to " + ip_str + ":" + to_string(this->port) + "...");

    // send message
    uint32_t transaction_id = rng();
    ConnectRequest req(htonl(transaction_id)); 
    if (sendto(this->sd, &req, sizeof(req), 0, (struct sockaddr*) &this->server_addr, sizeof(this->server_addr)) < 0) {
        logger("Could not send message to server", LogType::ERROR);
        return false;
    }

    // prepare to receive message
    size_t recv_bytes = 0;
    socklen_t server_addr_res_len = sizeof(this->server_addr);
    char connect_response_buf[16];

    
    auto start_time = chrono::system_clock::now();

    // receive message (timeout loop)
    while(chrono::system_clock::now() - start_time < this->connect_timeout) {
        int recv_status = recvfrom(this->sd, connect_response_buf, sizeof(connect_response_buf), MSG_DONTWAIT,
            (struct sockaddr*) &this->server_addr, &server_addr_res_len);
        if (recv_status > 0)
            recv_bytes += recv_status;
        if (recv_bytes >= 16)
            break;
    }
    if (recv_bytes < 16) {
        logger("Connection timed out.", LogType::ERROR);
        return false;
    }
    //logger("Received " + to_string(recv_bytes) + " bytes.");

    // format response
    ConnectResponse res;
    res.action = ntohl(* (uint32_t*) &connect_response_buf[0]);
    res.transaction_id = ntohl(* (uint32_t*) &connect_response_buf[4]);
    res.connection_id = ntohl(* (uint32_t*) &connect_response_buf[8]);

    if (res.transaction_id != transaction_id) {
        logger("Incorrect transaction ID returned.", LogType::ERROR);
        return false;
    }

    if (res.action != 0) {
        logger("Incorrect action returned.", LogType::ERROR);
        return false;
    }

    logger("Transaction ID: " + to_string(transaction_id));

    this->connection_id = res.connection_id;
    this->last_connect_time = chrono::system_clock::now();

    return true;
}

optional<string> TrackerManager::announce(vector<benc::byte_t>& src)
{
    auto benc_result = benc::decode(src).value();
    auto benc_top_dict = benc_result.get_dict();
    string search_term = "info";
    vector<benc::byte_t> search_term_bytes(search_term.begin(), search_term.end());
    auto benc_info = benc_top_dict[search_term_bytes];
    auto new_benc = benc::BencTree(benc_info);
    auto benc_info_bytes = benc::encode(new_benc).value();
    
    unsigned char sha_buf[20];
    if(!SHA1(benc_info_bytes.data(), benc_info_bytes.size(), sha_buf)) {
        logger("Could not compute hash of info data", LogType::ERROR);
    }

    return {};
}