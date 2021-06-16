#ifndef MARIAI_H
#define MARIAI_H
#include "board.h"
#include "common.h"
#include "draw.h"

class Node {
public:
  Node(Node *p, Coords g, Stone s)
      : Q(100.), win(0), visit(0), prev(p), coords(g), turn(s), leaf(true) {}

  ~Node() {}

public:
  float Q;
  int win;
  int visit;
  Node *prev;
  Coords coords;
  Stone turn;
  bool leaf;
  vector<Node> children;
  vector<uint8_t> children_iQ;
};

class Mariai {
public:
  Mariai(Board *b, Draw *d);
  ~Mariai();
  Board *gb();
  Draw *gd();
  int RandomMove(Board &b, bool is_move_x);
  float calc_ucb(const Node *node);
  void sort_children_by_Q(Node *node);
  Node *get_most_visited_child(Node *node);
  void init_tree(Node *root, Board &b);
  void run_mcts(Node *root, Board &b);
  tuple<bool, Node *> select_path(Node *node, Board &vg);
  void expand_node(Node *node, Board &vg);
  bool backpropagation(Node *node, Node *root, Stone turn);
  void fast_rollout(Board &vg, bool quit);
  void insert_node(Node *node, Coords q, Stone s);
  bool is_expandable(Node *node);
  bool move_and_check_quit(Node *node, Board &vg);
  void print_tree(Node *node, int set_width, ofstream &fout);
  Coords pick_best(Node *node);
  Coords next_move();
  void show_progress();
  void dump_progress(float progress);
  void gen_candy(Board &b);
  void refine_candy(Board &b);
  void analyze_pattern(Board &b, size_t size);
  void find_pattern_inline(Board &b, int i, int j, int di, int dj, size_t size);
  bool find_pattern_each(Board &b, int i, int j, int di, int dj, string pt);
  bool match_stones(Board &b, char ch, int i, int j);

public:
  VecCoords candy;

private:
  Board *p_board;
  Draw *p_draw;
  int it;
};

template <typename T> void uniq_vec(vector<T> &vec) {
  sort(vec.begin(), vec.end());
  vec.erase(unique(vec.begin(), vec.end()), vec.end());
}

#endif
