#ifndef COMMON_H
#define COMMON_H

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <map>
#include <ncurses.h>
#include <random>
#include <stdint.h>
#include <stdio.h>
#include <string>
#include <time.h>
#include <tuple>
#include <unordered_map>
#include <vector>
using namespace std;

#define N 19
#define GOAL 5
#define WP 10
#define BCF 1
#define PLAYOUTS 1200000
#define BRANCHING 2
#define UCB_C 0.3
#define UCB_POW 0.5
#define MARIAI_DEPTH 4
#define EARLY_CUT (PLAYOUTS / 3)

#define PRINT_CANDY 1
#define PRINT_TREE 0
#define AI_MATCH 1
#define AI_QUIZ 0
#define QUIZ_NUM 4
#define MATCH_NUM 200
#define PUT_BOARD 1

#define PBWIDTH 33
#define PBSTR "||||||||||||||||||||||||||||||||||||||||"
#define LINE_BUFFER (PLAYOUTS / 7)
#define INF 1.0e7
#define SOFIAI_DEPTH 4

#define TC_RESET "\033[0m"
#define TC_BLACK "\033[30m"
#define TC_RED "\033[91m"
#define TC_GREEN "\033[92m"
#define TC_YELLOW "\033[93m"
#define TC_BLUE "\033[94m"
#define TC_MAGENTA "\033[95m"
#define TC_CYAN "\033[96m"
#define TC_WHITE "\033[97m"
#define TC_CLEAR "\033[2J"

enum Stone { EMPTY, BLACK, WHITE, CANDY };
enum Player { HUMAN, SOFIAI, MARIAI };

typedef tuple<int, int> Coords;
typedef vector<Coords> VecCoords;

class Node {
public:
  Node(Node *p, Coords g, Stone s, uint8_t depth)
      : Q(100.), win(0), visit(0), prev(p), grd(g), turn(s), leaf(true),
        depth(depth) {}
  ~Node() {}

public:
  float Q;
  int win;
  int visit;
  Node *prev;
  Coords grd;
  Stone turn;
  bool leaf;
  uint8_t depth;
  vector<Node> child;
  vector<uint8_t> icQ;
};

template <typename T> void uniq_vec(vector<T> &vec) {
  sort(vec.begin(), vec.end());
  vec.erase(unique(vec.begin(), vec.end()), vec.end());
}

#endif
