#include <algorithm>
#include <sstream>
#include "board.h"

Board::Board() {
  grid = std::vector<std::vector<CellState>>(3, std::vector<CellState>(3, CellState_EMPTY));
}

void Board::reset() {
  for (auto &row : grid) {
    std::fill(row.begin(), row.end(), CellState_EMPTY);
  }
}

bool Board::makeMove(int row, int col, CellState state) {
  if (row < 0 || row >= 3 || col < 0 || col >= 3) {
    return false;
  }

  if (grid[row][col] == CellState_EMPTY) {
    grid[row][col] = state;
    return true;
  }

  return false;
}

bool Board::isFull() const {
  for (const auto &row : grid) {
    if (std::any_of(row.begin(), row.end(), [](CellState cell) { return cell == CellState_EMPTY; })) {
      return false;
    }
  }

  return true;
}

bool Board::isWinner(CellState state) const {
  return checkRows(state) || checkCols(state) || checkDiagonals(state);
}

std::string Board::toString() const {
  std::stringstream ss;
  for (const auto &row : grid) {
    for (const auto &cell : row) {
      if (cell == CellState::CellState_EMPTY) ss << ".";
      else if (cell == CellState::CellState_X) ss << "X";
      else ss << "O";
      ss << " ";
    }
    ss << "\n";
  }
  return ss.str();
}

bool Board::checkRows(CellState state) const {
  for (const auto &row : grid) {
    if (std::all_of(row.begin(), row.end(), [state](CellState cell) { return cell == state; })) {
      return true;
    }
  }

  return false;
}

bool Board::checkCols(CellState state) const {
  for (int col = 0; col < 3; col++) {
    if (grid[0][col] == state && grid[1][col] == state && grid[2][col] == state) {
      return true;
    }
  }

  return false;
}

bool Board::checkDiagonals(CellState state) const {
  return (grid[0][0] == state && grid[1][1] == state && grid[2][2] == state) ||
    (grid[0][2] == state && grid[1][1] == state && grid[2][0] == state);
}
