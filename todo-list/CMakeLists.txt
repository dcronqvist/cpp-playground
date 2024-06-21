project(todo-list VERSION 1.0.0 LANGUAGES CXX)

add_executable(todo-list
    src/main.cpp
)

target_include_directories(todo-list PRIVATE include)