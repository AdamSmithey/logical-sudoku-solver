cmake_minimum_required(VERSION 3.27)
project(LogicalSudokuSolver)

set(CMAKE_CXX_STANDARD 23)

include_directories(.)

add_executable(LogicalSudokuSolver
        main.cpp
        solver.cpp
        solver.h)
        