#pragma once

enum Status {
    OK,
    ERR_GENERAL,
    ERR_CONNECT
};

template<typename T>
struct Result {
    Status status;
    T data;
};