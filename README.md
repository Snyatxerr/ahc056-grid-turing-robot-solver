# AHC056 Solver

C++17 solver for AtCoder Heuristic Contest 056, **Grid Turing Robot**.

## Overview

AHC056 asks you to move a robot through a sequence of target cells by assigning colors and state-transition rules to the grid.

This program constructs a route through the required targets and converts that route into the transition rules used by the robot.

Problem: https://atcoder.jp/contests/ahc056

## Approach

For each pair of consecutive targets, the solver searches among shortest paths and uses additional costs to prefer routes whose cells can be reused with simple behavior.

After the route is fixed, the observed straight/turn behavior is converted into color and state transitions. Short left/right turn patterns share compact DFA states, while irregular cases use dedicated fallback states.

More detailed notes are available in [`docs/approach.md`](docs/approach.md).

## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
```

Or directly with g++:

```bash
g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic src/main.cpp -o ahc056_solver
```

The solver reads one instance from standard input and writes the solution to standard output.

## License

MIT License. See [`LICENSE`](LICENSE).
