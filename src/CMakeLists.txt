add_executable(mozo mozo.cpp trackermanager.cpp net.cpp logger.cpp)
target_link_libraries(mozo BENC)
target_include_directories(mozo PUBLIC benc/include)