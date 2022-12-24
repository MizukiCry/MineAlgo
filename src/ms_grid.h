#ifndef MINEALGO_MS_GRID_H_
#define MINEALGO_MS_GRID_H_

#include <cassert>

enum GridState {
    kOpened,
    kUnknown,
    kFlaged,
};

class Grid {
private:
    // Indicates whether this grid is mine.
    bool is_mine_;

    // The number of mines around this grid.
    int mine_count_;

    // Indicates the state of this grid: opened, unknown or flaged.
    GridState state_;

public:

    bool is_mine() const {
        return is_mine_;
    }

    void set_is_mine(bool value) {
        is_mine_ = value;
    }

    int mine_count() const {
        return mine_count_;
    }

    void set_mine_count(int value) {
        assert(0 <= value && value <= 8);
        mine_count_ = value;
    }

    GridState state() const {
        return state_;
    }

    void set_state(GridState value) {
        state_ = value;
    }

    bool IsOpened() const {
        return state() == GridState::kOpened;
    }

    bool IsUnknown() const {
        return state() == GridState::kUnknown;
    }

    bool IsFlaged() const {
        return state() == GridState::kFlaged;
    }

    Grid(bool is_mine = false, int mine_count = 0, GridState state = GridState::kUnknown) {
        set_is_mine(is_mine);
        set_mine_count(mine_count);
        set_state(state);
    }

    ~Grid() {}

    // Grid operator=(const Grid& o) = delete;
};

#endif