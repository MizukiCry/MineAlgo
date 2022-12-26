#ifndef MINEALGO_MS_BOARD_H_
#define MINEALGO_MS_BOARD_H_

#include <vector>

#include "ms_grid.h"
#include "ms_lib.h"

namespace ms_algo {
    // The game board of minesweeper.
    class Board {
    private:
        // The number of rows of the game board.
        int row_count_;

        // The number of columns of the game board.
        int column_count_;

        // The game board.
        std::vector<std::vector<Grid>> board_;

    public:
        void Resize(const int row_count, const int column_count) {
            assert(1 <= row_count && row_count <= 50);
            assert(1 <= column_count && column_count <= 100);
            row_count_ = row_count;
            column_count_ = column_count;
            board_.resize(row_count + 1);
            for (auto column: board_) {
                column.resize(column_count + 1);
            }
        }

        int row_count() const {
            return row_count_;
        }

        int column_count() const {
            return column_count_;
        }

        std::vector<std::vector<Grid>> board() const {
            return board_;
        }

        std::vector<std::vector<Grid>>& board_ref() {
            return board_;
        }

        bool Inside(int row, int column) const {
            return 1 <= row && row <= row_count() && 1 <= column && column <= column_count();
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

        int count_mine(int row, int column) {
            assert(Inside(row, column));
            int result = 0;
            for (int i = 0; i < 8; ++i) {
                int around_row = row + kRowOffset[i];
                int around_column = column + kColumnOffset[i];
                if (Inside(around_row, around_column) && get_grid(around_row, around_column).is_mine()) {
                    ++result;
                }
            }
            get_grid_ref(row, column).set_mine_count(result);
            return result;
        }

        void Open(int row, int column) {
            assert(Inside(row, column));
            Grid& current_grid = get_grid_ref(row, column);
            assert(current_grid.IsUnknown());
            assert(!current_grid.is_mine());
            current_grid.set_state(GridState::kOpened);
            if (current_grid.mine_count() == 0) {
                for (int i = 0; i < 8; ++i) {
                    int new_row = row + kRowOffset[i];
                    int new_column = column + kColumnOffset[i];
                    if (Inside(new_row, new_column) && get_grid(new_row, new_column).IsUnknown()) {
                        Open(new_row, new_column);
                    }
                }
            }
        }

        Board(int row_count = 1, int column_count = 1) {
            Resize(row_count, column_count);
        }

        ~Board() {}
    };
}

#endif