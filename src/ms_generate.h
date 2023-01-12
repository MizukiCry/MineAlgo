#ifndef MINEALGO_MS_GENERATE_H_
#define MINEALGO_MS_GENERATE_H_

#include <algorithm>
#include <cassert>
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
        int total_mine_count,
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
                    total_mine_count -= 1;
                    result.get_grid_ref(row, column).set_is_mine();
                    break;
                }
            }
        }

        if (total_mine_count < 0 || total_mine_count > (int)grids.size()) {
            return {false, result};
        }
        std::shuffle(grids.begin(), grids.end(), ms_rand);
        for (int i = 0; i < total_mine_count; ++i) {
            auto [row, column] = grids[i];
            result.get_grid_ref(row, column).set_is_mine();
        }
        result.Refresh();
        return {true, result};
    }

    std::pair<bool, Board> GenerateSolvable(
        int row_count,
        int column_count,
        double time_limit,
        int total_mine_count,
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
                    total_mine_count -= 1;
                    result.get_grid_ref(row, column).set_is_mine();
                    break;
                }
            }
        }
        return {};
    }

    /**
        @brief Generates a game board according to the arguments.
        @param row_count The number of rows.
        @param column_count The number of columns.
        @param type The type of board to be generated, completely random by `GenerateType::kNormal` and solvable without any guess by `GenerateType::kSolvable`.
        @param time_limit The time limitation, default by 1.0s. (May not be accurate)
        @param total_mine_count The number of mines in the board.
        @param restriction The restrictions of the board.
    */
    std::pair<bool, Board> Generate(
        int row_count,
        int column_count,
        vector<vector<RestrictionType>> restriction,
        GenerateType type = GenerateType::kNormal,
        double time_limit = 1.0,
        int total_mine_count = 0
    ) {
        assert(1 <= row_count && row_count <= 50);
        assert(1 <= column_count && column_count <= 100);
        assert((int)restriction.size() == row_count + 1);
        for (int row = 1; row <= row_count; ++row) {
            assert((int)restriction[row].size() == column_count + 1);
        }
        assert(time_limit > kEpsilon);
        if (total_mine_count == 0) {
            total_mine_count = row_count * column_count / 5;
        }
        assert(0 <= total_mine_count && total_mine_count <= row_count * column_count);
        if (type == GenerateType::kNormal) {
            return GenerateNormal(row_count, column_count, total_mine_count, restriction);
        } else {
            return GenerateSolvable(row_count, column_count, time_limit, total_mine_count, restriction);
        }
    }

    /**
        @brief Generates a game board according to the arguments.
        @param row_count The number of rows.
        @param column_count The number of columns.
        @param type The type of board to be generated, completely random by `GenerateType::kNormal` and solvable without any guess by `GenerateType::kSolvable`.
        @param time_limit The time limitation, default by 1.0s. (May not be accurate)
        @param total_mine_count The number of mines in the board.
        @param start_row The row of the starting position guaranteed not to be mine. 0 means no limitation.
        @param start_column The column of the starting position guaranteed not to be mine. 0 means no limitation.
    */
    std::pair<bool, Board> Generate(
        int row_count,
        int column_count,
        int start_row,
        int start_column,
        GenerateType type = GenerateType::kNormal,
        double time_limit = 1.0,
        int total_mine_count = 0
    ) {
        assert(1 <= row_count && row_count <= 50);
        assert(1 <= column_count && column_count <= 100);
        assert(1 <= start_row && start_row <= row_count);
        assert(1 <= start_column && start_column <= column_count);
        vector<vector<RestrictionType>> restriction(row_count + 1, vector<RestrictionType>(column_count + 1, RestrictionType::kUnrestricted));
        restriction[start_row][start_column] = RestrictionType::kNotMine;
        return Generate(row_count, column_count, restriction, type, time_limit, total_mine_count);
    }
}

#endif