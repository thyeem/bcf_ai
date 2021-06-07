#ifndef BOARD_H
#define BOARD_H
#include "common.h"

class Board {
public:
    Board();
    ~Board();
    Stone      get_stone            (int x, int y);
    void       set_stone            (int x, int y, Stone s);
    int        get_score            (Stone s);
    void       set_score            (Stone s, int score);
    Player     get_player           (Stone s);
    void       set_player           (Player pB, Player pW);
    bool       is_last_move         (int x, int y);
    void       set_last_move        (int x, int y);
    string     get_nickname         (Stone s);
    void       toggle_turn          ();
    Stone      last_turn            ();
    Stone      whose_turn           ();

    void       print_board          (bool dump_candy);
    void       read_board           (string file);
    void       write_board          (string file);
    bool       in_range             (int x, int y);
    int        make_move            (int x, int y);
    bool       check_quit           (int x, int y);
    int        check_quit_inline    (int x, int y, int dx, int dy);
    bool       check_3_3            (int x, int y);
    int        check_3_3_inline     (int x, int y, int dx, int dy);
    void       bite_move            (int x, int y);
    void       bite_move_inline     (int x, int y, int dx, int dy);

private:
    Stone      board[N][N];
    Stone      turn;
    Stone      last;
    Player     playerB;
    Player     playerW;
    int        scoreB;
    int        scoreW;
    int        X;
    int        Y;

public:
    int        moves;
    double     eB;
    double     eW;
    VecCoords  candy;
};

#endif
