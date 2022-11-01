project(MOZO)
cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

find_library(BENC REQUIRED)

add_subdirectory(src)