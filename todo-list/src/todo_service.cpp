#include "todo_service.h"

void ToDoService::add(const ToDoItem &item) {
  repository.add(item);
}

int ToDoService::getNextId() {
  auto items = repository.getAll();
  const auto max = std::max_element(items.begin(), items.end(), [](const ToDoItem &a, const ToDoItem &b) {
    return a.id < b.id;
    });

  return max == items.end() ? 1 : max->id + 1;
}

void ToDoService::remove(int id) {
  repository.remove(id);
}

void ToDoService::complete(int id) {
  auto items = repository.getAll();
  auto it = std::find_if(items.begin(), items.end(), [id](const ToDoItem &item) {
    return item.id == id;
    });

  if (it != items.end()) {
    ToDoItem item = *it;
    item.completed = true;
    repository.update(item);
  }
}
