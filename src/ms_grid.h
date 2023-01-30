#ifndef MINEALGO_MS_GRID_H_
#define MINEALGO_MS_GRID_H_

#include <cassert>

namespace ms_algo {
    // Describes the state of a grid.
    enum GridState {
        kUnknown,
        kOpened,
        kFlaged,
    };

    // A grid of the game board.
    class Grid {
    private:
        // Indicates whether this grid is mine.
        bool is_mine_;

        // The number of mines around this grid.
        int mine_count_;

        // Indicates the state of this grid: opened, unknown or flaged.
        GridState state_;

    public:
        // Returns whether this grid is mine.
        bool is_mine() const {
            return is_mine_;
        }

        // Sets whether this grid is mine.
        void set_is_mine(bool value = true) {
            is_mine_ = value;
        }

        // Returns the number of mines around.
        int mine_count() const {
            return mine_count_;
        }

        // Sets the number of mines around.
        void set_mine_count(int value) {
            assert(0 <= value && value <= 8);
            mine_count_ = value;
        }

        // Returns the state of this grid.
        GridState state() const {
            return state_;
        }

        // Sets the state of this grid.
        void set_state(GridState value) {
            state_ = value;
        }

        // Returns whether this grid is opened.
        bool IsOpened() const {
            return state() == GridState::kOpened;
        }

        // Returns whether this grid is unknown.
        bool IsUnknown() const {
            return state() == GridState::kUnknown;
        }

        // Returns whether this grid is flaged.
        bool IsFlaged() const {
            return state() == GridState::kFlaged;
        }

        // Makes a new grid.
        Grid(bool is_mine = false, int mine_count = 0, GridState state = GridState::kUnknown) {
            set_is_mine(is_mine);
            set_mine_count(mine_count);
            set_state(state);
        }

        ~Grid() {}

        // Grid operator=(const Grid& o) = delete;
    };
}

#endif