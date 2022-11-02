#pragma once

#include <optional>

enum Status {
    OK,
    ERR_GENERAL,
    ERR_CONNECT
};

template<typename T>
struct Result {
    Status status;
    std::optional<T> data;

    operator bool() const
    {
        return this->status == Status::OK;
    }

    T&& get()
    {
        return std::move(data).value();
    }
};