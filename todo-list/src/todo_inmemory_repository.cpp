#include "todo_inmemory_repository.h"

void ToDoInMemoryRepository::add(const ToDoItem &item) {
  items.push_back(item);
}

void ToDoInMemoryRepository::remove(int id) {
  items.erase(std::remove_if(items.begin(), items.end(), [id](const ToDoItem &item) {
    return item.id == id;
    }), items.end());
}

void ToDoInMemoryRepository::update(const ToDoItem &item) {
  auto it = std::find_if(items.begin(), items.end(), [item](const ToDoItem &i) {
    return i.id == item.id;
    });

  if (it != items.end()) {
    *it = item;
  }
}

std::vector<ToDoItem> ToDoInMemoryRepository::getAll() const {
  return items;
}
