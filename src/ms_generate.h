#ifndef MINEALGO_MS_GENERATE_H_
#define MINEALGO_MS_GENERATE_H_

#include <vector>
#include <algorithm>

#include "ms_board.h"

namespace ms_algo {
    enum RestrictionType {
        kUnrestricted,
        kIsMine,
        kIsNumber,
    };

    class GridRestriction {
    public:
        RestrictionType restriction;
        int number;
    };

    enum GenerateType {
        kNormal,
        kSolvable,
    };

    

    std::pair<bool, Board> GenerateNormal(int row_count, int column_count, int total_mine_count) {
        assert(1 <= row_count && row_count <= 50);
        assert(1 <= column_count && column_count <= 100);
        assert(1 <= total_mine_count && total_mine_count <= row_count * column_count);

        std::vector<std::pair<int, int>> grids;
        for (int row = 1; row <= row_count; ++row) {
            for (int column = 1; column <= column_count; ++column) {
                grids.emplace_back(row, column);
            }
        }
        std::shuffle(grids.begin(), grids.end(), ms_rand);

        Board result(row_count, column_count);
        for (int i = 0; i < total_mine_count; ++i) {
            auto [row, column] = grids[i];
            result.get_grid_ref(row, column).set_is_mine();
        }
        for (int row = 1; row <= row_count; ++row) {
            for (int column = 1; column <= column_count; ++column) {
                result.count_mine(row, column);
            }
        }
        return {true, result};
    }

    std::pair<bool, Board> GenerateSolvable(int row_count, int column_count, int total_mine_count) {

    }

    std::pair<bool, Board> Generate(int row_count, int column_count, GenerateType type = GenerateType::kNormal, double time_limit = 1.0, int total_mine_count = 0) {
        if (total_mine_count == 0) {
            total_mine_count = row_count * column_count / 5;
        }
        if (type == GenerateType::kNormal) {
            return GenerateNormal(row_count, column_count, total_mine_count);
        } else {
            return GenerateSolvable(row_count, column_count, total_mine_count);
        }
    }
}

#endif