#ifndef SOFIAI_H
#define SOFIAI_H
#include "common.h"
#include "board.h"

typedef   tuple<double, int, int>                    Tfii;
typedef   vector<tuple<double, Tii>>                 VTfg;
typedef   map<Tii, double>                           Mgf;
typedef   map<string, tuple<double, vector<Tii>>>    MsTdVg;
typedef   map<string, double>                        Msd;

class Sofiai {
    static unsigned int g_seed;

public:
    Sofiai(Board *b);
    ~Sofiai();
    void       set_coeff        ();
    Board*     gb               ();
    void       print_board      ();
    Tii        next_move        ();
    Tfii       minimax          (Board b, int x, int y, int depth, bool is_maximizer, 
                                 float alpha, float beta);
    float      eval_state       (Board &b, int depth);
    void       analyze_pt       (Board &b, Stone o, Stone x, int mode);
    void       find_pt          (Board &b, int i, int j, Stone o, Stone x, string pt); 
    bool       find_pt_inline   (Board &b, int i, int j, int di, int dj, 
                                 Stone o, Stone x, string pt); 
    Stone      char_to_stone    (char ch, Stone o, Stone x);

    VTii       get_child        (Board &b, int mode);

private:
    Board*     p_board;
    Stone      cb[N][N];

public:
    MsTdVg     mpt;
    Msd        cpt;
    Msd        ccc;
    Msd        ppt;

};

#endif
