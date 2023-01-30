#ifndef MINEALGO_MS_SOLVE_H_
#define MINEALGO_MS_SOLVE_H_

#include <algorithm>
#include <atomic>
#include <cassert>
#include <iostream>
#include <utility>
#include <vector>

#include "ms_board.h"
#include "ms_grid.h"
#include "ms_lib.h"

namespace ms_algo {

    vector<int> GaussianElimination(Matrix<double>& matrix) {
        vector<int> result;
        for (size_t current = 0; current < matrix.size(); ++current) {
            int max_row = current;
            for (size_t row = max_row + 1; row < matrix.size(); ++row) {
                if (Greater(matrix[row][current], matrix[max_row][current])) {
                    max_row = row;
                }
            }
            if ((int)current != max_row) {
                std::swap(matrix[current], matrix[max_row]);
            }

            for (size_t row = 0; row < matrix.size(); ++row) {
                if (row != current && NotZero(matrix[row][current])) {
                    matrix[row] -= matrix[current] * (matrix[current][current] / matrix[row][current]);
                }
            }

            if (IsZero(matrix[current][current])) {
                std::cerr << "Gaussian Elimination Error" << std::endl;
                assert(false);
            }
            matrix[current] /= matrix[current][current];

            bool solved = true;
            for (size_t column = 0; column + 1 < matrix[current].size(); ++column) {
                if (column != current && NotZero(matrix[current][column])) {
                    solved = false;
                    break;
                }
            }
            if (solved) {
                result.push_back(current);
            }
        }
        return result;
    }

    std::pair<int64_t, vector<int64_t>> EnumerateMine(const Matrix<double>& matrix, std::atomic_bool& time_up) {
        int variable_count = (*matrix.begin()).size() - 1;
        int unfree_variable_count = matrix.size();
        int free_variable_count = variable_count - unfree_variable_count;
        int64_t legal_count = 0;
        vector<int64_t> count(variable_count, 0);
        for (int64_t situation = (1 << free_variable_count) - 1; situation >= 0; --situation) {
            if (time_up) {
                return {};
            }
            vector<double> unfree_variable(unfree_variable_count);
            bool illegal = false;
            for (size_t current_variable = 0; current_variable < matrix.size(); ++current_variable) {
                unfree_variable[current_variable] = matrix[current_variable].back();
                for (int index = 0; index < free_variable_count; ++index) {
                    unfree_variable[current_variable] -= (situation >> index & 1) * matrix[current_variable][unfree_variable_count + index];
                }
                if (!Equal(unfree_variable[current_variable], 0.0) && !Equal(unfree_variable[current_variable], 1.0)) {
                    illegal = true;
                    break;
                }
            }
            if (illegal) {
                continue;
            }
            ++legal_count;
            for (int index = 0; index < unfree_variable_count; ++index) {
                if (Equal(unfree_variable[index], 1.0)) {
                    ++count[index];
                }
            }
            for (int index = 0; index < free_variable_count; ++index) {
                if ((situation >> index & 1)) {
                    ++count[unfree_variable_count + index];
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
                    gauss_matrix.emplace_back(std::move(equation));
                }
                result.emplace_back(std::move(unknown_positions), std::move(gauss_matrix));
            }
        }
        return result;
    }

    bool SolveOneStep(int row_count, int column_count, Matrix<std::pair<GridState, int>>& states, std::atomic_bool& time_up) {
        assert((int)states.size() == row_count + 1);
        for (int row = 1; row <= row_count; ++row) {
            assert((int)states[row].size() == column_count + 1);
        }
        vector<Region> regions = std::move(Divide(row_count, column_count, states));
        ShuffleVector(regions);
        bool result = false;
        for (auto& region: regions) {
            if (time_up) {
                break;
            }
            vector<int> solved = std::move(GaussianElimination(region.second));
            if (!solved.empty()) {
                for (int index: solved) {
                    auto [row, column] = region.first[index];
                    if (Equal(region.second[index].back(), 1.0)) {
                        states[row][column].first = GridState::kFlaged;
                    } else {
                        states[row][column].first = GridState::kOpened;
                    }
                }
                result = true;
                continue;
            }
            auto [legal_count, count] = EnumerateMine(region.second, time_up);
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

    bool Solvable(Board board, std::atomic_bool& time_up) {
        while (!time_up) {
            if (board.Solved()) {
                return true;
            }
            Matrix<std::pair<GridState, int>> situation(std::move(board.GetSituation()));
            bool successed = SolveOneStep(board.row_count(), board.column_count(), situation, time_up);
            if (!successed) {
                return false;
            }
            board.SetSituation(situation);
        }
        return false;
    }
}

#endif