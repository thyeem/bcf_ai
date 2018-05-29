#ifndef GAME_H
#define GAME_H
#include "common.h"
#include "board.h"
#include "sofiai.h"
#include "mariai.h"
#include "draw.h"

class Game {
public:
    Game();
    ~Game();
    Board*    gb             ();
    Draw*     gd             ();
    void      select_menu    (char ch);
    char      main_menu      ();
    void      play_game      ();
    bool      auto_play;

private:
    Board*    p_board;
    Draw*     p_draw;
};

#endif
