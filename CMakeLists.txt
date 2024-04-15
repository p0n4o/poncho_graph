cmake_minimum_required(VERSION 3.24)
project(laba3)

set(CMAKE_CXX_STANDARD 17)

add_executable(laba3
        source/main.cpp
        graph/graph.h
        graph/graph.hpp
        matrix/matrix.h
        matrix/matrix.hpp
        source/pseudonyms.h workdir/generate_matrix.hpp)
