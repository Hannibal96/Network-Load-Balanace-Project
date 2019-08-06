cmake_minimum_required(VERSION 3.12)
project(NetworkSimulator)

set(CMAKE_CXX_STANDARD 14)

add_executable(NetworkSimulator
        Job.cpp
        main.cpp defs.h Server.cpp Server.h Job.cpp Job.h)
