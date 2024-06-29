#pragma once
#include "todo_repository.h"

class ToDoJsonFileRepository : public ToDoRepository {
private:
  std::string filePath;

public:
  ToDoJsonFileRepository(const std::string &filePath) : filePath(filePath) {}

  void add(const ToDoItem &item) override;
  void remove(int id) override;
  void update(const ToDoItem &item) override;
  std::vector<ToDoItem> getAll() const override;
};
