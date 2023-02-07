#ifndef MINEALGO_MS_SOLVE_H_
#define MINEALGO_MS_SOLVE_H_

#include <algorithm>
#include <atomic>
#include <cassert>
#include <future>
#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>

#include "ms_board.h"
#include "ms_grid.h"
#include "ms_lib.h"
#include "ms_timer.h"

namespace ms_algo {
    vector<std::pair<int, int>> GaussianElimination(Matrix<double>& matrix) {
        if (kPrintDebugInfo) {
            std::clog << "GaussianElimination:" << std::endl;
            std::clog << "Before Gaussian:" << std::endl;
            for (const auto& row: matrix) {
                for (auto number: row) {
                    std::clog << std::fixed << std::setprecision(0) << number << ' ';
                }
                std::clog << std::endl;
            }
        }

        int unfree_variable_count = 0;
        for (size_t current = 0; current < matrix[0].size(); ++current) {
            int max_row = unfree_variable_count;
            for (size_t row = max_row + 1; row < matrix.size(); ++row) {
                if (Greater(std::abs(matrix[row][current]), std::abs(matrix[max_row][current]))) {
                    max_row = row;
                }
            }

            if (IsZero(matrix[max_row][current])) {
                continue;
            }

            if (unfree_variable_count != max_row) {
                std::swap(matrix[unfree_variable_count], matrix[max_row]);
            }

            for (int row = 0; row < (int)matrix.size(); ++row) {
                if (row != unfree_variable_count && NotZero(matrix[row][current])) {
                    matrix[row] -= matrix[unfree_variable_count] * (matrix[row][current] / matrix[unfree_variable_count][current]);
                }
            }
            matrix[unfree_variable_count] /= matrix[unfree_variable_count][current];

            ++unfree_variable_count;
            if (unfree_variable_count == (int)matrix.size()) {
                break;
            }
        }
        matrix.resize(unfree_variable_count);

        if (kPrintDebugInfo) {
            std::clog << "After Gaussian:" << std::endl;
            for (const auto& row: matrix) {
                for (auto number: row) {
                    std::clog << std::fixed << std::setprecision(0) << number << ' ';
                }
                std::clog << std::endl;
            }
        }

        vector<std::pair<int, int>> result;
        for (const auto& row: matrix) {
            int not_zero_position = -1;
            for (size_t column = 0; column + 1 < row.size(); ++column) {
                if (NotZero(row[column])) {
                    if (not_zero_position == -1) {
                        not_zero_position = column;
                    } else {
                        not_zero_position = -1;
                        break;
                    }
                }
            }
            if (not_zero_position != -1) {
                if (IsZero(row.back())) {
                    result.emplace_back(not_zero_position, 0);
                } else if (Equal(row.back(), 1.0)) {
                    result.emplace_back(not_zero_position, 1);
                } else {
                    std::cerr << "Gaussian Elimination Error." << std::endl;
                    assert(false);
                }
            }
        }
        return result;
    }

    std::pair<int64_t, vector<int64_t>> EnumerateMine(const Matrix<double>& matrix, Timer& timer) {
        int variable_count = (*matrix.begin()).size() - 1;
        int unfree_variable_count = matrix.size();
        int free_variable_count = variable_count - unfree_variable_count;

        vector<int> free_variable_positions;
        vector<int> unfree_variable_positions;
        free_variable_positions.reserve(free_variable_count);
        unfree_variable_positions.reserve(unfree_variable_count);
        for (const auto& row: matrix) {
            for (size_t column = 0; column + 1 < row.size(); ++column) {
                if (NotZero(row[column])) {
                    unfree_variable_positions.push_back(column);
                    break;
                }
            }
        }
        for (int index = 0; index < variable_count; ++index) {
            if (std::find(unfree_variable_positions.begin(), unfree_variable_positions.end(), index) == unfree_variable_positions.end()) {
                free_variable_positions.push_back(index);
            }
        }

        int64_t legal_count = 0;
        vector<int64_t> count(variable_count, 0);
        for (int64_t situation = (1 << free_variable_count) - 1; situation >= 0; --situation) {
            if (timer.TimeIsUp()) {
                if (kPrintDebugInfo) {
                    std::cerr << "EnumerateMine Timeout!" << std::endl;
                }
                return {};
            }
            vector<double> unfree_variables;
            unfree_variables.reserve(unfree_variable_count);
            bool illegal = false;
            for (int unfree_variable_index = 0; unfree_variable_index < unfree_variable_count; ++unfree_variable_index) {
                double unfree_variable_value = matrix[unfree_variable_index].back();
                for (int index = 0; index < free_variable_count; ++index) {
                    unfree_variable_value -= (situation >> index & 1) * matrix[unfree_variable_index][free_variable_positions[index]];
                }
                if (!Equal(unfree_variable_value, 0.0) && !Equal(unfree_variable_value, 1.0)) {
                    illegal = true;
                    break;
                }
                unfree_variables.push_back(unfree_variable_value);
            }
            if (illegal) {
                continue;
            }
            ++legal_count;
            for (int index = 0; index < free_variable_count; ++index) {
                if ((situation >> index & 1)) {
                    ++count[free_variable_positions[index]];
                }
            }
            for (int index = 0; index < unfree_variable_count; ++index) {
                if (Equal(unfree_variables[index], 1.0)) {
                    ++count[unfree_variable_positions[index]];
                }
            }
        }
        return {legal_count, count};
    }

    using Positions = vector<std::pair<int, int>>;
    using Region = std::pair<Positions, Matrix<double>>;

    void Search(
        int row,
        int column,
        int row_count,
        int column_count,
        const Matrix<std::pair<GridState, int>>& states,
        Matrix<int>& search_states,
        Positions& known_positions,
        Positions& unknown_positions
    ) {
        GridState current_state = states[row][column].first;
        if (current_state == GridState::kOpened) {
            known_positions.emplace_back(row, column);
        } else {
            unknown_positions.emplace_back(row, column);
        }
        search_states[row][column] = -1;

        for (int index = 0; index < 8; ++index) {
            int next_row = row + kRowOffset[index];
            int next_column = column + kColumnOffset[index];
            if (!Inside(next_row, next_column, row_count, column_count)) {
                continue;
            }
            if (search_states[next_row][next_column] > -2) {
                continue;
            }

            bool search_next = false;
            if (current_state == GridState::kOpened && states[next_row][next_column].first == GridState::kUnknown) {
                search_next = true;
            }
            else if (current_state == GridState::kUnknown && search_states[next_row][next_column] == -2) {
                search_next = true;
            }
            if (search_next) {
                Search(next_row, next_column, row_count, column_count, states, search_states, known_positions, unknown_positions);
            }
        }
    }

    vector<Region> Divide(int row_count, int column_count, const Matrix<std::pair<GridState, int>>& states) {
        vector<Region> result;
        Matrix<int> search_states(row_count + 1, vector<int>(column_count + 1, -3));

        for (int row = 1; row <= row_count; ++row) {
            for (int column = 1; column <= column_count; ++column) {
                if (states[row][column].first != GridState::kOpened) {
                    continue;
                }

                bool unsolved = false;
                for (int index = 0; index < 8; ++index) {
                    int next_row = row + kRowOffset[index];
                    int next_column = column + kColumnOffset[index];
                    if (Inside(next_row, next_column, row_count, column_count)) {
                        if (states[next_row][next_column].first == GridState::kUnknown) {
                            unsolved = true;
                            break;
                        }
                    }
                }
                if (unsolved) {
                    search_states[row][column] = -2;
                }
            }
        }

        for (int row = 1; row <= row_count; ++row) {
            for (int column = 1; column <= column_count; ++column) {
                if (search_states[row][column] != -2) {
                    continue;
                }

                Positions known_positions, unknown_positions;
                Search(row, column, row_count, column_count, states, search_states, known_positions, unknown_positions);
                ShuffleVector(unknown_positions);
                for (int index = 0; index < (int)unknown_positions.size(); ++index) {
                    auto [p_row, p_column] = unknown_positions[index];
                    search_states[p_row][p_column] = index;
                }

                Matrix<double> gauss_matrix;
                for (auto [p_row, p_column]: known_positions) {
                    vector<double> equation(unknown_positions.size() + 1, 0.0);

                    int mine_count = states[p_row][p_column].second;
                    for (int index = 0; index < 8; ++index) {
                        int next_row = p_row + kRowOffset[index];
                        int next_column = p_column + kColumnOffset[index];

                        if (Inside(next_row, next_column, row_count, column_count)) {
                            switch (states[next_row][next_column].first)
                            {
                            case GridState::kFlaged:
                                --mine_count;
                                break;
                            case GridState::kUnknown:
                                equation[search_states[next_row][next_column]] = 1;
                                break;
                            default:
                                break;
                            }
                        }
                    }
                    *equation.rbegin() = mine_count;
                    gauss_matrix.emplace_back(equation);
                }
                result.emplace_back(unknown_positions, gauss_matrix);
            }
        }
        return result;
    }

    bool SolveOneStep(int row_count, int column_count, Matrix<std::pair<GridState, int>>& states, Timer& timer) {
        if (kPrintDebugInfo) {
            std::clog << "\nSolveOneStep" << std::endl;
        }

        assert((int)states.size() == row_count + 1);
        for (int row = 1; row <= row_count; ++row) {
            assert((int)states[row].size() == column_count + 1);
        }
        vector<Region> regions = Divide(row_count, column_count, states);
        ShuffleVector(regions);

        if (kPrintDebugInfo) {
            std::clog << "regions: " << regions.size() << 'x' << std::endl;

            for (const auto& region: regions) {
                std::clog << "region:\nPositions:";

                for (auto [row, column]: region.first) {
                    std::clog << " (" << row << ", " << column << ')';
                }

                std::clog << std::endl << "Matrix: " << region.second.size() << " x " << region.second[0].size() << std::endl;
                for (const auto& row: region.second) {
                    for (const auto& number: row) {
                        std::clog << (int)number << ' ';
                    }
                    std::clog << std::endl;
                }
            }
        }

        bool result = false;
        for (auto& region: regions) {
            if (timer.TimeIsUp()) {
                if (kPrintDebugInfo) {
                    std::clog << "SolveOneStep Timeout!" << std::endl;
                }
                break;
            }
            vector<std::pair<int, int>> solved = GaussianElimination(region.second);
            if (!solved.empty()) {
                for (auto [index, type]: solved) {
                    auto [row, column] = region.first[index];
                    states[row][column].first = type ? GridState::kFlaged : GridState::kOpened;
                }
                result = true;
                continue;
            }
            auto [legal_count, count] = EnumerateMine(region.second, timer);
            if (!legal_count) {
                continue;
            }
            for (size_t index = 0; index < count.size(); ++index) {
                auto [row, column] = region.first[index];
                if (count[index] == 0) {
                    states[row][column].first = GridState::kOpened;
                } else if (count[index] == legal_count) {
                    states[row][column].first = GridState::kFlaged;
                } else {
                    continue;
                }
                result = true;
            }
        }
        return result;
    }

    bool Solvable(Board board, Timer& timer) {
        if (kPrintDebugInfo) {
            std::clog << "\nSolvable?" << std::endl;
            board.Print();
            board.PrintAll();
            std::clog << std::endl;
        }

        while (!timer.TimeIsUp()) {
            if (board.Solved()) {
                if (kPrintDebugInfo) {
                    std::clog << "Solved!" << std::endl;
                }
                return true;
            }
            Matrix<std::pair<GridState, int>> situation(board.GetSituation());

            if (kPrintDebugInfo) {
                board.Print();
            }
            bool successed = SolveOneStep(board.row_count(), board.column_count(), situation, timer);
            if (!successed) {
                return false;
            }
            board.SetSituation(situation);
        }
        if (kPrintDebugInfo) {
            std::clog << "Solvable Timeout!" << std::endl;
        }
        return false;
    }

    bool Solvable(Board board, int time_limit_milliseconds = 1000) {
        Timer timer(time_limit_milliseconds);
        return Solvable(board, timer);
    }
}

#endif