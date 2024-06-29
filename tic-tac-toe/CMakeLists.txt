project(tic-tac-toe VERSION 1.0.0 LANGUAGES CXX)

add_executable(tic-tac-toe
    src/main.cpp
    src/board.cpp
    src/game.cpp
)

target_include_directories(tic-tac-toe PRIVATE include)