project(todo-list VERSION 1.0.0 LANGUAGES CXX)

add_executable(todo-list
    src/main.cpp
    src/todo_inmemory_repository.cpp
    src/todo_jsonfile_repository.cpp
    src/todo_service.cpp
)

target_include_directories(todo-list PRIVATE include)

add_subdirectory(third-party/nlohmann)