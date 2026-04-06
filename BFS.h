#ifndef BFS_H
#define BFS_H
#include <stdint.h>

#define ARRSIZE2BYTE(Xmax, Ymax) (((Xmax) * (Ymax) + 7) / 8)

static inline int bitIdx(int x, int y, int Xmax)
{
    return y * Xmax + x;
}

static inline void bitSet(uint8_t *arr, int x, int y, int Xmax)
{
    int idx = bitIdx(x, y, Xmax);
    arr[idx / 8] |= (uint8_t)(1 << (idx % 8));
}

static inline int bitCheck(const uint8_t *arr, int x, int y, int Xmax)
{
    int idx = bitIdx(x, y, Xmax);
    return arr[idx / 8] & (1 << (idx % 8));
}

static inline void bitClear(uint8_t *arr, int x, int y, int Xmax)
{
    int idx = bitIdx(x, y, Xmax);
    arr[idx / 8] &= (uint8_t)~(1 << (idx % 8));
}

typedef struct Point_st
{
    int x;
    int y;
}Point_st;

typedef struct Grid_st Grid_st;

/*-------------------------------------------------------GRID_HANDLING-----------------------------------------------------------------------------*/

Grid_st* makeGrid(int rows, int columns, Point_st* blockers_p, int blockersSize);
int getGridMaxX(const Grid_st* grid_p);
int getGridMaxY(const Grid_st* grid_p);
int isBlocked(const Grid_st* grid_p, int x, int y);
void destroyGrid(Grid_st* grid_p);

/*-------------------------------------------------------GRID_HANDLING_END-------------------------------------------------------------------------*/

/*-------------------------------------------------------SOLVE------------------------------------------------------------------------------------*/

void solve(const Grid_st* grid_p, int maxSteps);

/*-------------------------------------------------------SOLVE_END--------------------------------------------------------------------------------*/


#endif /*BFS_H*/