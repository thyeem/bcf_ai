#ifndef GAME_H
#define GAME_H
#include "board.h"
#include "common.h"
#include "draw.h"
#include "mariai.h"
#include "sofiai.h"

class Game {
public:
  Game();
  ~Game();
  Board *gb();
  Draw *gd();
  void select_menu(char ch);
  char main_menu();
  void play_game();
  bool auto_play;

private:
  Board *p_board;
  Draw *p_draw;
};

#endif
