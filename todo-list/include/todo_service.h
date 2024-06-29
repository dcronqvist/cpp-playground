#pragma once
#include "todo_repository.h"

class ToDoService {
private:
  ToDoRepository &repository;

public:
  ToDoService(ToDoRepository &repository) : repository(repository) {}

  void add(const ToDoItem &item);
  int getNextId();
  void remove(int id);
  void complete(int id);
};
