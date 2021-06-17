#ifndef BOARD_H
#define BOARD_H
#include "common.h"

class Board {
public:
  Board();
  ~Board();
  Stone get_stone(int x, int y);
  void set_stone(int x, int y, Stone s);
  int get_score(Stone s);
  void set_score(Stone s, int score);
  Player get_player(Stone s);
  void set_player(Player pB, Player pW);
  bool is_last_move(int x, int y);
  void set_last_move(int x, int y);
  string get_nickname(Stone s);
  void toggle_turn();
  Stone last_turn();
  Stone whose_turn();

  bool in_range(int x, int y);
  int make_move(int x, int y);
  bool check_quit(int x, int y);
  bool check_3_3(int x, int y);
  int count_straight(int x, int y, int dx, int dy);
  int count_straight_open(int x, int y, int dx, int dy);
	bool is_nstones_made(int x, int y, int npi_over_four, int n);
	bool is_nstones_made_open(int x, int y, int npi_over_four, int n);
  void bite_move(int x, int y);
  void bite_move_towards(int x, int y, int dx, int dy);
  void print_board(VecCoords candy, bool print_candy);
  void read_board(string file);
  void write_board(string file);

private:
  Stone board[N][N];
  Stone turn;
  Stone last;
  Player playerB;
  Player playerW;
  int scoreB;
  int scoreW;
  int last_x;
  int last_y;

public:
  int moves;
	int inf_x;
	int sup_x;
	int inf_y;
	int sup_y;
  float eB;
  float eW;
};

#endif
