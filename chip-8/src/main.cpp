#include <fstream>

#include "chip8.h"

int main(int arc, char **argv) {

  // Read the game data from the file at argv[1]
  std::ifstream file(argv[1], std::ios::binary);
  if (!file.is_open()) {
    return 1;
  }

  file.seekg(0, std::ios::end);
  int fileSize = file.tellg();
  file.seekg(0, std::ios::beg);

  unsigned char *gameData = new unsigned char[fileSize];
  file.read((char *)gameData, fileSize);
  file.close();

  Chip8 chip8 = Chip8(gameData, fileSize);
  return chip8.run();
}
