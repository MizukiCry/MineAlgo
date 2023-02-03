#ifndef MINEALGO_MS_BOARD_H_
#define MINEALGO_MS_BOARD_H_

#include <algorithm>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

#include "ms_grid.h"
#include "ms_lib.h"

namespace ms_algo {
    using std::vector;

    // The game board of minesweeper.
    class Board {
    private:
        // The number of rows of the game board.
        int row_count_;

        // The number of columns of the game board.
        int column_count_;

        // The game board.
        Matrix<Grid> board_;

    public:
        void Print() const {
            std::cout << "Current Game Board: " << row_count() << " x " << column_count() << std::endl;
            for (int row = 1; row <= row_count(); ++row) {
                for (int column = 1; column <= column_count(); ++column) {
                    Grid grid = get_grid(row, column);
                    switch (grid.state())
                    {
                    case GridState::kUnknown:
                        std::cout << '?';
                        break;
                    case GridState::kFlaged:
                        std::cout << 'x';
                        break;
                    case GridState::kOpened:
                        if (grid.mine_count() == 0) {
                            std::cout << '.';
                        } else {
                            std::cout << grid.mine_count();
                        }
                    }
                }
                std::cout << std::endl;
            }
        }

        void PrintAll() const {
            std::cout << "Actual Game Board: " << row_count() << " x " << column_count() << std::endl;
            for (int row = 1; row <= row_count(); ++row) {
                for (int column = 1; column <= column_count(); ++column) {
                    Grid grid = get_grid(row, column);
                    if (grid.is_mine()) {
                        std::cout << '*';
                    } else if (grid.mine_count() != 0) {
                        std::cout << grid.mine_count();
                    } else {
                        std::cout << '.';
                    }
                }
                std::cout << std::endl;
            }
        }

        void Resize(const int row_count, const int column_count) {
            assert(1 <= row_count && row_count <= 50);
            assert(1 <= column_count && column_count <= 100);
            row_count_ = row_count;
            column_count_ = column_count;
            board_.resize(row_count + 1);
            for (auto& column: board_) {
                column.resize(column_count + 1);
            }
        }

        int row_count() const {
            return row_count_;
        }

        int column_count() const {
            return column_count_;
        }

        Matrix<Grid> board() const {
            return board_;
        }

        Matrix<Grid>& board_ref() {
            return board_;
        }

        bool Inside(int row, int column) const {
            return ms_algo::Inside(row, column, row_count(), column_count());
        }

        Grid get_grid(int row, int column) const {
            assert(Inside(row, column));
            return board_[row][column];
        }

        Grid& get_grid_ref(int row, int column) {
            assert(Inside(row, column));
            return board_[row][column];
        }

        void set_grid(int row, int column, Grid grid) {
            assert(Inside(row, column));
            board_[row][column] = grid;
        }

        int CountMine(int row, int column) {
            assert(Inside(row, column));
            int result = 0;
            for (int index = 0; index < 8; ++index) {
                int next_row = row + kRowOffset[index];
                int next_column = column + kColumnOffset[index];
                if (Inside(next_row, next_column) && get_grid(next_row, next_column).is_mine()) {
                    ++result;
                }
            }
            get_grid_ref(row, column).set_mine_count(result);
            return result;
        }

        void Refresh() {
            for (int row = 1; row <= row_count(); ++row) {
                for (int column = 1; column <= column_count(); ++column) {
                    CountMine(row, column);
                }
            }
        }

        void Open(int row, int column) {
            assert(Inside(row, column));
            Grid& current_grid = get_grid_ref(row, column);
            assert(current_grid.IsUnknown());
            assert(!current_grid.is_mine());
            current_grid.set_state(GridState::kOpened);
            if (current_grid.mine_count() == 0) {
                for (int index = 0; index < 8; ++index) {
                    int next_row = row + kRowOffset[index];
                    int next_column = column + kColumnOffset[index];
                    if (Inside(next_row, next_column) && get_grid(next_row, next_column).IsUnknown()) {
                        Open(next_row, next_column);
                    }
                }
            }
        }

        // Returns current situation of the board.
        Matrix<std::pair<GridState, int>> GetSituation() const {
            Matrix<std::pair<GridState, int>> situation(row_count() + 1, vector<std::pair<GridState, int>>(column_count() + 1));
            for (int row = 1; row <= row_count(); ++row) {
                for (int column = 1; column <= column_count(); ++column) {
                    situation[row][column] = {get_grid(row, column).state(), get_grid(row, column).mine_count()};
                }
            }
            return situation;
        }

        void SetSituation(Matrix<std::pair<GridState, int>>& situation) {
            assert((int)situation.size() == row_count() + 1);
            for (int row = 1; row <= row_count(); ++row) {
                assert((int)situation[row].size() == column_count() + 1);
                for (int column = 1; column <= column_count(); ++column) {
                    if (get_grid(row, column).state() == GridState::kUnknown) {
                        switch (situation[row][column].first)
                        {
                        case GridState::kFlaged:
                            get_grid_ref(row, column).set_state(GridState::kFlaged);
                            break;
                        case GridState::kOpened:
                            Open(row, column);
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }

        bool Solved() const {
            for (int row = 1; row <= row_count(); ++row) {
                for (int column = 1; column <= column_count(); ++column) {
                    if (get_grid(row, column).state() == GridState::kUnknown) {
                        return false;
                    }
                }
            }
            return true;
        }

        Board(int row_count = 1, int column_count = 1) {
            Resize(row_count, column_count);
        }

        ~Board() {}
    };
}

#endif