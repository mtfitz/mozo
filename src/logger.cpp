#include <iostream>
#include <fstream>

#include "logger.hpp"

using namespace std;

void logger(string msg, LogType log_type)
{
    ofstream log_file("log.txt", ios_base::app);

    string prefix;
    switch (log_type) {
    case LogType::WARNING:
        prefix = "[\033[1;33mWARN\033[0m] ";
        break;
    case LogType::ERROR:
        prefix = "[\033[1;31mERR\033[0m] ";
        break;
    default:
        break;
    }

    string output = prefix + msg;

    cout << output << endl;
    log_file << output << endl;
}