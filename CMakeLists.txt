cmake_minimum_required(VERSION 3.29)
project(cpp-playground VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(todo-list)
add_subdirectory(tic-tac-toe)
add_subdirectory(chip-8)