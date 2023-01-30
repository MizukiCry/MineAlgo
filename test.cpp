#include <cstdio>
#include <iostream>

#include "src/minealgo.h"

/*
Main functions:

- ms_algo::Generate()
- ms_algo::Solvable()
- ms_algo::SolveOneStep()

*/

int main() {

	// Generate a board randomly.
	auto [result, board] = ms_algo::Generate(10, 10, 3, 3, ms_algo::kNormal);

	if (!result) {
		std::cout << "Failed" << std::endl;
		return 0;
	}
	board.Print();


	return 0;
}