#include <iostream>
#include <format>
#include <optional>

#include "todo_jsonfile_repository.h"
#include "todo_service.h"
#include "nlohmann/json.hpp"

void printToDoItem(const ToDoItem &item) {
  std::cout << std::format(
    "ID: {}\nTitle: {}\nDescription: {}\nCompleted: {}",
    item.id,
    item.title,
    item.description,
    item.completed);
}

void printSeparator() {
  std::cout << "----------------------------------------" << std::endl;
}

void printRepository(const ToDoRepository &repository, bool onlyIncomplete) {
  auto allItems = repository.getAll();
  std::vector<ToDoItem> incompleteItems{};

  std::copy_if(allItems.begin(), allItems.end(), std::back_inserter(incompleteItems), [](const auto &item) {
    return !item.completed;
    });

  auto items = onlyIncomplete ? incompleteItems : allItems;

  if (items.empty()) {
    std::cout << "To-do list is empty!" << std::endl;
    return;
  }

  printSeparator();
  for (const auto &item : items) {
    printToDoItem(item);
    std::cout << std::endl;
    printSeparator();
  }
}

struct CommandLineArgs {
  bool add;
  std::string addTitle;
  std::string addDescription;

  bool remove;
  int removeId;

  bool complete;
  int completeId;

  bool print;

  std::optional<std::string> failedToParseMessage;
};

CommandLineArgs parseCommandLineArgs(int argc, char **argv) {
  CommandLineArgs args{};

  // Debug print
  // for (int i = 0; i < argc; i++) {
  //   std::cout << "\"" << argv[i] << "\"" << std::endl;
  // }

  if (argc < 2) {
    args.failedToParseMessage = "No arguments provided!";
    return args;
  }

  std::string cmd = std::string(argv[1]);

  if (cmd == "add") {
    // argv[2] must be title
    // argv[3] must be description
    if (argc < 4) {
      args.failedToParseMessage = "Title and description must be provided when adding!";
      return args;
    }

    args.add = true;
    args.addTitle = argv[2];
    args.addDescription = argv[3];
    return args;
  }

  if (cmd == "remove") {
    // argv[2] must be id
    if (argc < 3) {
      args.failedToParseMessage = "ID must be provided when removing!";
      return args;
    }

    args.remove = true;
    args.removeId = std::stoi(argv[2]);
    return args;
  }

  if (cmd == "complete") {
    // argv[2] must be id
    if (argc < 3) {
      args.failedToParseMessage = "ID must be provided when completing!";
      return args;
    }

    args.complete = true;
    args.completeId = std::stoi(argv[2]);
    return args;
  }

  if (cmd == "print") {
    args.print = true;
    return args;
  }

  args.failedToParseMessage = "Invalid arguments provided!";
  return args;
}

int main(int argc, char **argv)
{
  auto commandLineArgs = parseCommandLineArgs(argc, argv);
  if (commandLineArgs.failedToParseMessage.has_value()) {
    std::cout << commandLineArgs.failedToParseMessage.value() << std::endl;
    return 1;
  }

  ToDoJsonFileRepository repository{ "todo.json" };
  ToDoService service{ repository };

  if (commandLineArgs.add) {
    service.add(ToDoItem{ service.getNextId(), commandLineArgs.addTitle, commandLineArgs.addDescription, false });
    std::cout << "Added to-do item!" << std::endl;
    return 0;
  }

  if (commandLineArgs.remove) {
    service.remove(commandLineArgs.removeId);
    std::cout << "Removed to-do item!" << std::endl;
    return 0;
  }

  if (commandLineArgs.complete) {
    service.complete(commandLineArgs.completeId);
    std::cout << "Completed to-do item!" << std::endl;
    return 0;
  }

  if (commandLineArgs.print) {
    printRepository(repository, true);
    return 0;
  }

  return 0;
}
