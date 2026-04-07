# Grid Traversal — Greedy Nearest-Unvisited BFS

C solution to a grid traversal problem: given an N×M grid with blocked squares and a movement budget, find a path that visits the maximum number of unique squares.

## Approach

A greedy heuristic is run from every valid starting cell. Each iteration:

1. **BFS** expands from the current position up to the remaining step budget
2. Each reachable unvisited cell is scored: `(unvisited_neighbours + 1) / distance`
3. The agent walks to the highest-scoring target via the BFS parent chain
4. Repeat until the budget is exhausted or no reachable unvisited cells remain

The best result across all starting points is printed.

**Time complexity:** O((N×M)³) — meets the problem requirement.

## Design Highlights

- `Grid_st` is an **opaque type** — callers hold a pointer but cannot access fields directly, enforcing immutability after construction
- Blockers stored as a **packed bitfield** (`uint8_t[]`) — one bit per cell, cache-friendly
- `depth_p` (`int[]`, -1 = unseen) serves as both the **BFS distance map and visited marker** — one array, two roles
- `queue_p` is pre-allocated once and reused across all BFS iterations — no per-iteration heap traffic (except path reconstruction)
- All allocation failures propagate cleanly with no memory leaks

## Build

```
make
./main
```

Requires gcc with C99 support (`-std=c99`).

## Example Output
Test 5: 8x8, 25 steps as in example, given blockers:
{0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {3,3}, {3,4}, {5,5}, {6,6}
Output:
```
(1,1), (2,1), (3,1), (4,1), (5,1), (6,1), (6,2), (7,2), (7,3),
(7,4), (6,4), (6,5), (5,5), (5,6), (5,7), (4,7), (3,7), (2,7),
(2,6), (2,5), (1,5), (1,4), (2,4), (3,4), (3,5), 25
```

## Files

| File | Description |
|------|-------------|
| `BFS.h` | Public API — grid construction/destruction, `solve` |
| `BFS.c` | Implementation — grid internals, BFS, greedy solver |
| `main.c` | Test cases |

## See also
[GridTraversal](https://github.com/michaljj/GridTraversal) — an earlier beam search
solver for the non-revisiting variant of the same grid traversal problem.
