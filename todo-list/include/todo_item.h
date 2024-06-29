#pragma once
#include <string>

struct ToDoItem {
  int id;
  std::string title;
  std::string description;
  bool completed;

  ToDoItem() : id(0), title(""), description(""), completed(false) {}

  ToDoItem(int id, std::string title, std::string description, bool completed)
    : id(id), title(title), description(description), completed(completed) {}
};
