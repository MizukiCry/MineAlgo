#ifndef MINEALGO_MS_GENERATE_H_
#define MINEALGO_MS_GENERATE_H_

#include <algorithm>
#include <atomic>
#include <cassert>
#include <future>
#include <vector>

#include "ms_board.h"

namespace ms_algo {
    using std::vector;

    enum RestrictionType {
        kUnrestricted,
        kIsMine,
        kNotMine,
    };

    enum GenerateType {
        kNormal,
        kSolvable,
    };

    // (Do not call this function directly) Generates a game board randomly.
    std::pair<bool, Board> GenerateNormal(
        int row_count,
        int column_count,
        int randomize_mine_count,
        vector<vector<RestrictionType>> restriction
    ) {
        Board result(row_count, column_count);
        vector<std::pair<int, int>> grids;
        for (int row = 1; row <= row_count; ++row) {
            for (int column = 1; column <= column_count; ++column) {
                switch (restriction[row][column]) {
                case RestrictionType::kUnrestricted:
                    grids.emplace_back(row, column);
                    break;
                case RestrictionType::kIsMine:
                    randomize_mine_count -= 1;
                    result.get_grid_ref(row, column).set_is_mine();
                    break;
                }
            }
        }

        if (randomize_mine_count < 0 || randomize_mine_count > (int)grids.size()) {
            return {false, result};
        }
        ShuffleVector(grids);
        for (int i = 0; i < randomize_mine_count; ++i) {
            auto [row, column] = grids[i];
            result.get_grid_ref(row, column).set_is_mine();
        }
        result.Refresh();
        return {true, result};
    }

    // (Do not call this function directly) Tries to generate a solvable game board.
    std::pair<bool, Board> TryGenerateSolvable(
        int row_count,
        int column_count,
        int randomize_mine_count,
        const vector<vector<RestrictionType>>& restriction,
        std::atomic_bool& time_up
    ) {
        Board initial_board(row_count, column_count);
        std::vector<std::pair<int, int>> grids;
        for (int row = 1; row <= row_count; ++row) {
            for (int column = 1; column <= column_count; ++column) {
                switch (restriction[row][column])
                {
                case RestrictionType::kIsMine:
                    initial_board.get_grid_ref(row, column).set_is_mine();
                    break;
                case RestrictionType::kUnrestricted:
                    grids.emplace_back(row, column);
                    break;
                }
            }
        }

        while(!time_up) {
            Board result(initial_board);
            ShuffleVector(grids);
            for (int i = 0; i < randomize_mine_count; ++i) {
                auto [row, column] = grids[i];
                result.get_grid_ref(row, column).set_is_mine();
            }
            result.Refresh();
            if (Solvable(result)) {
                time_up = true;
                return {true, result};
            }
        }
        return {};
    }

    // (Do not call this function directly) Calls TryGenerateSolvable() int multiple threads
    std::pair<bool, Board> GenerateSolvable(
        int row_count,
        int column_count,
        int time_limit_milliseconds,
        int randomize_mine_count,
        int thread_count,
        vector<vector<RestrictionType>> restriction
    ) {
        std::atomic_bool time_up = false;

        std::async(std::launch::async, [time_limit_milliseconds, &time_up] {
            std::this_thread::sleep_for(std::chrono::milliseconds(time_limit_milliseconds));
            time_up = true;
        });

        std::vector<std::future<std::pair<bool, Board>>> results(thread_count);
        for (auto &result: results) {
            result = std::async(TryGenerateSolvable, row_count, column_count, randomize_mine_count, std::cref(restriction), std::ref(time_up));
        }

        for (auto &result: results) {
            if (result.get().first) {
                return result.get();
            }
        }
        return {};
    }

    /**
        @brief Generates a game board according to the arguments.
        @param row_count The number of rows.
        @param column_count The number of columns.
        @param type The type of board to be generated, completely random by `GenerateType::kNormal` and solvable without any guess by `GenerateType::kSolvable`.
        @param time_limit_milliseconds The time limitation, default by 1000 ms. (May not be accurate)
        @param randomize_mine_count The number of mines to be added into the board.
        @param restriction The restrictions of the board.
    */
    std::pair<bool, Board> Generate(
        int row_count,
        int column_count,
        vector<vector<RestrictionType>> restriction,
        GenerateType type = GenerateType::kNormal,
        int time_limit_milliseconds = 1000,
        int thread_count = 1,
        int randomize_mine_count = 0
    ) {
        assert(1 <= row_count && row_count <= kMaxRowCount);
        assert(1 <= column_count && column_count <= kMaxColumnCount);
        assert((int)restriction.size() == row_count + 1);
        for (int row = 1; row <= row_count; ++row) {
            assert((int)restriction[row].size() == column_count + 1);
        }
        assert(1 <= time_limit_milliseconds && time_limit_milliseconds <= kMaxTimeLimitMilliseconds);
        assert(1 <= thread_count && thread_count <= kMaxThreadCount);

        int max_randomize_mine_count = 0;
        for (int row = 1; row <= row_count; ++row) {
            for (int column = 1; column <= column_count; ++column) {
                if (restriction[row][column] == RestrictionType::kUnrestricted) {
                    ++max_randomize_mine_count;
                }
            }
        }
        if (randomize_mine_count == 0) {
            randomize_mine_count = std::min(row_count * column_count / 5, max_randomize_mine_count / 2);
        }
        assert(0 <= randomize_mine_count && randomize_mine_count <= max_randomize_mine_count);
        if (type == GenerateType::kNormal) {
            return GenerateNormal(row_count, column_count, randomize_mine_count, restriction);
        } else {
            return GenerateSolvable(row_count, column_count, time_limit_milliseconds, randomize_mine_count, thread_count, restriction);
        }
    }

    /**
        @brief Generates a game board according to the arguments.
        @param row_count The number of rows.
        @param column_count The number of columns.
        @param type The type of board to be generated, completely random by `GenerateType::kNormal` and solvable without any guess by `GenerateType::kSolvable`.
        @param time_limit_milliseconds The time limitation, default by 1000 ms. (May not be accurate)
        @param randomize_mine_count The number of mines to be added into the board.
        @param start_row The row of the starting position guaranteed not to be mine. 0 means no limitation.
        @param start_column The column of the starting position guaranteed not to be mine. 0 means no limitation.
    */
    std::pair<bool, Board> Generate(
        int row_count,
        int column_count,
        int start_row,
        int start_column,
        GenerateType type = GenerateType::kNormal,
        int time_limit_milliseconds = 1000,
        int thread_count = 1,
        int randomize_mine_count = 0
    ) {
        assert(1 <= row_count && row_count <= kMaxRowCount);
        assert(1 <= column_count && column_count <= kMaxColumnCount);
        assert(1 <= start_row && start_row <= row_count);
        assert(1 <= start_column && start_column <= column_count);
        vector<vector<RestrictionType>> restriction(row_count + 1, vector<RestrictionType>(column_count + 1, RestrictionType::kUnrestricted));
        restriction[start_row][start_column] = RestrictionType::kNotMine;
        return Generate(row_count, column_count, restriction, type, time_limit_milliseconds, thread_count, randomize_mine_count);
    }
}

#endif