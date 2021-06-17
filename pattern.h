
#ifndef PATTERN_H
#define PATTERN_H

#include "board.h"
#include "common.h"

class Pattern {
public:
  Pattern();
  ~Pattern();

  VecCoords find_candidates(Board &b, size_t size);
  void find_pattern_inline(Board &b, int i, int j, int di, int dj, size_t size);
  bool find_pattern_each(Board &b, int i, int j, int di, int dj, string pt);
  bool match_stones(Board &b, char ch, int i, int j);
  void refine_candy(VecCoords *candy, Board &b);
};

template <typename T> void uniq_vec(vector<T> &vec) {
  sort(vec.begin(), vec.end());
  vec.erase(unique(vec.begin(), vec.end()), vec.end());
}

#endif