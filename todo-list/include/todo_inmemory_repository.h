#pragma once
#include "todo_repository.h"

class ToDoInMemoryRepository : public ToDoRepository {
private:
  std::vector<ToDoItem> items;

public:
  ToDoInMemoryRepository() : items{} {}

  void add(const ToDoItem &item) override;
  void remove(int id) override;
  void update(const ToDoItem &item) override;
  std::vector<ToDoItem> getAll() const override;
};
