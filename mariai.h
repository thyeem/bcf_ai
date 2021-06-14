#ifndef MARIAI_H
#define MARIAI_H
#include "board.h"
#include "common.h"
#include "draw.h"

class Mariai {
public:
  Mariai(Board *b, Draw *d);
  ~Mariai();
  Board *gb();
  Draw *gd();
  int random_move(Board &b, bool is_move_x);
  void gen_candy(Board &b);
  void refine_candy(Board &b);
  void analyze_pattern(Board &b, size_t size);
  void find_pattern_inline(Board &b, int i, int j, int di, int dj, size_t size);
  bool find_pattern_each(Board &b, int i, int j, int di, int dj, string pt);
  bool match_stones(Board &b, char ch, int i, int j);
  bool on_main_axis(int x, int y, int di, int dj);
  bool on_crux_wing(int x, int y, int di, int dj);
  bool on_perp_wing(int x, int y, int di, int dj);

  void sort_icQ(Node *node);
  vector<size_t> sort_icV(Node *node);
  vector<size_t> sort_icW(Node *node);
  Node *get_maxV_child(Node *node);
  double calc_ucb(Node *node);
  void init_tree(Node *roof, Board &b);
  void run_mcts(Node *roof, Board &b);
  tuple<bool, Node *> select_path(Node *node, Board &vg);
  void expand_node(Node *node, Board &vg);
  bool backpropagation(Node *node, Node *roof, Stone turn);
  void fast_rollout(Board &vg, bool quit);
  void insert_node(Node *node, Coords q, Stone s);
  bool is_expandable(Node *node);
  bool move_check_quit_vg(Node *node, Board &vg);
  void prune_tree(Node *node);
  void print_tree(Node *node, int set_width, ofstream &fout);
  Coords pick_best(Node *node);
  Coords next_move();
  void show_progress();
  void dump_progress(double pct);

public:
  VecCoords candy;

private:
  Board *p_board;
  Draw *p_draw;
  mt19937_64 mt64_rand;
  int it;
};

#endif
