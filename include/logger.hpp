#pragma once

#include <string>

enum LogType {
    NONE,
    WARNING,
    ERROR,
};

void logger(std::string msg, LogType log_type = NONE);