#ifndef COMMON_H
#define COMMON_H

#include <random>
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <algorithm>
#include <tuple>
#include <vector>
#include <map>
#include <unordered_map>
#include <iterator>
#include <cstring>
#include <ncurses.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
using namespace std;

#define N                  19
#define GOAL               5
#define WP                 10
#define BCF                1
#define PLAYOUTS           1200000
#define NEXP               1
#define UCB_C              0.5
#define UCB_POW            0.5
#define UCB_C0             (log(PLAYOUTS))
#define EARLY_CUT          400000
#define CUT_DENSITY        2.19

#define PRINT_TREE         0
#define AI_MATCH           1
#define AI_QUIZ            0
#define QUIZ_NUM           4
#define MATCH_NUM          200
#define PUT_CANDY          1
#define PUT_BOARD          1

#define PBWIDTH            33
#define PBSTR              "||||||||||||||||||||||||||||||||||||||||"
#define LINE_BUFFER        (PLAYOUTS / 6)

// Sofiai's parameter
#define INF                1.0e7
#define DEPTH              4

#define TC_RESET           "\033[0m"
#define TC_BLACK           "\033[30m"
#define TC_RED             "\033[91m"
#define TC_GREEN           "\033[92m"
#define TC_YELLOW          "\033[93m"
#define TC_BLUE            "\033[94m"
#define TC_MAGENTA         "\033[95m"
#define TC_CYAN            "\033[96m"
#define TC_WHITE           "\033[97m"
#define TC_CLEAR           "\033[2J"

enum     Stone  { EMPTY, BLACK, WHITE, CANDY };
enum     Player { HUMAN, SOFIAI, MARIAI };
enum     Mode   { NIL, NORMAL, SHORT };
typedef  tuple<int, int> Tii;
typedef  vector<Tii> VTii;

class Node {
public:
    Node(Node* p, Tii g, Stone s) :
        Q      (UCB_C0),
        wp     (0),
        win    (0),
        visit  (0),
        prev   (p),
        grd    (g),
        turn   (s),
        leaf   (true) {}
    ~Node() {}

public:
    double        Q;
    double        wp;
    int           win;
    int           visit;
    Node*         prev;
    Tii           grd;
    Stone         turn;
    bool          leaf;
    vector<Node>  child;
    vector<char>  icQ;
};

template <typename T>
void uniq_vec(vector<T> &vec) {
    sort(vec.begin(), vec.end());
    vec.erase(unique(vec.begin(), vec.end()), vec.end());
}

#endif
