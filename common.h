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

#define N              19
#define GOAL           5 
#define WP             10
#define BCF            1
#define NPLAY          1000000
#define NEXP           40
#define UCB_C          0.3
#define UCB_POW        0.5
#define UCB_C0         (log(NPLAY))
#define RATIO_FD       50

#define RUN            1
#define PRINT_TREE     0
#define AI_MATCH       1
#define AI_QUIZ        0
#define QUIZ_NUM       4
#define MATCH_NUM      300
#define PUT_CANDY      1
#define PUT_BOARD      1

#define PBWIDTH        33
#define PBSTR          "||||||||||||||||||||||||||||||||||||||||"
#define LINE_BUFFER    (NPLAY / 6)
#define FREQ_FD        (NPLAY / 4)

// Sofiai's parameter
#define INF            1.0e7
#define DEPTH          4

#define TC_RESET       "\033[0m"
#define TC_BLACK       "\033[30m"
#define TC_RED         "\033[31m"
#define TC_GREEN       "\033[32m"
#define TC_YELLOW      "\033[33m"
#define TC_BLUE        "\033[34m"
#define TC_MAGENTA     "\033[35m"
#define TC_CYAN        "\033[36m"
#define TC_WHITE       "\033[37m"
#define TC_CLEAR       "\033[2J"

enum      Stone  { EMPTY, BLACK, WHITE, CANDY };
enum      Player { HUMAN, SOFIAI, MARIAI };
typedef   tuple<int, int>    Tii;
typedef   vector<Tii>        VTii;

class Node {
public:
    Node(Node* p, Tii g, Stone s) : Q      (UCB_C0),
                                    wp     (0), 
                                    win    (0), 
                                    visit  (0), 
                                    prev   (p), 
                                    grd    (g), 
                                    turn   (s),
                                    leaf   (true) {}
    ~Node() {}

public: 
    double          Q;
    double          wp;
    int             win;
    int             visit;
    Node*           prev;
    Tii             grd;
    Stone           turn;
    bool            leaf;
    vector<Node>    child;
    vector<char>    icQ;
};

class Key_hasher {
public:
    size_t operator()(const tuple<Tii, Stone> &t) const {
        return (( hash<int>() (get<0>(get<0>(t))) << 1) ^
                ( hash<int>() (get<1>(get<0>(t))) << 1) ^
                ( hash<int>() (get<1>(t) << 1 )) >> 1);
    }
};

template <typename T>
void uniq_vec(vector<T> &vec) {
    sort(vec.begin(), vec.end());
    vec.erase(unique(vec.begin(), vec.end()), vec.end());
}

template <typename T>
void sort_vec_more(vector<T> &vec) {
    sort(vec.begin(), vec.end(), greater<T>());
}

template <typename T>
void sort_vec_less(vector<T> &vec) {
    sort(vec.begin(), vec.end(), less<T>());
}

template <typename K, typename V>
vector<pair<K, V>> sort_map_more(map<K, V> &map) {
    vector<pair<K, V>> vmap(map.begin(), map.end());
    sort(vmap.begin(), vmap.end(),
         [] (pair<K, V> const &p1, pair<K, V> const &p2)
         { return p1.second > p2.second; });
    return vmap;
}

template <typename K, typename V>
vector<pair<K, V>> sort_map_less(map<K, V> &map) {
    vector<pair<K, V>> vmap(map.begin(), map.end());
    sort(vmap.begin(), vmap.end(),
         [] (pair<K, V> const &p1, pair<K, V> const &p2)
         { return p1.second < p2.second; });
    return vmap;
}

#endif
