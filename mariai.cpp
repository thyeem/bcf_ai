#include "mariai.h"
#include <_types/_uint8_t.h>

Mariai::Mariai(Board *b, Draw *d) : it(0) {
  p_board = b;
  p_draw = d;
  fastrand_seed();
  lehmer64_seed();
}

Mariai::~Mariai() {}

Board *Mariai::gb() { return p_board; }

Draw *Mariai::gd() { return p_draw; }

int Mariai::RandomMove(Board &b, bool is_move_x) {
  if (is_move_x) {
    return (int)random_coords(b.inf_x, b.sup_x);
  } else {
    return (int)random_coords(b.inf_y, b.sup_y);
  }
}

float Mariai::calc_ucb(const Node *node) {
  const float log_playouts = log(PLAYOUTS);
  if (node->prev == NULL)
    return 1;
  return (1. * node->win / node->visit) +
         UCB_C * pow(log_playouts / node->visit, UCB_POW);
}

void Mariai::sort_children_by_Q(Node *node) {
  if (node->prev == NULL)
    return;

  sort(node->prev->children_iQ.begin(), node->prev->children_iQ.end(),
       [&](const size_t a, const size_t b) {
         return node->prev->children[a].Q > node->prev->children[b].Q;
       });
}

Node *Mariai::get_most_visited_child(Node *node) {
  int max = -1;
  Node *most_visited = NULL;
  for (auto &child : node->children) {
    if (child.visit > max && child.Q > 0) {
      max = child.visit;
      most_visited = &child;
    }
  }
  return most_visited;
}

Coords Mariai::next_move() {
  if (gb()->moves == 0)
    return make_tuple(N / 2, N / 2);
  Node root(NULL, make_tuple(-1, -1), EMPTY);
  it = 0;

  // Init tree: forced first expansion
  init_tree(&root, *gb());
  // MCTS based on UCB1
  run_mcts(&root, *gb());

#if PRINT_TREE
  string move = to_string(gb()->moves);
  string fname = "gtree_" + string(3 - move.length(), '0') + move + ".log";
  ofstream fout(fname, ios::app);
  print_tree(&root, 0, fout);
  fout.close();
#endif

  // return make_tuple(-1, -1);
  return pick_best(&root);
}

void Mariai::init_tree(Node *root, Board &b) { expand_node(root, b); }

void Mariai::run_mcts(Node *root, Board &b) {
  for (int n = 0; n < PLAYOUTS; n++) {
    Node *head = NULL;
    Board vb = b;
    bool quit = false;

    // TREE POLICY: selection & expansion
    tie(quit, head) = select_path(root, vb);

    // DEFAULT POLICY: playout using Monte-Carlo method
    fast_rollout(vb, quit);

    // BACKUP: backpropagation
    quit = backpropagation(head, root, vb.whose_turn());
    if (quit)
      return;
    else
      it++;
    show_progress();
  }
}

tuple<bool, Node *> Mariai::select_path(Node *node, Board &vb) {
  // TREE POLICY: selection
  while (!node->leaf) {
    node = &node->children[node->children_iQ[0]];
    if (move_and_check_quit(node, vb))
      return make_tuple(true, node);
  }

  // TREE POLICY: expansion
  if (is_expandable(node)) {
    expand_node(node, vb);
    node = &node->children[node->children_iQ[0]];
    if (move_and_check_quit(node, vb))
      return make_tuple(true, node);
  }
  return make_tuple(false, node);
}

void Mariai::expand_node(Node *node, Board &vb) {
  Board g = vb;
  gen_candy(g);
  uint8_t node_index = 0;
  for (auto q : candy) {
    insert_node(node, q, g.whose_turn());
    node->children_iQ.push_back(node_index++);
  }
  node->leaf = false;
}

void Mariai::insert_node(Node *node, Coords q, Stone s) {
  node->children.push_back(Node(node, q, s));
}

void Mariai::fast_rollout(Board &vb, bool quit) {
  while (!quit) {
    int x, y;
    while (1) {
      x = RandomMove(vb, true);
      y = RandomMove(vb, false);
      if (!vb.make_move(x, y))
        break;
    }
    quit = vb.check_quit(x, y);
    if (!quit)
      vb.toggle_turn();
  }
}

bool Mariai::backpropagation(Node *node, Node *root, Stone turn) {
  while (node != NULL) {
    node->visit++;
    // fast-decision
    if (node != root && node->visit > EARLY_CUT)
      return true;
    if (turn == node->turn)
      node->win++;
    node->Q = calc_ucb(node);
    sort_children_by_Q(node);
    node = node->prev;
  }
  return false;
}

bool Mariai::is_expandable(Node *node) {
  return node->leaf && node->visit >= BRANCHING;
}

bool Mariai::move_and_check_quit(Node *node, Board &vb) {
  Coords q = node->coords;
  int x, y;
  tie(x, y) = q;
  vb.make_move(x, y);
  if (vb.check_quit(x, y))
    return true;
  vb.toggle_turn();
  return false;
}

void Mariai::show_progress() {
  if (gd() == NULL)
    return;
  if (it % LINE_BUFFER == 0)
    gd()->dump_progress(1.0 * it / PLAYOUTS);
}

Coords Mariai::pick_best(Node *node) {
  Node *best = get_most_visited_child(node);
  float winning_prob = 1. * best->win / best->visit;
  gb()->eB =
      (best->turn == BLACK) ? 100 * winning_prob : 100 * (1 - winning_prob);
  gb()->eW = 100 - gb()->eB;
  return best->coords;
}

void Mariai::print_tree(Node *node, int set_width, ofstream &fout) {
  Node *head = node;
  string color;
  if (!fout.is_open())
    return;
  switch (head->turn) {
  case BLACK:
    color = "B";
    break;
  case WHITE:
    color = "W";
    break;
  default:
    color = "ROOT";
  }
  if (head->Q > -2) {
    fout << setw(set_width) << "[";
    fout << "(" << get<0>(head->coords) << ", " << get<1>(head->coords) << ", "
         << color << "), "
         << "Q: " << head->Q << ", "
         << "w: " << head->win << ", "
         << "v: " << head->visit << "]"
         << "\n";
  }
  if (!head->leaf) {
    vector<size_t> indices(node->children.size());
    iota(indices.begin(), indices.end(), 0);
    sort(indices.begin(), indices.end(), [&](const size_t a, const size_t b) {
      return node->children[a].visit > node->children[b].visit;
    });
    for (auto i : indices) {
      print_tree(&head->children[i], set_width + 3, fout);
    }
  }
}

// POLICY: NEXT MOVE CANDIDATES =========================================

void Mariai::gen_candy(Board &b) {
  candy.clear();
  analyze_pattern(b, 1);
  if (!candy.size())
    analyze_pattern(b, 0);
}

void Mariai::refine_candy(Board &b) {
  uniq_vec(candy);
  for (auto it = candy.begin(); it != candy.end();) {
    int i, j;
    tie(i, j) = (*it);
    if (b.check_3_3(i, j))
      it = candy.erase(it);
    else
      ++it;
  }
}

void Mariai::analyze_pattern(Board &b, size_t size) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (b.get_stone(i, j) == EMPTY)
        continue;
      find_pattern_inline(b, i, j, 1, 0, size);
      find_pattern_inline(b, i, j, -1, 0, size);
      find_pattern_inline(b, i, j, 0, 1, size);
      find_pattern_inline(b, i, j, 0, -1, size);
      find_pattern_inline(b, i, j, 1, 1, size);
      find_pattern_inline(b, i, j, 1, -1, size);
      find_pattern_inline(b, i, j, -1, 1, size);
      find_pattern_inline(b, i, j, -1, -1, size);
    }
  }
  refine_candy(b);
}

void Mariai::find_pattern_inline(Board &b, int i, int j, int di, int dj,
                                 size_t size) {
  if (size == 0) {
    find_pattern_each(b, i, j, di, dj, "*?_");
    find_pattern_each(b, i, j, di, dj, "*_?_");
  } else {
    find_pattern_each(b, i, j, di, dj, "=?=");
    find_pattern_each(b, i, j, di, dj, "|?|");
    find_pattern_each(b, i, j, di, dj, "==?");
    find_pattern_each(b, i, j, di, dj, "||?");
    find_pattern_each(b, i, j, di, dj, "=|||_?");
    find_pattern_each(b, i, j, di, dj, "|===_?");
    find_pattern_each(b, i, j, di, dj, "|_|?");

    // find_pattern_each(b, i, j, di, dj, "=|?_");
    // find_pattern_each(b, i, j, di, dj, "|=?|");
  }
}

bool Mariai::find_pattern_each(Board &b, int i, int j, int di, int dj,
                               string pt) {
  int size = pt.length();
  for (int s = 0; s < size; s++) {
    int x = i + s * di;
    int y = j + s * dj;
    if (!b.in_range(x, y))
      return false;
    if (!match_stones(b, pt[s], x, y))
      return false;
  }
  // collecting candies
  for (int s = 0; s < size; s++) {
    int x = i + s * di;
    int y = j + s * dj;
    if (pt[s] == '?')
      candy.push_back(make_tuple(x, y));
  }
  return true;
}

bool Mariai::match_stones(Board &b, char ch, int i, int j) {
  switch (ch) {
  case '*':
    return (b.get_stone(i, j) != EMPTY) ? true : false;
    break;
  case '_':
  case '?':
    return (b.get_stone(i, j) == EMPTY) ? true : false;
    break;
  case '=':
    return (b.get_stone(i, j) == b.whose_turn()) ? true : false;
  case '|':
    return (b.get_stone(i, j) == b.last_turn()) ? true : false;
    break;
  default:
    return false;
  }
}
