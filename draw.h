#ifndef DRAW_H
#define DRAW_H
#include "board.h"
#include "common.h"

class Draw {
public:
  Draw();
  ~Draw();
  void init_screen_std();
  void init_screen_win(Board *b);
  void wipe_out_msg();
  void wipe_out_prg();
  void dump_debug(const char *msg);
  void dump_progress(float pct);
  void dump_board(Board *b);
  void dump_turn(Board *b);
  void dump_score(Board *b);
  void dump_msg(const char *msg);
  void dump_EWP(float eB, float eW);
  void dump_who_won(Board *b);
  void update_screen(int stop, int y, int x, Board *b);
  void pause_auto_play();
  void wait_for_q();
  bool wait_for_s();
  char get_char();
  Coords get_move();

private:
  WINDOW *wrap;
  WINDOW *win;
};

#endif
