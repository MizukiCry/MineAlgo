#include <cassert>
#include <cstdio>
#include <iostream>
#include <vector>

#include "src/minealgo.h"

/*
Main functions:

- ms_algo::Generate()
- ms_algo::Solvable()
- ms_algo::SolveOneStep()

*/

int main() {
	{
		// Generate a board randomly.
		auto [result, board] = ms_algo::Generate(3, 3, 2, 2, ms_algo::GenerateType::kNormal);


		if (!result) {
			std::cout << "Failed" << std::endl;
			return 0;
		}
		board.PrintAll();

		board.Open(2, 2);
		board.Print();
	}

	std::cout << "\n--------------------\n" << std::endl;

	{
		auto [result, board] = ms_algo::Generate(5, 5, 2, 2, ms_algo::GenerateType::kSolvable, 1500, 1);

		if (!result) {
			std::cout << "Failed" << std::endl;
			return 0;
		}
		board.PrintAll();

		board.Open(3, 3);
		board.Print();
		assert(ms_algo::Solvable(board));
	}

	return 0;
}