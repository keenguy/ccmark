//
// Created by yong gu on 01/10/2018.
//

#include <benchmark/benchmark.h>
#include "../src/ccmark.h"
#include <string>
#include <fstream>
#include <sstream>

static void BM_MDCreation(benchmark::State& state) {
    for (auto _ : state)
        ccm::CCMark md;
}
// Register the function as a benchmark
BENCHMARK(BM_MDCreation);

// Define another benchmark
static void BM_Mark(benchmark::State& state) {
    ccm::CCMark md;
    std::ifstream in("gbench.md");
    std::stringstream ss;
    ss<<in.rdbuf();
    std::string src {ss.str()};
    for (auto _ : state)
        md.render(src);
}
BENCHMARK(BM_Mark);

BENCHMARK_MAIN();