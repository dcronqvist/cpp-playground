#include <iostream>
#include "game.h"

class HumanPlayerInputProvider : public PlayerInputProvider {
public:
  std::pair<int, int> getMove(const Board &board, const Player &player) override {
    int row, col;
    std::string playerSymbolStr = player.symbol == CellState_X ? "X" : "O";
    std::cout << "You are " << playerSymbolStr << " (" << player.name << "), Enter row and column (0-2): ";
    std::cin >> row >> col;
    return { row, col };
  }
};

int main(int argc, char **argv) {
  Player player1 = { "Daniel", CellState_X };
  Player player2 = { "Saga", CellState_O };
  auto playerInputProvider = std::make_shared<HumanPlayerInputProvider>();
  Game game(playerInputProvider, player1, player2);

  game.start();

  return 0;
}
