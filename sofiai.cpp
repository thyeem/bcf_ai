#include "sofiai.h"

Sofiai::Sofiai(Board *b) {
    p_board = b;
    set_coeff();
}

Sofiai::~Sofiai() {}

void Sofiai::set_coeff() {
    ccc[ "depth"  ] = +1.0e2;
    ccc[ "score"  ] = +1.2e4;
   
    // score =========================
    cpt[ "_xxxxa" ] = -5.0e6;
    cpt[ "_ooooa" ] = +5.0e6;
    cpt[ "xooooa" ] = +3.0e5;
    cpt[ "oxxxxa" ] = -1.0e5;
    cpt[ "xooa"   ] = -1.0e4;
    cpt[ "oxxa"   ] = +1.0e4;
    cpt[ "_xxxa"  ] = -2.0e4;
    cpt[ "_oooa"  ] = +2.0e4;
   
    // move priority =================
    ppt[ "xxxxa" ]  = 1;
    ppt[ "ooooa" ]  = 1;
    ppt[ "xxxa"  ]  = 2;
    ppt[ "oooa"  ]  = 2;
    ppt[ "xxx_a" ]  = 3;
    ppt[ "ooo_a" ]  = 3;
    ppt[ "xax"   ]  = 4;
    ppt[ "oao"   ]  = 4;
    ppt[ "_xxa"  ]  = 5;
    ppt[ "_ooa"  ]  = 5;
    ppt[ "_x_xa" ]  = 5;
    ppt[ "_o_oa" ]  = 5;
    ppt[ "xooa"  ]  = 6;
    ppt[ "oxxa"  ]  = 6;
}

Board *Sofiai::gb() { 
    return p_board;
}

//=======================================================================

Tii Sofiai::next_move() {
    if ( gb()->moves == 0 ) return make_tuple(N/2, N/2);
    double bestval;
    int x, y;
    tie(bestval, x, y) = minimax((*gb()), -1, -1, 0, true, -INF, +INF);
    return make_tuple(x, y);
}

Tfii Sofiai::minimax(Board b, int x, int y, int depth, 
                      bool is_maximizer, float alpha, float beta) {
    float val;
    float bestval;
    int   bestx;
    int   besty;
    VTii  child; 

    if ( x != -1 ) { 
        // called when depth >= 1
        b.make_move(x, y);
        if ( b.check_quit(x, y) ) {
            if ( is_maximizer ) {
                // negative 
                // Entering here implies the game was ended by the previous move
                bestval = -INF + ccc["depth"] * depth;
            } else {
                // positive
                bestval = +INF - ccc["depth"] * depth;
            }
            return make_tuple(bestval, x, y);
        }

        // evaluate the state when the game is not over
        if ( depth == DEPTH ) {
            bestval = eval_state(b, depth);
            return make_tuple(bestval, x, y);
        }
        b.toggle_turn();
    }

    // getting child
    child = get_child(b, 1);

    if ( is_maximizer ) {
        bestval = -INF;
        for ( auto r : child ) {
            int i = get<0>(r);
            int j = get<1>(r);
            tie(val, x, y) = minimax(b, i, j, depth+1, false, alpha, beta);
            if ( depth == 0 ) {
                if ( val > bestval ) {
                    bestval = val;
                    bestx   = i;
                    besty   = j;
                }
            } else {
                bestval = fmax(bestval, val);
            }
            alpha = fmax(alpha, bestval);
            if ( beta <= alpha ) break;
        }
    } else {
        bestval = +INF;
        for ( auto r : child ) {
            int i = get<0>(r);
            int j = get<1>(r);
            tie(val, x, y) = minimax(b, i, j, depth+1, true, alpha, beta);
            bestval = fmin(bestval, val);
            beta    = fmin(beta, bestval);
            if ( beta <= alpha ) break;
        }
    }
    return make_tuple(bestval, bestx, besty);
}


float Sofiai::eval_state(Board &b, int depth) {
    // eval_state always gonna be called at a minimizer's turn
    // maximizer's stone: o
    Stone x = b.whose_turn();
    Stone o = ( x == BLACK ) ? WHITE : BLACK;
    float f_depth = -ccc["depth"] * depth;
    float f_score = +ccc["score"] * ( pow(1. * b.get_score(o), 1.4) - 
                                      pow(1. * b.get_score(x), 1.4) );
    float f_pt    = 0;

    analyze_pt(b, o, x, 2);
    for ( auto r : cpt ) {
        f_pt += cpt[r.first] * get<0>(mpt[r.first]);
    }
    return f_depth + f_score + f_pt;
}

void Sofiai::analyze_pt(Board &b, Stone o, Stone x, int mode) {
    mpt.clear();
    if ( mode == 2 ) {
    // evaluate scoring
        for ( int i = 0; i < N; i++ ) {
            for ( int j = 0; j < N; j++ ) {
                if ( b.get_stone(i, j) == EMPTY ) {
                    find_pt(b, i, j, o, x, "_oooa"  );
                    find_pt(b, i, j, o, x, "_ooooa" );
                    find_pt(b, i, j, o, x, "_xxxa"  );
                    find_pt(b, i, j, o, x, "_xxxxa" );
                } else {
                    find_pt(b, i, j, o, x, "oxxa"   );
                    find_pt(b, i, j, o, x, "xooooa" );
                    find_pt(b, i, j, o, x, "xooa"   );
                }
            }
        }
    } else if ( mode == 1 ) {
    // finding next move candidates (child)
        for ( int i = 0; i < N; i++ ) {
            for ( int j = 0; j < N; j++ ) {
                if ( b.get_stone(i, j) == EMPTY ) {
                    find_pt(b, i, j, o, x, "_ooa"  );
                    find_pt(b, i, j, o, x, "_xxa"  );
                    find_pt(b, i, j, o, x, "_x_xa" );
                    find_pt(b, i, j, o, x, "_o_oa" );
                } else {
                    find_pt(b, i, j, o, x, "oooa"  );
                    find_pt(b, i, j, o, x, "ooooa" );
                    find_pt(b, i, j, o, x, "oxxa"  );
                    find_pt(b, i, j, o, x, "xxxa"  );
                    find_pt(b, i, j, o, x, "xxxxa" );
                    find_pt(b, i, j, o, x, "xooa"  );
                    find_pt(b, i, j, o, x, "oao"   );
                    find_pt(b, i, j, o, x, "xax"   );
                    find_pt(b, i, j, o, x, "ooo_a" );
                    find_pt(b, i, j, o, x, "xxx_a" );
                }
            }
        }   
    } else {
    // when child.size() == 0
        for ( int i = 0; i < N; i++ ) {
            for ( int j = 0; j < N; j++ ) {
                if ( b.get_stone(i, j) == EMPTY ) continue;
                find_pt(b, i, j, o, x, "oa" );
                find_pt(b, i, j, o, x, "xa" );
            }
        }
    }
}

void Sofiai::find_pt(Board &b, int i, int j, Stone o, Stone x, string pt) { 
    float count = 0;
    if ( find_pt_inline(b, i, j,  1,  0, o, x, pt) ) count++;  
    if ( find_pt_inline(b, i, j, -1,  0, o, x, pt) ) count++;  
    if ( find_pt_inline(b, i, j,  0,  1, o, x, pt) ) count++;  
    if ( find_pt_inline(b, i, j,  0, -1, o, x, pt) ) count++;  
    if ( find_pt_inline(b, i, j,  1,  1, o, x, pt) ) count++;  
    if ( find_pt_inline(b, i, j,  1, -1, o, x, pt) ) count++;  
    if ( find_pt_inline(b, i, j, -1,  1, o, x, pt) ) count++;  
    if ( find_pt_inline(b, i, j, -1, -1, o, x, pt) ) count++;  
    if ( count ) get<0>(mpt[pt]) += count;
}

bool Sofiai::find_pt_inline(Board &b, int i, int j, int di, int dj, 
                            Stone o, Stone x, string pt) {
    int size = pt.length();
    VTii *vg = &get<1>(mpt[pt]);

    for ( int s = 0; s < size; s++ ) {
        if ( !b.in_range(i + s*di, j + s*dj) ) return false;
        if ( b.get_stone(i + s*di, j + s*dj) != 
                  char_to_stone(pt[s], o, x) ) return false; 
    }

    for ( int s = 0; s < size; s++ ) {
        if ( pt[s] == 'a' ) {
            vg->emplace_back(i + s*di, j + s*dj);
        }
    }
    return true;
}

//=======================================================================

VTii Sofiai::get_child(Board &b, int mode) {
    VTii child;
    Mgf  mch;
    VTfg vch;
    Stone x = b.whose_turn();
    Stone o = ( x == BLACK ) ? WHITE : BLACK;
    analyze_pt(b, o, x, mode);

    for ( auto r : mpt ) {
        string pt = r.first;
        VTii    v = get<1>(mpt[pt]);
        for ( auto q : v ) {
            int i = get<0>(q);
            int j = get<1>(q);
            if ( !b.check_3_3(i, j) ) {
                if ( ( !mch[q] ) || 
                     ( mch[q] > ppt[pt] ) ) mch[q] = ppt[pt];
            }
        }
    }
  
    // sort move candidates by priority value
    for ( auto r : mch ) {
        vch.emplace_back(r.second, r.first);
    }
    sort(vch.begin(), vch.end());
    for ( auto r : vch ) {
        child.push_back(get<1>(r));
    }

    if ( !child.size() ) {
        random_device rd;
        mt19937 g(rd());
        child = get_child(b, 0);
        shuffle(child.begin(), child.end(), g);
    }
    return child;
}

Stone Sofiai::char_to_stone(char ch, Stone o, Stone x) {
    if ( ch == 'o' ) {
        return o;
    } else if ( ch == 'x' ) {
        return x;
    } else {
        return EMPTY;
    }
}

