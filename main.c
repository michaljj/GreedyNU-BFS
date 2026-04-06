#include "BFS.h"
#include <stdio.h>
#include <stdlib.h>

static void test1(void)
{
    printf("Test 1: 4x4 no blockers, 16 steps\n");
    Grid_st *grid = makeGrid(4, 4, NULL, 0);
    solve(grid, 16);
    destroyGrid(grid);
    printf("Expected: 16 unique cells\n\n");
}

static void test2(void)
{
    printf("Test 2: 5x5 with blockers, 10 steps\n");
    Point_st blockers[] = { {1,1}, {2,1}, {1,2}, {3,3} };
    Grid_st *grid = makeGrid(5, 5, blockers, 4);
    solve(grid, 10);
    destroyGrid(grid);
    printf("Expected: 10 unique cells\n\n");
}

static void test3(void)
{
    printf("Test 3: 1x1 grid, 5 steps\n");
    Grid_st *grid = makeGrid(1, 1, NULL, 0);
    solve(grid, 5);
    destroyGrid(grid);
    printf("Expected: 1 unique cell\n\n");
}

static void test4(void)
{
    printf("Test 4: invalid params grid\n");
    Grid_st *grid = makeGrid(0, 4, NULL, 0);
    if (grid == NULL)
    {
        printf("makeGrid correctly returned NULL\n\n");
    }
}

static void test5(void)
{
    printf("Test 5: 8x8, 25 steps as in example\n");
    Point_st blockers[] = { {0,2}, {1,2}, {2,2}, {3,2}, {4,2}, {3,3}, {3,4}, {5,5}, {6,6}};
    Grid_st *grid = makeGrid(8, 8, blockers, (sizeof(blockers) / sizeof(Point_st)));
    solve(grid, 25);
    destroyGrid(grid);
}

int main(void)
{
    test1();
    test2();
    test3();
    test4();
    test5();
    return 0;
}
