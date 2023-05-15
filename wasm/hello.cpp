// hello.cpp
#include <iostream>
#include <vector>
#include <ranges>
#include <algorithm>
#include <numeric>
#include <format>
#include <wasm_simd128.h>

extern "C"
{
    int sum();
}

auto funct()
{
    std::vector<int> tester;
    tester.resize(10);
    std::iota(tester.begin(), tester.end(), 0);
    return tester;
}

int sum()
{
    auto range = funct();
    return std::accumulate(range.begin(), range.end(), 0);
}



int main() {
    //std::clog << "Hello, WebAssemblly!\n";

    //auto print = [](auto e){ std::cout << std::format("{}, ", e); };

    //std::ranges::for_each(funct(), print);

    return 0;
}