#include "board.h"
#include "common.h"
#include "sofiai.h"

int main(int argc, char *argv[]) {
  Board *pb = new Board;
  if (argc < 2) {
    cerr << "No input file.\n" << endl;
    return 0;
  }
  pb->read_board(argv[1]);

  Sofiai sofia(pb);
  int x, y;
  tie(x, y) = sofia.next_move();
  pb->make_move(x, y);
  pb->write_board(argv[1]);
  delete pb;
  return 0;
}
