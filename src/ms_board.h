#ifndef MINEALGO_MS_BOARD_H_
#define MINEALGO_MS_BOARD_H_

#include <vector>

#include "ms_grid.h"

using std::vector;

const int kRowOffset[] = {-1, -1, -1, 0, 0, 1, 1, 1};
const int kColumnOffset[] = {-1, 0, 1, -1, 1, -1, 0, 1};

class Board {
private:
    // The number of rows and columns of the game board.
    int row_count_, column_count_;

    // The game board
    vector<vector<Grid>> board_;

public:
    void Resize(int row_count, int column_count) {
        assert(1 <= row_count && row_count <= 50);
        assert(1 <= column_count && column_count <= 100);
        row_count_ = row_count;
        column_count_ = column_count;
        board_.resize(row_count + 1);
        for (auto column: board_) {
            column.resize(column_count + 1);
        }
    }

    int row_count() {
        return row_count_;
    }

    int column_count() {
        return column_count_;
    }

    vector<vector<Grid>> board() {
        return board_;
    }

    bool Inside(int row, int column) {
        return 1 <= row && row <= row_count() && 1 <= column && column <= column_count();
    }

    Grid get_grid(int row, int column) {
        assert(Inside(row, column));
        return board_[row][column];
    }

    void set_grid(int row, int column, Grid grid) {
        assert(Inside(row, column));
        board_[row][column] = grid;
    }

    void set_grid_state(int row, int column, GridState state) {
        assert(Inside(row, column));
        board_[row][column].set_state(state);
    }

    void Open(int row, int column) {
        assert(Inside(row, column));
        Grid current_grid = get_grid(row, column);
        assert(current_grid.IsUnknown());
        assert(!current_grid.is_mine());
        set_grid_state(row, column, GridState::kOpened);
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

#endif