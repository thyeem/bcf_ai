#ifndef SOFIAI_H
#define SOFIAI_H
#include "board.h"
#include "common.h"

typedef tuple<double, int, int> MinimaxNode;
typedef tuple<double, Coords> CoordsValue;
typedef vector<CoordsValue> VecCoordsValue;

class Sofiai {
  static unsigned int g_seed;

public:
  Sofiai(Board *b);
  ~Sofiai();
  void set_coeff();
  Board *gb();
  void print_board();
  Coords next_move();
  MinimaxNode minimax(Board b, int x, int y, int depth, bool is_maximizer,
                      float alpha, float beta);
  float eval_state(Board &b, int depth);
  void analyze_pt(Board &b, Stone o, Stone x, int mode);
  void find_pt(Board &b, int i, int j, Stone o, Stone x, string pt);
  bool find_pt_inline(Board &b, int i, int j, int di, int dj, Stone o, Stone x,
                      string pt);
  Stone char_to_stone(char ch, Stone o, Stone x);

  VecCoords get_child(Board &b, int mode);

private:
  Board *p_board;
  Stone cb[N][N];

public:
  map<string, tuple<double, VecCoords>> lookup_pattern;
  map<string, double> lookup_coeff;
  map<string, double> lookup_priority;
};

#endif
