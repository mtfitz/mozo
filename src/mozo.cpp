#define CPPHTTPLIB_OPENSSL_SUPPORT

#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <benc.hpp>
#include <httplib/httplib.h>

#include "mozo.hpp"

using namespace std;

vector<pair<string, string>> parse_environ()
{
    vector<pair<string, string>> environ_vars;
    size_t i = 0;
    while (environ[i]) {
        string this_var = environ[i];
        string key = strtok(this_var.data(), "=");
        string val = strtok(nullptr, "");
        //cout << key << "=" << val << endl;
        environ_vars.push_back({key, val});
        i++;
    }
    return environ_vars;
}

int main(int argc, char** argv)
{
    // prepare input file stream
    vector<benc::byte_t> src;
    ifstream torrent_file("linuxmint.torrent", std::ifstream::binary | std::ifstream::ate);
    if (!torrent_file) {
        logger("Invalid input file", ERROR);
        exit(0);
    }
    // get file size (should be OK for binary files)
    size_t file_size = (size_t) torrent_file.tellg();
    src.resize(file_size);
    torrent_file.seekg(0);
    torrent_file.read((char *) src.data(), file_size);

    TrackerManager tm("tracker.opentrackr.org", 1337);
    if (!tm.connect()) {
        logger("Terminating abnormally...");
        exit(0);
    }

    // send success message
    // TO MAKE THIS WORK, set environment variables as such:
    // TWILIO_ACCOUNT_SID = [Twilio account SID]
    // TWILIO_AUTH_TOKEN = [Twilio auth token, keep this safe!]
    // TWILIO_PHONE_NUMBER = [Twilio-given phone number, format with country code and optional '+', e.g. +15555555555]
    auto env = parse_environ();
    auto account_sid_iter = find_if(env.begin(), env.end(), [](pair<string, string> p){ return p.first == "TWILIO_ACCOUNT_SID"; });
    auto auth_token_iter = find_if(env.begin(), env.end(), [](pair<string, string> p){ return p.first == "TWILIO_AUTH_TOKEN"; });
    auto phone_number_iter = find_if(env.begin(), env.end(), [](pair<string, string> p){ return p.first == "TWILIO_PHONE_NUMBER"; });
    
    if (argc == 2 &&
        account_sid_iter != env.end() &&
        auth_token_iter != env.end()  &&
        phone_number_iter != env.end()) {

        string base_url = "https://api.twilio.com";
        string account_url = "/2010-04-01/Accounts/" + account_sid_iter->second + "/Messages.json";
        httplib::Client alert(base_url);
        alert.set_basic_auth(account_sid_iter->second.c_str(), auth_token_iter->second.c_str());

        string phone_number_from = phone_number_iter->second;
        if (phone_number_from.length() > 0 && phone_number_from[0] == '+') {
            phone_number_from.erase(0, 1);
        }

        string phone_number_to = argv[1];
        if (phone_number_to.length() > 0 && phone_number_to[0] == '+') {
            phone_number_to.erase(0, 1);
        }

        // send the message!
        string post_msg = "Body=MOZO%20connected&From=%2B" + phone_number_from + "&To=%2B" + phone_number_to;
        auto res = alert.Post(account_url.c_str(), post_msg.c_str(), "application/x-www-form-urlencoded");

        logger("Sent alert, got response " + to_string(res->status));
        if (res->status != 201) {
            logger("Twilio: " + res->body, ERROR);
        }
    }

    tm.announce(src);

    return 0;
}