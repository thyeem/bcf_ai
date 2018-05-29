#ifndef TEST_H
#define TEST_H 

#include "common.h"
#include "board.h"
#include "sofiai.h"
#include "mariai.h"

class Test {

public:
    Test();
    ~Test();
    void    run_test           ();
    void    set_quizN          (Board &b, int quizN);
    void    solve_quiz         (int quizN, bool put_board);
    void    match_bet_AIs      ();
    bool    match_play         (int nth);
    void    print_param_info   ();
    void    print_per_move     (Board &b, int &lines, int &pass, int nth);
    void    print_per_game     (Board &b, int &lines, int &pass, int nth, int start_at);
    void    print_stats        (bool fdump);
    void    rewind_stdout      (int lines, int pass);

private:
    string   dump_file;
    ofstream fout;
    int      games;
    int      wins;
};

#endif
