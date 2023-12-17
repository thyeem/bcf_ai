#include "pattern.h"
#include "common.h"

Pattern::Pattern() {}

Pattern::~Pattern() {}

VecCoords Pattern::find_candidates(Board &b, VecCoords &candy, size_t size) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      if (b.get_stone(i, j) == EMPTY)
        continue;
      find_pattern_inline(b, candy, i, j, 1, 0, size);
      find_pattern_inline(b, candy, i, j, -1, 0, size);
      find_pattern_inline(b, candy, i, j, 0, 1, size);
      find_pattern_inline(b, candy, i, j, 0, -1, size);
      find_pattern_inline(b, candy, i, j, 1, 1, size);
      find_pattern_inline(b, candy, i, j, 1, -1, size);
      find_pattern_inline(b, candy, i, j, -1, 1, size);
      find_pattern_inline(b, candy, i, j, -1, -1, size);
    }
  }
  refine_candy(candy, b);
  return candy;
}

void Pattern::find_pattern_inline(Board &b, VecCoords &candy, int i, int j,
                                  int di, int dj, size_t size) {
  if (size == 0) {
    find_pattern_each(b, candy, i, j, di, dj, "*?");
  } else {
    find_pattern_each(b, candy, i, j, di, dj, "=?=");
    find_pattern_each(b, candy, i, j, di, dj, "|?|");
    find_pattern_each(b, candy, i, j, di, dj, "==?");
    find_pattern_each(b, candy, i, j, di, dj, "||?");
    find_pattern_each(b, candy, i, j, di, dj, "=_?");
    find_pattern_each(b, candy, i, j, di, dj, "|_?");
    find_pattern_each(b, candy, i, j, di, dj, "|_|?");
  }
}

bool Pattern::find_pattern_each(Board &b, VecCoords &candy, int i, int j,
                                int di, int dj, string pt) {
  int size = pt.length();
  for (int s = 0; s < size; s++) {
    int x = i + s * di;
    int y = j + s * dj;
    if (!b.in_range(x, y))
      return false;
    if (!match_stones(b, pt[s], x, y))
      return false;
  }
  // collecting candidates
  for (int s = 0; s < size; s++) {
    int x = i + s * di;
    int y = j + s * dj;
    if (pt[s] == '?')
      candy.push_back(make_tuple(x, y));
  }
  return true;
}

bool Pattern::match_stones(Board &b, char ch, int i, int j) {
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

void Pattern::refine_candy(VecCoords &candy, Board &b) {
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
