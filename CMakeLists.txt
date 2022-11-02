project(MOZO)
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)

find_package(Boost REQUIRED)
find_package(OpenSSL REQUIRED)

add_subdirectory(benc)

add_subdirectory(src)