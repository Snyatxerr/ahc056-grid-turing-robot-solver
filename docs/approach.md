# Approach

## 1. Route construction

For every consecutive target pair, the solver first computes unweighted grid distances from both ends with BFS. A Dijkstra search then runs over `(row, column, previous-direction)` states while enforcing

```text
dist_from_start[current] + 1 + dist_to_goal[next] = segment_shortest_distance
```

for every transition. Therefore the heuristic cost only chooses **which shortest path** to use; it does not increase the number of moves for the segment.

The extra cost favors stable semantic roles for cells. Reusing a historically straight-through cell as a turning cell (or the reverse) receives a large penalty. Smaller penalties prefer straight movement and lightly discourage repeatedly routing through already-used cells.

## 2. Event classification

After all target-to-target segments are concatenated, every movement step is classified relative to the previous direction as one of:

- straight
- left turn
- right turn
- backtrack

Each visited grid cell is then summarized from the sequence of events observed on that cell.

## 3. Compact cells and special cells

Cells are split into three categories.

### Straight cells

Cells that only need straight-through behavior require no turn history.

### Short L/R-pattern cells

A cell used only for left/right turns, with at most four such events, is represented by its remaining L/R suffix. All suffixes up to length four form a small DFA:

```text
"LRR" -> consume L -> "RR" -> consume R -> "R" -> consume R -> ""
```

This shares states between cells with equal remaining behavior.

### Special cells

Cells with a backtrack, a mixture of straight and turn behavior, or too many turns are deliberately not compressed. They receive visit-indexed colors instead. This fallback keeps the common representation compact while retaining an escape hatch for irregular cases.

## 4. Rule generation

The solver walks the constructed route once more while tracking the current color of each cell and a four-state direction encoding. For each `(color, internal state)` pair encountered, it emits the corresponding repaint, state transition, and absolute movement direction.

## Trade-offs

This is a heuristic constructive solution. The large role-conflict penalty is intentionally simple and was chosen to reduce difficult-to-compress cells rather than to optimize a smooth mathematical objective. The DFA bound of four turn events is likewise a pragmatic compression boundary.

## Possible improvements

- benchmark parameter choices over a fixed seed set
- optimize the route and automaton jointly instead of sequentially
- replace the hard role-conflict penalty with a learned or adaptive cost
- search for larger shared automata across special-cell visit sequences
- add a local simulator-based regression suite for generated rules
