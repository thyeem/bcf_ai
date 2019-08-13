#include "common.h"
#include "board.h"
#include "mariai.h"

int main (int argc, char* argv[]) {
    Board *pb = new Board;
    if ( argc < 2 ) {
        cerr << "No input file.\n" << endl;
        return 0;
    }
    pb->read_board(argv[1]);

    Mariai maria(pb, NULL);
    int x, y;
    tie(x, y) = maria.next_move();
    pb->make_move(x, y);
    pb->write_board(argv[1]);
    delete pb;
    return 0;
}

