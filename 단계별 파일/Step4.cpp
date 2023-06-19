//Stage.cpp
//Step4 게이트까지 구현
#include "Stage.h"

Stage::Stage()
{
    // std::chrono::steady_clock::time_point lastSpeedSlowTime;
    srand((unsigned)time(0)); //난수 생성을 위해 시드값 설정 
    initscr(); //curse라이브러리 초기화 
    keypad(stdscr, TRUE); //키보드 입력 모드 true->입력가능상태 
    cbreak(); //입력을 줄단위로 처리하지 않고 문자로 입력받도록!
    noecho(); //키 입력시 입력한 문자가 화면에 출력되지 않도록!

    start_color(); //컬러모드를 사용하기 위해 
    if (has_colors() == FALSE) //터미널에서 컬러를 지원하지 않는 경우 에외처리
    {
        endwin();
        printf("Your terminal does not support color\n");
        exit(1);
    }
    if (init_color(COLOR_BLUE, 0, 0, 300) == ERR) //터미널이 COLOR_BLUE를 사용자 정의로 변경할 수 없는 경우 
    {
        printw("Your terminal cannot change the color definitions\n");
        printw("press any key to continue...\n");
        getch(); // 사용자로부터 입력 대기, 사용자가 아무 키나 누를때까지 일시정지 
        moveSnake();
    }
    
    init_pair(EMPTY, COLOR_WHITE, COLOR_BLACK);
    init_pair(WALL, COLOR_BLACK, COLOR_WHITE);
    init_pair(IMMUNE_WALL, COLOR_BLACK, COLOR_WHITE);
    init_pair(SNAKE_HEAD, COLOR_YELLOW, COLOR_BLACK);
    init_pair(SNAKE_BODY, COLOR_GREEN, COLOR_BLACK);
    init_pair(GROWTH_ITEM, COLOR_WHITE, COLOR_BLUE);
    init_pair(POISON_ITEM, COLOR_BLACK, COLOR_MAGENTA);
    init_pair(SPEED_SLOW, COLOR_BLACK, COLOR_RED);
    init_pair(GATE, COLOR_CYAN, COLOR_CYAN);
    init_pair(10, COLOR_YELLOW, COLOR_BLACK);
    init_pair(11, COLOR_BLACK, COLOR_WHITE);

    init_pair(12, COLOR_MAGENTA, COLOR_YELLOW);
    init_pair(13, COLOR_WHITE, COLOR_WHITE);
    
    menuLastFocus = 0,
    speed = 2,
    optLastFocus = speed - 1,
    tcount = 0;
    manualTitle = "< MANUAL >",
    menuTitle = "< MENU >",
    menuTxt[0] = " - PLAY: Start the game",
    menuTxt[1] = " - HELP: Manual of the game",
    menuTxt[2] = " - EXIT: Exit the game";
    shorTitle = "< SHORTCUTS >";
    shorTxt[0] = " - Arrow up(^): MOVE UP",
    shorTxt[1] = " - Arrow down(v): MOVE DOWN",
    shorTxt[2] = " - Arrow left(<): MOVE LEFT",
    shorTxt[3] = " - Arrow right(>): MOVE RIGHT",
    shorTxt[4] = " - 'p': GAME PAUSE",
    shorTxt[5] = " - 'r': GAME RESUME",
    shorTxt[6] = " - 'esc': BACK TO THE MAIN MENU";
}

Stage::~Stage()
{
    delwin(scrollBar);
    delwin(description);
    delwin(manual);
    delwin(info);
    delwin(mission);
    delwin(score);
    delwin(game);
    endwin();
}

void Stage::screenLock()
{
    cout << "\e[3;240;120t";
    cout << "\e[8;40;120t";
    system("resize -s 40 120");
    y = 40, x = 120;
    mvprintw(y - 1, 0, "SnakeGame ver. 1.0");
    sizeY = y / 1.5,
    sizeX = x / 1.5,
    startY = y / 2 - sizeY / 2,
    startX = x / 2 - sizeX / 2,
    desSizeY = sizeY - 6,
    desSizeX = sizeX - 6,
    desStartY = startY + 3,
    desStartX = startX + 3,
    txtLines = 26,
    hidTxtLen = txtLines - desSizeY > 0 ? txtLines - desSizeY : 0,
    scrollBarLen = desSizeY - hidTxtLen;
}

string Stage::menu()
{
    clear();
    screenLock();
    curs_set(0);
    string txt[4];
    txt[0] = "[ SNAKE GAME ]";
    int focus = menuLastFocus;
    level = 0;
    while (1)
    {
        if (!focus)
            focus = 300;
        txt[1] = "PLAY";
        txt[2] = "HELP";
        txt[3] = "EXIT";
        attron(COLOR_PAIR(10));
        mvprintw(y / 2 - 2, x / 2 - txt[0].length() / 2, txt[0].c_str());
        attroff(COLOR_PAIR(10));
        for (int i = 1; i < sizeof(txt) / sizeof(txt[0]); i++)
        {
            if (i == abs(focus % 4 + 1))
            {
                attron(COLOR_PAIR(11));
                mvprintw(y / 2 + i, x / 2 - (txt[i].length() / 2), txt[i].c_str());
                attroff(COLOR_PAIR(11));
            }
            else
                mvprintw(y / 2 + i, x / 2 - (txt[i].length() / 2), txt[i].c_str());
        }
        switch (getch())
        {
        case UP:
            focus--;
            break;
        case DOWN:
            focus++;
            break;
        case ENTER:
            menuLastFocus = focus;
            return txt[abs(focus % 4 + 1)];
        }
    }
    return NULL;
}

void Stage::play()
{
    screenLock();
    setMap();
    int n;
    for (int i = 0; i < STAGE_NUM; i++)
    {
        timeoutMs = speedMs[speed - 1];
        msTime = n = 0;
        dir = LEFT;
        copyMap(i);
        setMission(); 
        makeSnake();
        appearGate();
        appearMagicGate();
        drawMap();
        while (1)
        {
            switch (getch())
            {
            case LEFT:
                dir = LEFT;
                break;
            case UP:
                dir = UP;
                break;
            case RIGHT:
                dir = RIGHT;
                break;
            case DOWN:
                dir = DOWN;
                break;
            case PAUSE:
                alert(y / 2 - 4, x / 2 - 34, "Press 'r' to play!", TRUE);
                while (1)
                {
                    if (getch() == RESUME)
                        break;
                }
                break;
            case ESC:
                endwin();
                return;
            }
            moveSnake();
            if (chkEnter)
            {
                if (++n >= stat[0])
                {
                    disappearGate();
                    disappearMagicGate();
                    appearGate();
                    appearMagicGate();
                    n = 0;
                    chkEnter = FALSE;
                }
            }
            if (++msTime % (msDiv[speed - 1] * 5) == 0)
            {
                disappearItem();
                appearItem();
            }
            if (stat[0] < 3)
                gameOver();
            if (isMissionClear())
            {
                alert(y / 2 - 4, x / 2 - 27, "Stage Clear!", FALSE);
                speed++;
                break;
            }
            if (checkGameOver())
            {
                alert(y / 2 - 4, x / 2 - 25, "Game Over!", FALSE);
                return;
            }
            drawMap();
            timeout(timeoutMs);
        }
        level++;
    }
    endwin();
}

void Stage::help()
{
    screenLock();
    int ySize = 0, yScroll = 0;
    while (1)
    {
        manual = newwin(sizeY, sizeX, startY, startX);
        description = newwin(desSizeY, desSizeX, desStartY, desStartX);
        scrollBar = newwin(scrollBarLen, 2, desStartY + yScroll, startX + sizeX - 6);
        wattron(manual, COLOR_PAIR(10));
        box(manual, 0, 0);
        mvwprintw(manual, 0, sizeX / 2 - manualTitle.length() / 2, "%s", manualTitle.c_str());
        wattroff(manual, COLOR_PAIR(10));

        mvwprintw(description, 0 + ySize,
                  sizeX / 2 - menuTitle.length() / 2 - 3, "%s", menuTitle.c_str());
        for (int i = 0; i < sizeof(menuTxt) / sizeof(menuTxt[0]); i++)
            mvwprintw(description, 2 + (i * 2) + ySize, sizeX / 2 - menuTxt[2].length() / 2 - 3, "%s", menuTxt[i].c_str());

        mvwprintw(description, 11 + ySize,
                  sizeX / 2 - shorTitle.length() / 2 - 3, "%s", shorTitle.c_str());
        for (int i = 0; i < sizeof(shorTxt) / sizeof(shorTxt[0]); i++)
            mvwprintw(description, 13 + (i * 2) + ySize, sizeX / 2 - shorTxt[6].length() / 2 - 3, "%s", shorTxt[i].c_str());

        if (txtLines >= desSizeY)
        {
            wattron(scrollBar, COLOR_PAIR(10));
            box(scrollBar, 0, 0);
            wattroff(scrollBar, COLOR_PAIR(10));
        }
        refresh();
        wrefresh(manual);
        wrefresh(description);
        wrefresh(scrollBar);
    RE:
        switch (getch())
        {
        case UP:
            if (yScroll)
                yScroll--;
            else
                goto RE;
            if (ySize)
                ySize++;
            break;
        case DOWN:
            if (yScroll < desSizeY - scrollBarLen)
                yScroll++;
            else
                goto RE;
            if (ySize > desSizeY - txtLines && txtLines > desSizeY)
                ySize--;
            break;
        case ESC:
            return;
        }
    }
}



void Stage::setMap()
{
    int i, j, k;
    stage = new int **[STAGE_NUM];
    for (i = 0; i < STAGE_NUM; i++)
    {
        stage[i] = new int *[MAP_ROW];
        for (j = 0; j < MAP_ROW; j++)
        {
            stage[i][j] = new int[MAP_COL];
        }
    }

    for (i = 0; i < STAGE_NUM; i++)
    {
        for (j = 0; j < MAP_ROW; j++)
        {
            for (k = 0; k < MAP_COL; k++)
            {
                if (!j || !k || j == ROW_END || k == COL_END)
                    stage[i][j][k] = WALL;
                else
                    stage[i][j][k] = EMPTY;
            }
        }
        stage[i][0][0] = IMMUNE_WALL;
        stage[i][0][COL_END] = IMMUNE_WALL;
        stage[i][ROW_END][0] = IMMUNE_WALL;
        stage[i][ROW_END][COL_END] = IMMUNE_WALL;
        if (i == 1)
        {
            for (int z = 10; z < 40; z++)
                stage[i][7][z] = WALL;
            for (int z = 10; z < 40; z++)
                stage[i][MAP_ROW - 7][z] = WALL;
        }
        if (i == 2)
        {
            for (int z = 5; z < 20; z++)
                stage[i][z][MAP_COL - 15] = WALL;
            for (int z = 5; z < 20; z++)
                stage[i][z][15] = WALL;
        }
        if (i == 3)
        {
            for (int z = 10; z < 40; z++)
            {
                if (z > 22 && z < 27)
                    continue;
                stage[i][7][z] = WALL;
            }
            for (int z = 10; z < 40; z++)
            {
                if (z > 22 && z < 27)
                    continue;
                stage[i][MAP_ROW - 7][z] = WALL;
            }
            for (int z = 5; z < 20; z++)
            {
                if (z > 10 && z < 14)
                    continue;
                if (stage[i][z][MAP_COL - 15] == WALL)
                    stage[i][z][MAP_COL - 15] = IMMUNE_WALL;
                else
                    stage[i][z][MAP_COL - 15] = WALL;
            }
            for (int z = 5; z < 20; z++)
            {
                if (z > 10 && z < 14)
                    continue;
                if (stage[i][z][15] == WALL)
                    stage[i][z][15] = IMMUNE_WALL;
                else
                    stage[i][z][15] = WALL;
            }
        }
    }
}

void Stage::copyMap(int nStage)
{
    map = new int *[MAP_ROW];
    for (int i = 0; i < MAP_COL; i++)
        map[i] = new int[MAP_COL];
    for (int i = 0; i < MAP_ROW; i++)
    {
        for (int j = 0; j < MAP_COL; j++)
            map[i][j] = stage[nStage][i][j];
    }
}

void Stage::drawMap()
{
    game = newwin(MAP_ROW, MAP_COL,
                  y / 2 - MAP_ROW / 2, x / 2 - (MAP_COL / 2 + 16));
    for (int i = 0; i < MAP_ROW; i++)
    {
        for (int j = 0; j < MAP_COL; j++)
        {
            int index = map[i][j];
            wattron(game, COLOR_PAIR(index));
            mvwaddch(game, i, j, itemIndex[index]);
            wattroff(game, COLOR_PAIR(index));
        }
        printw("\n");
    }

    //score = newwin(19, 30, y / 2 - (MAP_ROW / 2 + 4), x / 2 + MAP_COL / 2 - 7.4);
    //wattron(score, COLOR_PAIR(10));
    //box(score, 0, 0);
    //mvwprintw(score, 0, 10, "[ SCORE ]");
    //wattroff(score, COLOR_PAIR(10));
    // mvwprintw(score, 3, 5, "Snake Length: %d / %d", stat[0], SNAKE_MAX_LENGTH);
    // mvwprintw(score, 6, 5, "Growth Items: %d", stat[1]);
    // mvwprintw(score, 9, 5, "Poison Items: %d", stat[2]);
    // mvwprintw(score, 12, 5, "Slow Items: %d", stat[4]);
    // mvwprintw(score, 15, 5, "Gate: %d", stat[3]);

    // mission = newwin(16, 30, y / 2 - (MAP_ROW / 2 + 4) + 19, x / 2 + MAP_COL / 2 - 7.4);
    // wattron(mission, COLOR_PAIR(10));
    // box(mission, 0, 0);
    // mvwprintw(mission, 0, 9, "[ MISSION ]");
    // wattroff(mission, COLOR_PAIR(10));
    // mvwprintw(mission, 3, 5, "Snake Length: %d ( %c )", statMission[0], chkMission[0]);
    // mvwprintw(mission, 6, 5, "Growth Items: %d ( %c )", statMission[1], chkMission[1]);
    // mvwprintw(mission, 9, 5, "Poison Items: %d ( %c )", statMission[2], chkMission[2]);
    // mvwprintw(mission, 12, 5, "Gate: %d ( %c )", statMission[3], chkMission[3]);

    //info = newwin(4, 15, y / 2 - (MAP_ROW / 2 + 4), x / 2 + MAP_COL / 2 - 47.4);
    //mvwprintw(info, 0, 1, "[ STAGE %d/%d ]", level + 1, STAGE_NUM);
    //mvwprintw(info, 2, 3, "< %02d:%02d >", msTime / (msDiv[speed - 1] * 60), (msTime / msDiv[speed - 1]) % 60);

    refresh();
    //wrefresh(info);
    wrefresh(game);
    //wrefresh(score);
    //wrefresh(mission);
}

void Stage::appearItem()
{
    int appearNum = rand() % 3 + 1;
    for (int i = 0; i < appearNum; i++)
    {
        int itemType = rand() % NUMBER_OF_ITEMS + 5; // ITEM types should be consecutive integers.
        
        while (1)
        {
            int y = rand() % (MAP_ROW - 2) + 1;
            int x = rand() % (MAP_COL - 2) + 1;
            if (map[y][x] == EMPTY &&
                map[y][x - 1] != GATE && map[y][x + 1] != GATE &&
                map[y + 1][x] != GATE && map[y - 1][x] != GATE &&
                map[y][x - 1] != MAGIC_GATE && map[y][x + 1] != MAGIC_GATE &&
                map[y + 1][x] != MAGIC_GATE && map[y - 1][x] != MAGIC_GATE)
            {
                map[y][x] = itemType;
                itemPos.push_back(make_pair(y, x));
                break;
            }
        }
    }
}


void Stage::appearGate()
{
    int n, y, x;
    for (int i = 0; i < 2; i++)
    {
        while (1)
        {
            n = rand() % (!level ? 4 : 5);
            y = rand() % (MAP_ROW - (i?3:2)) + (i?2:1);
            x = rand() % (MAP_COL - (i?3:2)) + (i?2:1);
            switch (n)
            {
            case 0:
                y = 0;
                break;
            case 1:
                x = 0;
                break;
            case 2:
                x = COL_END;
                break;
            case 3:
                y = ROW_END;
                break;
            case 4:
                while (1)
                {
                    x = rand() % 30 + 10;
                    y = rand() % 15 + 5;
                    if (map[y][x] == WALL)
                        break;
                }
            }
            if (map[y][x] == WALL)
            {
                map[y][x] = GATE;
                gatePos.push_back(make_pair(y, x));
                break;
            }
        }
        if (i == 0)
            gate1 = new Something(y, x, GATE);
        if (i == 1)
            gate2 = new Something(y, x, GATE);
    }
}

void Stage::appearMagicGate()
{
    int n, y, x;
    for (int i = 0; i < 2; i++)
    {
        while (1)
        {
            n = rand() % (!level ? 4 : 5);
            y = rand() % (MAP_ROW - (i?3:2)) + (i?2:1);
            x = rand() % (MAP_COL - (i?3:2)) + (i?2:1);
            switch (n)
            {
            case 0:
                y = 0;
                break;
            case 1:
                x = 0;
                break;
            case 2:
                x = COL_END;
                break;
            case 3:
                y = ROW_END;
                break;
            case 4:
                while (1)
                {
                    x = rand() % 30 + 10;
                    y = rand() % 15 + 5;
                    if (map[y][x] == WALL && map[y][x] != GATE)
                        break;
                }
            }
            if(i==0){
                if (map[y][x] == WALL && map[y][x] != GATE )
                {
                    map[y][x] = MAGIC_GATE;
                    magicGatePos.push_back(make_pair(y, x));
                    break;
                }
            }
            if(i==1){
               if (map[y][x] == WALL && map[y][x] != GATE && map[y][x] != MAGIC_GATE )
                {
                    map[y][x] = MAGIC_EXIT;
                    magicGatePos.push_back(make_pair(y, x));
                    break;
                }   
            }
        }
        if (i == 0)
            magicGate1 = new Something(y, x, MAGIC_GATE);
        if (i == 1)
            magicGate2 = new Something(y, x, MAGIC_EXIT);
    }
}

void Stage::disappearItem()
{
    for (auto item : itemPos)
    {
        if (map[item.first][item.second] == GROWTH_ITEM || map[item.first][item.second] == POISON_ITEM || map[item.first][item.second] == SPEED_SLOW)
            map[item.first][item.second] = EMPTY;
    }
    itemPos.clear();
}

void Stage::disappearGate()
{
    for (auto gate : gatePos)
    {
        if(map[gate.first][gate.second] == GATE)
            map[gate.first][gate.second] = WALL;
    }
    gatePos.clear();
}

void Stage::disappearMagicGate()
{
    for (auto gate : magicGatePos)
    {
        if(map[gate.first][gate.second] == MAGIC_GATE)
            map[gate.first][gate.second] = WALL;
        if(map[gate.first][gate.second] == MAGIC_EXIT)
            map[gate.first][gate.second] = WALL;
    }
    magicGatePos.clear();
}

void Stage::makeSnake()
{
    stat[0] = 3;
    int row = 13;
    int col = 26;
    Bam = new Something(row, col--, SNAKE_BODY);
    Something *p = new Something(row, col--, SNAKE_BODY);
    Bam->link = p;
    p = new Something(row, col--, SNAKE_HEAD);
    Bam->link->link = p;
    map[Bam->y][Bam->x] = Bam->who;
    p = Bam->link;
    map[p->y][p->x] = p->who;
    p = p->link;
    map[p->y][p->x] = p->who;
}

void Stage::moveSnake()
{
    if (map[Bam->y][Bam->x] != WALL) //뱀의 머리의 현재위치가 벽이 아닌 경우. 
        map[Bam->y][Bam->x] = EMPTY; // 해당 위치를 빈공간으로 설정 
    Something *q = Bam;
    Something *p = q->link;
    while (p->link != NULL) //뱀의 몸통을 따라 노드를 이동, 각 노드의 좌표를 이전 좌표로 업데이트 
    {
        q->x = p->x;
        q->y = p->y;
        q = p;
        p = p->link;
    }
    if (dir == LEFT) //현재 방향에 따라 뱀의 머리와 머리 다음 노드의 좌표 업데이트
    {
        map[p->y][p->x] = q->who;
        q->x = p->x;
        q->y = p->y;
        p->x--;
    }
    else if (dir == UP)
    {
        map[p->y][p->x] = q->who;
        q->x = p->x;
        q->y = p->y;
        p->y--;
    }
    else if (dir == RIGHT)
    {
        map[p->y][p->x] = q->who;
        q->x = p->x;
        q->y = p->y;
        p->x++;
    }
    else if (dir == DOWN)
    {
        map[p->y][p->x] = q->who;
        q->x = p->x;
        q->y = p->y;
        p->y++;
    }
    if (map[p->y][p->x] == WALL || map[p->y][p->x] == SNAKE_BODY) //뱀이 벽이나 다른 뱀의 몸통 ()
    {
        map[p->y][p->x] = IMMUNE_WALL;
        gameOver();
    }
    if (map[p->y][p->x] == GATE)
    {
        enterGate(p);
    }
    if (map[p->y][p->x] == MAGIC_GATE)
    {
        enterMagicGate(p);
    }
    if (map[p->y][p->x] == MAGIC_EXIT)
    {
        enterMagicGate(p);
    }
    if (map[p->y][p->x] == GROWTH_ITEM){
        eatItem(GROWTH_ITEM);
    } 
    if(map[p->y][p->x] == POISON_ITEM) {
        eatItem(POISON_ITEM);
    }
    if(map[p->y][p->x] == SPEED_SLOW){
        eatItem(SPEED_SLOW);
    }
    map[p->y][p->x] = p->who;
}

void Stage::enterGate(Something *head)
{
    chkEnter = TRUE;
    if (gate1->x == head->x && gate1->y == head->y)
    {
        if (gate2->x == 0)
        {
            head->x = 1;
            head->y = gate2->y;
            dir = RIGHT;
        }
        else if (gate2->x == COL_END)
        {
            head->x = COL_END - 1;
            head->y = gate2->y;
            dir = LEFT;
        }
        else if (gate2->y == 0)
        {
            head->x = gate2->x;
            head->y = 1;
            dir = DOWN;
        }
        else if (gate2->y == ROW_END)
        {
            head->x = gate2->x;
            head->y = ROW_END - 1;
            dir = UP;
        }
        findRoot(gate2);
        if (dir == LEFT)
        {
            head->x = gate2->x - 1;
            head->y = gate2->y;
        }
        else if (dir == UP)
        {
            head->x = gate2->x;
            head->y = gate2->y - 1;
        }
        else if (dir == RIGHT)
        {
            head->x = gate2->x + 1;
            head->y = gate2->y;
        }
        else if (dir == DOWN)
        {
            head->x = gate2->x;
            head->y = gate2->y + 1;
        }
    }
    else if (gate2->x == head->x && gate2->y == head->y)
    {
        if (gate1->x == 0)
        {
            head->x = 1;
            head->y = gate1->y;
            dir = RIGHT;
        }
        else if (gate1->x == COL_END)
        {
            head->x = COL_END - 1;
            head->y = gate1->y;
            dir = LEFT;
        }
        else if (gate1->y == 0)
        {
            head->x = gate1->x;
            head->y = 1;
            dir = DOWN;
        }
        else if (gate1->y == ROW_END)
        {
            head->x = gate1->x;
            head->y = ROW_END - 1;
            dir = UP;
        }
        findRoot(gate1);
        if (dir == LEFT)
        {
            head->x = gate1->x - 1;
            head->y = gate1->y;
        }
        else if (dir == UP)
        {
            head->x = gate1->x;
            head->y = gate1->y - 1;
        }
        else if (dir == RIGHT)
        {
            head->x = gate1->x + 1;
            head->y = gate1->y;
        }
        else if (dir == DOWN)
        {
            head->x = gate1->x;
            head->y = gate1->y + 1;
        }
    }
    stat[3]++;
}

void Stage::enterMagicGate(Something *head)
{
    chkMagicEnter = TRUE;
    if (magicGate1->x == head->x && magicGate1->y == head->y)
    {
        if (magicGate2->x == 0)
        {
            head->x = 1;
            head->y = magicGate2->y;
            dir = RIGHT;
        }
        else if (magicGate2->x == COL_END)
        {
            head->x = COL_END - 1;
            head->y = magicGate2->y;
            dir = LEFT;
        }
        else if (magicGate2->y == 0)
        {
            head->x = magicGate2->x;
            head->y = 1;
            dir = DOWN;
        }
        else if (magicGate2->y == ROW_END)
        {
            head->x = magicGate2->x;
            head->y = ROW_END - 1;
            dir = UP;
        }
        findRoot(magicGate2);
        if (dir == LEFT)
        {
            head->x = magicGate2->x - 1;
            head->y = magicGate2->y;
        }
        else if (dir == UP)
        {
            head->x = magicGate2->x;
            head->y = magicGate2->y - 1;
        }
        else if (dir == RIGHT)
        {
            head->x = magicGate2->x + 1;
            head->y = magicGate2->y;
        }
        else if (dir == DOWN)
        {
            head->x = magicGate2->x;
            head->y = magicGate2->y + 1;
        }
    }
    else if (magicGate2->x == head->x && magicGate2->y == head->y)
    {
        if (magicGate1->x == 0)
        {
            head->x = 1;
            head->y = magicGate1->y;
            dir = RIGHT;
        }
        else if (magicGate1->x == COL_END)
        {
            head->x = COL_END - 1;
            head->y = magicGate1->y;
            dir = LEFT;
        }
        else if (magicGate1->y == 0)
        {
            head->x = magicGate1->x;
            head->y = 1;
            dir = DOWN;
        }
        else if (magicGate1->y == ROW_END)
        {
            head->x = magicGate1->x;
            head->y = ROW_END - 1;
            dir = UP;
        }
        findRoot(magicGate1);
        if (dir == LEFT)
        {
            head->x = magicGate1->x - 1;
            head->y = magicGate1->y;
        }
        else if (dir == UP)
        {
            head->x = magicGate1->x;
            head->y = magicGate1->y - 1;
        }
        else if (dir == RIGHT)
        {
            head->x = magicGate1->x + 1;
            head->y = magicGate1->y;
        }
        else if (dir == DOWN)
        {
            head->x = magicGate1->x;
            head->y = magicGate1->y + 1;
        }
    }
    stat[3]++;
}

int Stage::findRoot(Something *gate)
{
    for (int i = 0; i < 4; i++)
    {
        if (dir == LEFT)
        {
            if (map[gate->y][gate->x - 1] == EMPTY)
                return dir;
            else
                dir = KEY_UP;
        }
        else if (dir == KEY_UP)
        {
            if (map[gate->y - 1][gate->x] == EMPTY)
                return dir;
            else
                dir = RIGHT;
        }
        else if (dir == RIGHT)
        {
            if (map[gate->y][gate->x + 1] == EMPTY)
                return dir;
            else
                dir = DOWN;
        }
        else if (dir == DOWN)
        {
            if (map[gate->y + 1][gate->x] == EMPTY)
                return dir;
            else
                dir = LEFT;
        }
    }
    return dir;
}

void Stage::eatItem(int item)
{
    if (item == GROWTH_ITEM)
    {
        if (stat[0] == 10)
            return;
        Something *p = new Something(Bam->y, Bam->x, SNAKE_BODY);
        if (Bam->x - Bam->link->x == 1)
            p->x++;
        else if (Bam->y - Bam->link->y == 1)
            p->y++;
        else if (Bam->x - Bam->link->x == -1)
            p->x--;
        else if (Bam->y - Bam->link->y == -1)
            p->y--;
        p->link = Bam;
        Bam = p;
        if (map[Bam->y][Bam->x] != WALL)
            map[Bam->y][Bam->x] = Bam->who;
        stat[0]++;
        stat[1]++;
    }
    else if (item == POISON_ITEM)
    {
        map[Bam->y][Bam->x] = EMPTY;
        Bam = Bam->link;
        stat[0]--;
        stat[2]++;
    }
    else if(item == SPEED_SLOW)
    {
        timeoutMs+=100;
        stat[4]++;
    }
}
    


void Stage::setMission()
{
    finish = chkEnter = FALSE;
    memset(stat, 0, sizeof(stat));
    memset(statMission, 0, sizeof(statMission));
    memset(chkMission, ' ', sizeof(chkMission));

    
    statMission[0] = 5;
    statMission[1] = 2;
    statMission[2] = 2;
    statMission[3] = 2; 

}


bool Stage::isMissionClear()
{
    int count = 0;
    for (int i = 0; i < 4; i++)
    {
        if (stat[i] >= statMission[i])
        {
            chkMission[i] = 'v';
            count++;
        }
        else if (!i)
            chkMission[i] = ' ';
    }
    if (count == 4)
        return TRUE;
    return FALSE;
}

void Stage::gameOver()
{
    finish = true;
}

void Stage::alert(int posY, int posX, const string msg, bool stopFlag)
{
    WINDOW *alert = newwin(7, msg.length() * 2, posY, posX);
    box(alert, 0, 0);
    wattron(alert, COLOR_PAIR(0));
    wbkgd(alert, COLOR_PAIR(2));
    mvwprintw(alert, 3, msg.length() / 2, msg.c_str());
    wrefresh(alert);
    if (!stopFlag)
        usleep(1750000);
}