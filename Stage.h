//Stage.h

#ifndef __STAGE__
#define __STAGE__
#include <ncurses.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <vector>
#include <fstream>

#include "Something.h"


#define UP KEY_UP
#define DOWN KEY_DOWN
#define RIGHT KEY_RIGHT
#define LEFT KEY_LEFT
#define ENTER 10
#define ESC 27
#define PAUSE 112
#define RESUME 114


#define EMPTY 0
#define WALL 1
#define IMMUNE_WALL 2
#define SNAKE_HEAD 3
#define SNAKE_BODY 4
#define GROWTH_ITEM 5
#define POISON_ITEM 6
#define GATE 8
#define SPEED_SLOW 7
#define NUMBER_OF_ITEMS 3
#define MAGIC_GATE 12
#define MAGIC_EXIT 13

using namespace std;

class Stage
{
public:
    
    Stage();
    ~Stage();

    
    void screenLock();

    
    string menu();
    void play();
    void help();

    
    int getStageNum() const { return STAGE_NUM; }
    int getMapRow() const { return MAP_ROW; }
    int getMapCol() const { return MAP_COL; }

    
    void setMap();
    void copyMap(int nStage);
    void drawMap();

    
    void appearItem();
    void appearGate();
    void appearMagicGate();
    void disappearItem();
    void disappearGate();
    void disappearMagicGate();

    
    void makeSnake();
    void moveSnake();
    void enterGate(Something *head);
    void enterMagicGate(Something *head);
    int findRoot(Something *gate);
    void eatItem(int item);

    
    void setMission();
    bool isMissionClear();
    bool checkGameOver() { return finish; }
    void gameOver();
    void alert(int posY, int posX, const string msg, bool stopFlag);

private:
    
    WINDOW *game, *score, *mission, *info;
    WINDOW *manual, *description, *scrollBar;

    
    int stat[5];

    
    char chkMission[4];
    int statMission[4];

    
    const int SNAKE_MAX_LENGTH = 10;
    Something *Bam;
    Something *gate1;
    Something *gate2;
    Something *magicGate1;
    Something *magicGate2;

    
    const int STAGE_NUM = 4,
              MAP_ROW = 30,
              MAP_COL = 50,
              ROW_END = MAP_ROW - 1,
              COL_END = MAP_COL - 1;
    const string itemIndex = "  X0O+-                ";

    
    int ***stage, **map;

    
    int dir, y, x,
        menuLastFocus, optLastFocus,
        level, speed, tcount,
        timeoutMs, msTime;
    const int speedMs[5] = {500,250,200,100,70} ;
    const int msDiv[5] = {2,4,5,10,20}; 

    
    string manualTitle, menuTitle, shorTitle;
    string menuTxt[4], shorTxt[7];
    int sizeY, sizeX,
        startY, startX;
    int desSizeY, desSizeX,
        desStartY, desStartX;
    int txtLines, hidTxtLen, scrollBarLen;

    vector<string> manualTxt;

    vector<pair<int, int>> itemPos, gatePos, magicGatePos;
    bool chkEnter, chkMagicEnter, finish;
};
#endif