cmake_minimum_required(VERSION 3.7)
project(eosdice)

set(CMAKE_CXX_STANDARD 11)

include_directories(.)

add_executable(eosdice
        eosdice.cpp
        eosdice.hpp
        eosio.token.hpp
        types.hpp
        utils.hpp)
