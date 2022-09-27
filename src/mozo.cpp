#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>
#include <benc.hpp>

#include "mozo.hpp"

using namespace std;

vector<pair<string, string>> parse_env_vars()
{
    vector<pair<string, string>> env_vars;
    size_t i = 0;
    while (environ[i]) {
        string this_var = environ[i];
        string key = strtok(this_var.data(), "=");
        string val = strtok(nullptr, "");
        //cout << key << "=" << val << endl;
        env_vars.push_back({key, val});
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

    tm.announce(src);

    return 0;
}