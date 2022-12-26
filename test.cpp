#include <bits/stdc++.h>

#include "src/ms_lib.h"

int main() {
    auto a = ms_algo::GetMilliseconds();
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    auto b = ms_algo::GetMilliseconds();
    std::cout << b - a << std::endl;
    return 0;
}