#pragma once
#include <vector>
#include <string>

enum CellState { CellState_EMPTY = -1, CellState_X = 0, CellState_O = 1 };

class Board {
private:
  std::vector<std::vector<CellState>> grid;
  bool checkRows(CellState state) const;
  bool checkCols(CellState state) const;
  bool checkDiagonals(CellState state) const;

public:
  Board();
  void reset();
  bool makeMove(int row, int col, CellState state);
  bool isFull() const;
  bool isWinner(CellState state) const;
  std::string toString() const;
};
