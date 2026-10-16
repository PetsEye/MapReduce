#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>

#define main reduce2_main
#include "reduce2.cpp"
#undef main

static size_t sink = 0;

template <typename F>
double median_ms(F f, int runs)
{
    std::vector<double> times;
    for (int r = 0; r < runs; ++r)
    {
        auto t0 = std::chrono::steady_clock::now();
        f();
        auto t1 = std::chrono::steady_clock::now();
        times.push_back(std::chrono::duration<double, std::milli>(t1 - t0).count());
    }
    std::sort(times.begin(), times.end());
    return times[times.size() / 2];
}

int main()
{
    const std::string filename = "divineComedy.txt";
    const int runs = 10;

    map(filename);

    std::cout << "map sequential:         " << median_ms([&] { sink += map(filename).size(); }, runs) << " ms\n";
    for (int k : {1, 2, 4, 8})
    {
        std::cout << "map_parallel k=" << k << ":    " << median_ms([&] { sink += map_parallel(filename, k).size(); }, runs) << " ms\n";
    }

    std::cout << "full pipeline seq:      " << median_ms([&] { sink += group_and_reduce(map(filename)).size(); }, runs) << " ms\n";
    std::cout << "full pipeline k=4:      " << median_ms([&] { sink += group_and_reduce(map_parallel(filename, 4)).size(); }, runs) << " ms\n";

    std::cout << "(sink " << sink << ")\n";
}
