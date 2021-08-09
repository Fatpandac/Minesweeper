#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

//#define DEBUG

typedef struct{
    int isMine;
    int isDisp;
    int countMine;
}PLACE;

typedef struct{
    int posX;
    int posY;
}CURSOR;

enum{
    UP     = 72,
    DOWM   = 80,
    LEFT   = 75,
    RIGHT  = 77,
    SPACE  = 32,
    Q_QUIT = 81,
}KEY;

int mapX = 0,mapY = 0,totalMine = 0;

void InitMapInfo(int level)
{
    switch (level) {
        case 1:
            mapX = 8;
            mapY = 8;
            totalMine = 10;
            break;
        case 2:
            mapX = 16;
            mapY = 16;
            totalMine = 40;
            break;
        case 3:
            mapX = 30;
            mapY = 16;
            totalMine = 99;
            break;
        default:
            mapX = 8;
            mapY = 8;
            totalMine = 10;
            break;
    }
}

PLACE **InitMap()
{
    PLACE **map;
    map = (PLACE **)malloc(mapY * sizeof(PLACE*));
    for (int i = 0;i < mapY;i++)
    {
        map[i] = (PLACE *)malloc(mapX * sizeof(PLACE));
    }
    for (int i = 0;i < mapY;i++)
    {
        for (int j = 0;j < mapX;j++)
        {
            map[i][j].isMine = 0;
            map[i][j].isDisp = 0;
            map[i][j].countMine = 0;
        }
    }
    return map;
}

void InitMine(PLACE **map)
{
    int sumMine = totalMine;
    int mineX = 0,mineY = 0;
    srand((unsigned)time(NULL));
    while (sumMine--)
    {
        mineY = rand() % mapY;
        mineX = rand() % mapX;
        (map[mineY][mineX].isMine == -1) ? (sumMine++) : (map[mineY][mineX].isMine = -1);
    }
}

void DispMapInfo(PLACE **map)
{
    for (int i = 0;i < mapY;i++)
    {
        for (int j = 0;j < mapX;j++)
        {
            printf("%2d ",map[i][j].countMine);
        }
        printf("  |  ");
        for (int j = 0;j < mapX;j++)
        {
            printf("%2d ",map[i][j].isMine);
        }
        printf("\n");
    }
    printf("\n");
    for (int i = 0;i < mapY;i++)
    {
        for (int j = 0;j < mapX;j++)
        {
            printf("%2d ",map[i][j].isDisp);
        }
        printf("\n");
    }
}

void CountAroundMine(PLACE **map,int placeY,int placeX)
{
    for (int i = -1;i < 2;i++)
    {
        if (placeY + i < 0 || placeY + i >= mapY) continue;
        for (int j = -1;j < 2;j++)
        {
            if ((placeX + j < 0 || placeX + j >= mapX) || ((placeY - placeX) == 0))continue;
            if (map[placeY + i][placeX + j].isMine == -1) map[placeY][placeX].countMine++;
        }
    }
}

void InitAroundMineInfo(PLACE **map)
{
    for (int i = 0;i < mapY;i++)
    {
        for (int j = 0;j < mapX;j++)
        {
            if (map[i][j].isMine != -1) CountAroundMine(map,i,j);
        }
    }
}

void DispMap(PLACE **map,CURSOR *cursor)
{
    system("cls");
    for (int i = 0;i < mapY;i++)
    {
        for (int j = 0;j < mapX;j++)
        {
            if (cursor->posY == i && cursor->posX == j){
                printf("\33[36m  □\33[0m");
            }else if (!map[i][j].isDisp){
                printf("  □");    
            }else if (map[i][j].countMine == 0) {
                (map[i][j].isMine) ? printf("\33[31m%3d\33[0m",-1) : printf("   ");
            }else if (map[i][j].countMine != 0) {
                printf("%3d",map[i][j].countMine);
            }
        }
        printf("\n");
    }
}

void Sweeper(PLACE **map,int y,int x)
{
    int aroundPlaceStack[20][2],top = 0;
    aroundPlaceStack[top][0] = y;
    aroundPlaceStack[top++][1] = x;
    do {
        y = aroundPlaceStack[--top][0];
        x = aroundPlaceStack[top][1];
        for (int i = -1;i < 2;i++)
        {
            for (int j = -1;j < 2;j++)
            {
                if (((i == -1 || i == 1) && (j == 1 || j == -1)) || i - j == 0) continue;
                if ((y + i < 0 || y + i >= mapY) || (x + j < 0 || x + j >= mapX)) continue;
                if (!map[y + i][x + j].isDisp)
                {
                    if (!map[y + i][x + j].isMine)
                    {
                        map[y + i][x + j].isDisp = 1;   
                        if (map[y + i][x + j].countMine == 0)
                        {
                            aroundPlaceStack[top  ][0] = y + i;
                            aroundPlaceStack[top++][1] = x + j;
                        }
                    }
                }
            }
        }
    }while(top);
}

int SweeperPlace(PLACE **map,int y,int x)
{
    if (map[y][x].isDisp)
    {
        return 0;
    }else{
        map[y][x].isDisp = 1;
        if (map[y][x].isMine) return 1;
        if (map[y][x].countMine != 0)
        {
            return 0;
        }else{
            Sweeper(map,y,x);
        }
    }
    return 0;
}

int ChooseSweeperPlace(PLACE **map,CURSOR *cursor)
{
    int input;
    while (1)
    {
        input = _getch();
        if (input == 224) input = _getch();
        switch (input)
        {
            case UP:
                cursor->posY -= (cursor->posY - 1 >= 0);
                break;
            case DOWM:
                cursor->posY += (cursor->posY + 1 < mapY);
                break;
            case LEFT:
                cursor->posX -= (cursor->posX - 1 >= 0);
                break;
            case RIGHT:
                cursor->posX += (cursor->posX + 1 < mapX);
                break;
            case SPACE:
                return (SweeperPlace(map,cursor->posY,cursor->posX));
            case Q_QUIT:
                exit(0);
        }
        DispMap(map,cursor);
    }
}

int IsWin(PLACE **map)
{
    for (int i = 0;i < mapY;i++)
    {
        for(int j = 0;j < mapX;j++)
        {
            if (!map[i][j].isDisp && !map[i][j].isMine)
            {
                return 0;
            }
        }
    }
    return 1;
}

void DispAllPlace(PLACE **map)
{
    for (int i = 0;i < mapY;i++)
    {
        for (int j = 0;j < mapX;j++)
        {
            if (map[i][j].isMine) map[i][j].isDisp = 1;
        }
    }
}

void FreeMap(PLACE **map)
{
    for (int i = 0;i < mapX;i++)
    {
        free(map[i]);
    }
    free(map);
}

int main(int agrc,char *argv[])
{
    int level = 1;
    PLACE **map;
    CURSOR cursor = {
        .posX = 0,
        .posY = 0,
    };
    printf("Please choose a level from 1 to 3:");
    level = getchar() - '0';
    InitMapInfo(level);
    map = InitMap();
    InitMine(map);
    InitAroundMineInfo(map);
    printf("\33[?25l");
    while (1)
    {
#ifdef DEBUG
        DispMapInfo(map);
#else
        DispMap(map,&cursor);
#endif
        if (ChooseSweeperPlace(map,&cursor))
        {
            DispAllPlace(map);
            DispMap(map,&cursor);
            printf("Game Over");
            break;
        }else if (IsWin(map)){
            DispAllPlace(map);
            DispMap(map,&cursor);
            printf("Game Over");
            break;
        }
    }
    FreeMap(map);
    printf("\33[?25H");
    return 0;
}
