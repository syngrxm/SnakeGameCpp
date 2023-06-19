#include "Stage.h"
/************************************************
    @file Main.cpp
    @author 안수현, 김광현 ,송규원
    @version 1.0 06/15/2022
************************************************/
int main()
{
    Stage view = Stage();
    string game;
    while(1)
    {
        game = view.menu();
        clear();
        if(game == "PLAY")
            view.play();
        else if(game == "HELP")
            view.help();
        else
            break;
    }
    return 0;
}