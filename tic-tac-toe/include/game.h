#pragma once
#include <memory>
#include "board.h"
#include "player.h"

class PlayerInputProvider {
public:
  virtual std::pair<int, int> getMove(const Board &board, const Player &player) = 0;
  virtual ~PlayerInputProvider() = default;
};

class Game {
private:
  Board board;
  Player player1;
  Player player2;
  Player *currentPlayer;
  std::shared_ptr<PlayerInputProvider> playerInputProvider;

  void switchPlayer();
  void printBoard() const;
  bool makeMove();

public:
  Game(
    std::shared_ptr<PlayerInputProvider> playerInputProvider,
    const Player &player1,
    const Player &player2);
  void start();
};
