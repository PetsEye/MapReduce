# mReduce

A MapReduce word counter in C++, been building it step by step following an AI guide to learn systems -> this is just the beginning :). The input is `divineComedy.txt`, and the output is the 20 most frequent words.

## Status

- **Phase 1:** sequential map, shuffle, and reduce -> completed that.
- **Phase 2:** multithreaded map. -> completed that. The file is split into byte ranges on whitespace boundaries, and each range is mapped on its own thread. Results match the sequential version for 1 to 16 chunks.
- **Phase 3:** process-based workers on one machine. Not started.
- **Phase 4:** workers on a Raspberry Pi over TCP. Not started.

## Testing

- Chunk output combined and compared against whole-file output, for 1, 2, 4, 7, and 16 chunks.
- AddressSanitizer and UndefinedBehaviorSanitizer runs with no reports.
- ThreadSanitizer run with no warnings.
- Timing in `bench.cpp` (median of 10).

## Build

```
clang++ -std=c++17 -O2 -o mapreduce reduce2.cpp
./mapreduce
```
