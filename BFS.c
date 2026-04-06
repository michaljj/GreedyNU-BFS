#include "BFS.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Grid_st
{
    int Xmax;
    int Ymax;
    uint8_t* blockers_p;
    int blockersSize;
};

static const int DX[] = { -1,  1,  0,  0 };
static const int DY[] = {  0,  0, -1,  1 };

typedef struct Bfs_st
{
    int Xmax;
    int Ymax;
    int gridSize;
    const Grid_st* grid_p;
    Point_st* queue_p;
    Point_st* parent_p;
    int* depth_p;
    Point_st* curPath_p;
    Point_st* bestPath_p;
    uint8_t* visited_p;
}Bfs_st;

static Bfs_st* Bfs_p = NULL;
/*-------------------------------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------GRID_HANDLING-----------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------*/

Grid_st *makeGrid(int rows, int columns, Point_st *blockers_p, int blockersSize)
{
    if (rows <= 0 || columns <= 0 || (blockers_p == NULL && blockersSize > 0) || (blockers_p != NULL && blockersSize <= 0) || blockersSize < 0)
    {
        fprintf(stderr, "Failed to create grid - invalid params\n");
        return NULL;
    }
    Grid_st* gridRet = malloc(sizeof(Grid_st));
    if (gridRet == NULL)
    {
        fprintf(stderr, "Failed to alloc grid\n");
        return NULL;
    }
    gridRet->Xmax = columns;
    gridRet->Ymax = rows;
    gridRet->blockersSize = ARRSIZE2BYTE(columns, rows);
    gridRet->blockers_p = calloc(gridRet->blockersSize, sizeof(uint8_t));
    if (gridRet->blockers_p == NULL)
    {
        fprintf(stderr, "Failed to alloc blockers list\n");
        free(gridRet);
        return NULL;
    }
    if (blockersSize == 0)
    {
        return gridRet;
    }
    
    for (int i = 0; i < blockersSize; i++)
    {
        int bx = blockers_p[i].x;
        int by = blockers_p[i].y;
        if (bx < 0 || bx >= gridRet->Xmax || by < 0 || by >= gridRet->Ymax)
        {
            fprintf(stderr, "Warning: blocker (%d,%d) out of bounds, skipped\n", bx, by);
            continue;
        }
        bitSet(gridRet->blockers_p, bx, by, gridRet->Xmax);
    }
    return gridRet;
}

int getGridMaxX(const Grid_st *grid_p)
{
    if (grid_p == NULL)
    {
        fprintf(stderr, "Failed to get MaxX - invalid params\n");
        return -1;
    }
    return grid_p->Xmax;
}

int getGridMaxY(const Grid_st *grid_p)
{
    if (grid_p == NULL)
    {
        fprintf(stderr, "Failed to get MaxY - invalid params\n");
        return -1;
    }
    return grid_p->Ymax;
}

int isBlocked(const Grid_st *grid_p, int x, int y)
{
    if (grid_p == NULL)
    {
        fprintf(stderr, "Failed to check blocker - invalid params\n");
        return -1;
    }
    if (x < 0 || x >= grid_p->Xmax || y < 0 || y >= grid_p->Ymax)
    {
        fprintf(stderr, "Failed to check blocker - out of bounds\n");
        return -1;
    }
    return bitCheck(grid_p->blockers_p, x, y, grid_p->Xmax);
}


void destroyGrid(Grid_st* grid_p)
{
    if (grid_p == NULL)
    {
        return;
    }
    free(grid_p->blockers_p);
    free(grid_p);
}

/*-------------------------------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------GRID_HANDLING_END-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------SOLVE------------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------*/

static void initBfs(const Grid_st* grid_p, int maxSteps)
{
    if (Bfs_p != NULL)
    {
        fprintf(stderr, "Failed to init Bfs - already done\n");
        return;
    }
    Bfs_p = calloc(1, sizeof(Bfs_st));
    if (Bfs_p == NULL)
    {
        fprintf(stderr, "Failed to alloc Bfs\n");
        return;
    }

    Bfs_p->grid_p = grid_p;
    Bfs_p->Xmax = getGridMaxX(grid_p);
    Bfs_p->Ymax = getGridMaxY(grid_p);
    Bfs_p->gridSize = Bfs_p->Xmax * Bfs_p->Ymax;

    Bfs_p->queue_p = malloc(sizeof(Point_st) * Bfs_p->gridSize);
    Bfs_p->parent_p = malloc(sizeof(Point_st) * Bfs_p->gridSize);
    Bfs_p->depth_p = malloc(sizeof(int) * Bfs_p->gridSize);
    Bfs_p->curPath_p = malloc(sizeof(Point_st) * (maxSteps + 1));
    Bfs_p->bestPath_p = malloc(sizeof(Point_st) * (maxSteps + 1));
    Bfs_p->visited_p = malloc(ARRSIZE2BYTE(Bfs_p->Xmax, Bfs_p->Ymax));

    if (!Bfs_p->queue_p || !Bfs_p->parent_p || !Bfs_p->depth_p || !Bfs_p->curPath_p || !Bfs_p->bestPath_p || !Bfs_p->visited_p)
    {
        fprintf(stderr, "Failed to solve - alloc failed\n");
        free(Bfs_p->queue_p);
        free(Bfs_p->parent_p);
        free(Bfs_p->depth_p);
        free(Bfs_p->curPath_p);
        free(Bfs_p->bestPath_p);
        free(Bfs_p->visited_p);
        free(Bfs_p);
        Bfs_p = NULL;
        return;
    }
    
}

static void destroyBfs(void)
{
    if (Bfs_p == NULL)
    {
        return;
    }
    
    free(Bfs_p->queue_p);
    free(Bfs_p->parent_p);
    free(Bfs_p->depth_p);
    free(Bfs_p->curPath_p);
    free(Bfs_p->bestPath_p);
    free(Bfs_p->visited_p);
    free(Bfs_p);
    Bfs_p = NULL;
}

static int countUnvisitedNeighbors(int x, int y)
{
    int count = 0;
    for (int d = 0; d < 4; d++)
    {
        int nx = x + DX[d];
        int ny = y + DY[d];
        if (nx >= 0 && nx < Bfs_p->Xmax && ny >= 0 && ny < Bfs_p->Ymax &&
            !isBlocked(Bfs_p->grid_p, nx, ny) &&
            !bitCheck(Bfs_p->visited_p, nx, ny, Bfs_p->Xmax))
        {
            count++;
        }
    }
    return count;
}

static int runGreedy(Point_st start, Point_st *outPath, int *outPathLen, int maxSteps)
{
    memset(Bfs_p->visited_p, 0, ARRSIZE2BYTE(Bfs_p->Xmax, Bfs_p->Ymax));

    Point_st pos = start;
    int pathLen = 0;
    int uniqueCount = 1;
    int stepsLeft = maxSteps;

    outPath[pathLen++] = pos;
    bitSet(Bfs_p->visited_p, pos.x, pos.y, Bfs_p->Xmax);

    while (stepsLeft > 0)
    {
        for (int i = 0; i < Bfs_p->gridSize; i++)
        {
            //depth -1 means unvisited
            Bfs_p->depth_p[i] = -1;
        }
        int head = 0;
        int tail = 0;

        Bfs_p->depth_p[bitIdx(pos.x, pos.y, Bfs_p->Xmax)] = 0;
        Bfs_p->queue_p[tail++] = pos;

        float bestScore = -1.0f;
        Point_st bestTarget = { -1, -1 };

        while (head < tail)
        {
            Point_st cur = Bfs_p->queue_p[head++];
            int curIdx = bitIdx(cur.x, cur.y, Bfs_p->Xmax);
            int curDepth = Bfs_p->depth_p[curIdx];

            if (curDepth > 0 && !bitCheck(Bfs_p->visited_p, cur.x, cur.y, Bfs_p->Xmax))
            {
                int density = countUnvisitedNeighbors(cur.x, cur.y);
                float score = (float)(density + 1) / (float)curDepth;
                if (score > bestScore)
                {
                    bestScore = score;
                    bestTarget = cur;
                }
            }

            if (curDepth >= stepsLeft)
            {
                continue;
            }

            for (int d = 0; d < 4; d++)
            {
                int nx = cur.x + DX[d];
                int ny = cur.y + DY[d];
                if (nx < 0 || nx >= Bfs_p->Xmax || ny < 0 || ny >= Bfs_p->Ymax)
                {
                    continue;
                }
                int nIdx = bitIdx(nx, ny, Bfs_p->Xmax);
                if (Bfs_p->depth_p[nIdx] != -1 || isBlocked(Bfs_p->grid_p, nx, ny))
                {
                    continue;
                }
                Bfs_p->depth_p[nIdx] = curDepth + 1;
                Bfs_p->parent_p[nIdx] = cur;
                Bfs_p->queue_p[tail++] = (Point_st){ nx, ny };
            }
        }

        if (bestTarget.x == -1)
        {
            break;
        }

        /* count segment len */
        int segLen = 0;
        Point_st trace = bestTarget;
        while (trace.x != pos.x || trace.y != pos.y)
        {
            segLen++;
            trace = Bfs_p->parent_p[bitIdx(trace.x, trace.y, Bfs_p->Xmax)];
        }

        /* reverse segment*/
        int pi = segLen - 1;
        trace = bestTarget;
        Point_st* reversedTrace = malloc(sizeof(Point_st) * segLen);
        if (reversedTrace == NULL)
        {
        fprintf(stderr, "Failed to alloc reversedTrace\n");
        break;
        }
        while (trace.x != pos.x || trace.y != pos.y)
        {
            reversedTrace[pi--] = trace;
            trace = Bfs_p->parent_p[bitIdx(trace.x, trace.y, Bfs_p->Xmax)];
        }

        /* walk the segment */
        for (int i = 0; i < segLen; i++)
        {
            pos = reversedTrace[i];
            if (!bitCheck(Bfs_p->visited_p, pos.x, pos.y, Bfs_p->Xmax))
            {
                bitSet(Bfs_p->visited_p, pos.x, pos.y, Bfs_p->Xmax);
                uniqueCount++;
            }
            outPath[pathLen++] = pos;
            stepsLeft--;
        }
        free(reversedTrace);
    }

    *outPathLen = pathLen;
    return uniqueCount;
}

void solve(const Grid_st *grid_p, int maxSteps)
{
    if (grid_p == NULL || maxSteps <= 0)
    {
        fprintf(stderr, "Failed to solve - invalid params\n");
        return;
    }
    initBfs(grid_p, maxSteps);
    if (Bfs_p == NULL)
    {
        fprintf(stderr, "Failed to solve - Bfs init failed\n");
        return;
    }

    int bestUnique = 0;
    int bestLen = 0;

    /* Count unblocked cells */
    int totalUnblocked = 0;
    for (int y = 0; y < Bfs_p->Ymax; y++)
    {
        for (int x = 0; x < Bfs_p->Xmax; x++)
        {
            if (!isBlocked(Bfs_p->grid_p, x, y))
            {
                totalUnblocked++;
            }
        }
    }
    int maxPossible = (maxSteps < totalUnblocked) ? maxSteps : totalUnblocked;

    /* Try every good cell */
    for (int sy = 0; sy < Bfs_p->Ymax; sy++)
    {
        for (int sx = 0; sx < Bfs_p->Xmax; sx++)
        {
            if (isBlocked(Bfs_p->grid_p, sx, sy))
            {
                continue;
            }
            int pathLen = 0;
            int uniqueCount = runGreedy((Point_st){ sx, sy }, Bfs_p->curPath_p, &pathLen, maxSteps - 1);

            if (uniqueCount > bestUnique)
            {
                bestUnique = uniqueCount;
                bestLen = pathLen;
                memcpy(Bfs_p->bestPath_p, Bfs_p->curPath_p, sizeof(Point_st) * bestLen);
            }
            if(bestUnique >= maxPossible) break;
        }
        if(bestUnique >= maxPossible) break;
    }

    /* Print result */
    for (int i = 0; i < bestLen; i++)
    {
        if (i > 0)
        {
            printf(", ");
        }
        printf("(%d,%d)", Bfs_p->bestPath_p[i].x + 1, Bfs_p->bestPath_p[i].y + 1);
    }
    printf(", %d\n", bestUnique);
    destroyBfs();
}

/*-------------------------------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------------SOLVE_END--------------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------------------------------------------------------------------------------*/