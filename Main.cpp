#include "Stage.h"

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