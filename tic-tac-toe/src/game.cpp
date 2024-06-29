#include <iostream>
#include "game.h"

Game::Game(
  std::shared_ptr<PlayerInputProvider> playerInputProvider,
  const Player &player1,
  const Player &player2)
  : playerInputProvider(playerInputProvider),
  player1(player1),
  player2(player2),
  currentPlayer(&this->player1) {}

void Game::switchPlayer() {
  if (currentPlayer == &player1) {
    currentPlayer = &player2;
  }
  else {
    currentPlayer = &player1;
  }
}

void Game::printBoard() const {
  std::cout << board.toString() << std::endl;
}

bool Game::makeMove() {
  const auto currentPlayerSymbol = currentPlayer->symbol;
  const auto [row, col] = playerInputProvider->getMove(board, *currentPlayer);

  if (!board.makeMove(row, col, currentPlayerSymbol)) {
    std::cout << "Invalid move. Try again." << std::endl;
    return false;
  }

  return true;
}

void Game::start() {
  std::cout << "Welcome to Tic-Tac-Toe!" << std::endl;

  while (true) {
    printBoard();

    if (makeMove()) {
      if (board.isWinner(currentPlayer->symbol)) {
        printBoard();
        std::cout << currentPlayer->name << " wins!" << std::endl;
        break;
      }

      if (board.isFull()) {
        printBoard();
        std::cout << "It's a tie!" << std::endl;
        break;
      }

      switchPlayer();
    }
  }

  std::cout << "Game over." << std::endl;
}
