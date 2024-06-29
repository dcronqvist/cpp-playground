#pragma once
#include <vector>

#include "todo_item.h"

class ToDoRepository {
public:
  virtual void add(const ToDoItem &item) = 0;
  virtual void remove(int id) = 0;
  virtual void update(const ToDoItem &item) = 0;
  virtual std::vector<ToDoItem> getAll() const = 0;
};
