#ifndef MINEALGO_MS_GENERATE_H_
#define MINEALGO_MS_GENERATE_H_

#include <algorithm>
#include <atomic>
#include <cassert>
#include <functional>
#include <future>
#include <iostream>
#include <vector>

#include "ms_board.h"
#include "ms_solve.h"
#include "ms_timer.h"

namespace ms_algo {
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
        int random_mine_count,
        Matrix<RestrictionType> restriction
    ) {
        if (kPrintDebugInfo) {
            std::clog << "GenerateNormal " << row_count << " x " << column_count << " : " << random_mine_count << std::endl;
        }

        Board result(row_count, column_count);
        vector<std::pair<int, int>> grids;
        for (int row = 1; row <= row_count; ++row) {
            for (int column = 1; column <= column_count; ++column) {
                switch (restriction[row][column]) {
                case RestrictionType::kUnrestricted:
                    grids.emplace_back(row, column);
                    break;
                case RestrictionType::kIsMine:
                    random_mine_count -= 1;
                    result.get_grid_ref(row, column).set_is_mine();
                    break;
                default:
                    break;
                }
            }
        }
        if (random_mine_count < 0 || random_mine_count > (int)grids.size()) {
            return {false, result};
        }
        ShuffleVector(grids);
        for (int i = 0; i < random_mine_count; ++i) {
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
        int random_mine_count,
        const Board& initial_board,
        vector<std::pair<int, int>> grids,
        Timer& timer
    ) {
        if (kPrintDebugInfo) {
            std::clog << "TryGenerateSolvable: " << row_count << " x " << column_count << " : " << random_mine_count << std::endl;
            std::clog << "Grids: " << grids.size() << 'x' << std::endl;
            for (auto [row, column]: grids) {
                std::clog << '(' << row << ", " << column << ") ";
            }
            std::clog << std::endl;
        }

        while(!timer.TimeIsUp()) {
            Board result(initial_board);
            ShuffleVector(grids);
            for (int i = 0; i < random_mine_count; ++i) {
                auto [row, column] = grids[i];
                result.get_grid_ref(row, column).set_is_mine();
            }
            result.Refresh();
            if (Solvable(result, timer)) {
                timer.Terminate();
                return {true, result};
            }
        }
        if (kPrintDebugInfo) {
            std::clog << "TryGenerateSolvable Timeout!" << std::endl;
        }
        return {};
    }

    // (Do not call this function directly) Calls TryGenerateSolvable() in multiple threads
    std::pair<bool, Board> GenerateSolvable(
        int row_count,
        int column_count,
        int time_limit_milliseconds,
        int random_mine_count,
        int thread_count,
        Matrix<RestrictionType> restriction,
        Matrix<GridState> gridstate
    ) {
        if (kPrintDebugInfo) {
            std::clog << "GenerateSolvable: " << row_count << " x " << column_count << std::endl;
            std::clog << "TimeLimit: " << time_limit_milliseconds << "ms" << std::endl;
            std::clog << "RandomMine: " << random_mine_count << std::endl;
            std::clog << "Thread: " << thread_count << 'x' << std::endl;

            std::clog << "\nRestriction: " << std::endl;
            for (int row = 1; row <= row_count; ++row) {
                for (int column = 1; column <= column_count; ++column) {
                    std::clog << restriction[row][column];
                }
                std::clog << std::endl;
            }

            std::clog << "\nGridState: " << std::endl;
            for (int row = 1; row <= row_count; ++row) {
                for (int column = 1; column <= column_count; ++column) {
                    std::clog << gridstate[row][column];
                }
                std::clog << std::endl;
            }
        }

        Timer timer(time_limit_milliseconds);

        Board initial_board(row_count, column_count);
        vector<std::pair<int, int>> grids;
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
                default:
                    break;
                }
                initial_board.get_grid_ref(row, column).set_state(gridstate[row][column]);
            }
        }

        vector<std::future<std::pair<bool, Board>>> results(thread_count);
        for (auto &result: results) {
            result = std::async(TryGenerateSolvable, row_count, column_count, random_mine_count, std::cref(initial_board), grids, std::ref(timer));
        }

        for (auto &result: results) {
            auto [result_state, board] = result.get();
            if (result_state) {
                if (kPrintDebugInfo) {
                    std::clog << "GenerateSolvable Succeed!" << std::endl;
                }
                return {true, board};
            }
        }
        return {false, {}};
    }

    /**
        @brief Generates a game board according to the arguments.
        @param row_count The number of rows.
        @param column_count The number of columns.
        @param restriction The restriction of the board, 'RestrictionType::kUnrestricted', 'RestrictionType::kIsMine' or 'RestrictionType::kNotMine'.
        @param gridstate The state of the board, 'GridState::kUnknown', 'GridState::kOpened' or 'GridState::kFlaged'.
        @param type The type of board to be generated, completely random by `GenerateType::kNormal` and solvable without any guess by `GenerateType::kSolvable`.
        @param time_limit_milliseconds The time limitation, default by 1000 ms. (May not be accurate)
        @param random_mine_count The number of mines to be added into the board.
        @param restriction The restrictions of the board.
    */
    std::pair<bool, Board> Generate(
        int row_count,
        int column_count,
        Matrix<RestrictionType> restriction,
        Matrix<GridState> gridstate,
        GenerateType type = GenerateType::kNormal,
        int time_limit_milliseconds = 1000,
        int thread_count = 1,
        int random_mine_count = 0
    ) {
        assert(1 <= row_count && row_count <= kMaxRowCount);
        assert(1 <= column_count && column_count <= kMaxColumnCount);

        assert((int)restriction.size() == row_count + 1);
        assert((int)gridstate.size() == row_count + 1);
        for (int row = 1; row <= row_count; ++row) {
            assert((int)restriction[row].size() == column_count + 1);
            assert((int)gridstate[row].size() == column_count + 1);
        }
        assert(1 <= time_limit_milliseconds && time_limit_milliseconds <= kMaxTimeLimitMilliseconds);
        assert(1 <= thread_count && thread_count <= kMaxThreadCount);

        int max_random_mine_count = 0;
        for (int row = 1; row <= row_count; ++row) {
            for (int column = 1; column <= column_count; ++column) {
                if (restriction[row][column] == RestrictionType::kUnrestricted) {
                    ++max_random_mine_count;
                }
            }
        }
        if (random_mine_count == 0) {
            random_mine_count = std::min(int(row_count * column_count * 0.15), max_random_mine_count / 4);
        }
        assert(0 <= random_mine_count && random_mine_count <= max_random_mine_count);
        if (type == GenerateType::kNormal) {
            return GenerateNormal(row_count, column_count, random_mine_count, restriction);
        } else {
            return GenerateSolvable(row_count, column_count, time_limit_milliseconds, random_mine_count, thread_count, restriction, gridstate);
        }
    }

    /**
        @brief Generates a game board according to the arguments.
        @param row_count The number of rows.
        @param column_count The number of columns.
        @param start_row The row of the starting position guaranteed not to be mine. 0 means no limitation.
        @param start_column The column of the starting position guaranteed not to be mine. 0 means no limitation.
        @param type The type of board to be generated, completely random by `GenerateType::kNormal` and solvable without any guess by `GenerateType::kSolvable`.
        @param time_limit_milliseconds The time limitation, default by 1000 ms. (May not be accurate)
        @param thread_count Enables multithreading by greater than 1.
        @param random_mine_count The number of mines to be added into the board.
    */
    std::pair<bool, Board> Generate(
        int row_count,
        int column_count,
        int start_row,
        int start_column,
        GenerateType type = GenerateType::kNormal,
        int time_limit_milliseconds = 1000,
        int thread_count = 1,
        int random_mine_count = 0
    ) {
        assert(1 <= row_count && row_count <= kMaxRowCount);
        assert(1 <= column_count && column_count <= kMaxColumnCount);

        if (start_row == 0) {
            start_row = RandInteger(0, row_count) + 1;
        }
        if (start_column == 0) {
            start_column = RandInteger(0, column_count) + 1;
        }

        assert(1 <= start_row && start_row <= row_count);
        assert(1 <= start_column && start_column <= column_count);

        Matrix<RestrictionType> restriction(row_count + 1, vector<RestrictionType>(column_count + 1, RestrictionType::kUnrestricted));
        Matrix<GridState> gridstate(row_count + 1, vector<GridState>(column_count + 1, GridState::kUnknown));
        restriction[start_row][start_column] = RestrictionType::kNotMine;
        gridstate[start_row][start_column] = GridState::kOpened;
        return Generate(row_count, column_count, restriction, gridstate, type, time_limit_milliseconds, thread_count, random_mine_count);
    }
}

#endif