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
#include <sys/types.h>
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
#define BRANCHING 5
#define UCB_C 1.414142135624
#define UCB_POW 0.5
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
#define MINIMAX_PLY 4

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

static __uint128_t g_lehmer64_state;

static __uint64_t g_fastrand_state;

static inline void lehmer64_seed() {
  mt19937_64 mt64_rand(random_device{}());
  g_lehmer64_state = mt64_rand() << 1 | 1;
}

static inline uint64_t lehmer64() {
  g_lehmer64_state *= UINT64_C(0xda942042e4dd58b5);
  return g_lehmer64_state >> 64;
}

static inline void fastrand_seed() {
  mt19937_64 mt64_rand(random_device{}());
  g_fastrand_state = mt64_rand();
}

static inline uint64_t fastrand() {
  g_fastrand_state = (214013 * g_fastrand_state + 2531011);
  return (g_fastrand_state >> 16) & 0x7FFF;
}

static inline uint64_t random_coords(uint8_t inf, uint8_t sup) {
  return lehmer64() % (sup - inf + 1) + inf;
}

#endif
