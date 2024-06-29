#include <fstream>
#include "todo_jsonfile_repository.h"
#include "nlohmann/json.hpp"

namespace ns {
  void to_json(nlohmann::json &j, const ToDoItem &item) {
    j = nlohmann::json{
      {"id", item.id},
      {"title", item.title},
      {"description", item.description},
      {"completed", item.completed}
    };
  }

  void from_json(const nlohmann::json &j, ToDoItem &item) {
    j.at("id").get_to(item.id);
    j.at("title").get_to(item.title);
    j.at("description").get_to(item.description);
    j.at("completed").get_to(item.completed);
  }
}

std::vector<ToDoItem> readFileAsRepository(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    return {};
  }

  nlohmann::json jsonArray;
  file >> jsonArray;
  file.close();

  std::vector<ToDoItem> items;
  for (const auto &jItem : jsonArray) {
    ToDoItem item;
    ns::from_json(jItem, item); // Use the from_json function you've defined
    items.push_back(item);
  }

  return items;
}

void saveRepositoryToFile(const std::vector<ToDoItem> &items, const std::string &filePath) {
  // Create a json array
  nlohmann::json jsonArray = nlohmann::json::array();

  // Convert each ToDoItem to json and add it to the json array
  for (const auto &item : items) {
    nlohmann::json jItem;
    ns::to_json(jItem, item); // Use the to_json function you've defined
    jsonArray.push_back(jItem);
  }

  // Write the json array to file
  std::ofstream file(filePath);
  if (file.is_open()) {
    file << jsonArray.dump(4); // Pretty print with an indent of 4 spaces
    file.close();
  }
  else {
    // Handle error: unable to open file
  }
}

void ToDoJsonFileRepository::add(const ToDoItem &item) {
  auto repo = readFileAsRepository(filePath);
  repo.push_back(item);
  saveRepositoryToFile(repo, filePath);
}

void ToDoJsonFileRepository::remove(int id) {
  auto repo = readFileAsRepository(filePath);
  repo.erase(std::remove_if(repo.begin(), repo.end(), [id](const ToDoItem &item) {
    return item.id == id;
    }), repo.end());
  saveRepositoryToFile(repo, filePath);
}

void ToDoJsonFileRepository::update(const ToDoItem &item) {
  auto items = readFileAsRepository(filePath);
  auto it = std::find_if(items.begin(), items.end(), [item](const ToDoItem &i) {
    return i.id == item.id;
    });

  if (it != items.end()) {
    *it = item;
  }

  saveRepositoryToFile(items, filePath);
}

std::vector<ToDoItem> ToDoJsonFileRepository::getAll() const {
  return readFileAsRepository(filePath);
}
