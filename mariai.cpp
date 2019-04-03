#include "mariai.h"

Mariai::Mariai(Board *b, Draw *d) : itr(0)
{
    p_board  = b;
    p_draw   = d;
    g_seed   = random_device{}();
}

Mariai::~Mariai() {}

Board* Mariai::gb() { 
    return p_board;
}

Draw* Mariai::gd() { 
    return p_draw;
}

int Mariai::fastrand() {
    g_seed = ( 214013 * g_seed + 2531011 );
    return ( g_seed >> 16 ) & 0x7FFF;
}

double Mariai::calc_ucb(Node* node) {
    if ( node->prev == NULL ) return 1;
    return ( 1. * node->win / node->visit ) + 
           UCB_C * pow( log(NPLAY) / node->visit, UCB_POW);
}

void Mariai::sort_icQ(Node* node) {
    node = node->prev;
    if ( node == NULL ) return;
    int    j = 0;
    int    size = node->icQ.size();
    char   tmp = 0;
    double tmpQ = 0;
    for ( int i = 1; i < size; i++ ) {
        tmp  = node->icQ[i];
        tmpQ = node->child[tmp].Q;
        for ( j = i; 
              j > 0 && tmpQ > node->child[node->icQ[j-1]].Q; j-- ) {
            node->icQ[j] = node->icQ[j-1];
        }
        node->icQ[j] = tmp;
    }
}

vector<size_t> Mariai::sort_icV(Node* node) {
    vector<size_t> idx(node->child.size());
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&] (const size_t a, const size_t b) 
    { 
        return node->child[a].visit > node->child[b].visit;
    });
    return idx; 
}

vector<size_t> Mariai::sort_icW(Node* node) {
    vector<size_t> idx(node->child.size());
    iota(idx.begin(), idx.end(), 0);
    sort(idx.begin(), idx.end(), [&] (const size_t a, const size_t b) 
    { 
        return node->child[a].wp > node->child[b].wp;
    });
    return idx; 
}

Node* Mariai::get_maxV_child(Node* node) {
    double max = -1;
    Node   *it = NULL;
    for ( auto &n : node->child ) {
        if ( n.visit > max &&
             n.Q     > 0 ) { 
            max = n.visit;
            it  = &n;
        }
    }
    return it;
}

Node* Mariai::get_maxW_child(Node* node) {
    double max = -1;
    Node   *it = NULL;
    for ( auto &n : node->child ) {
        if ( n.wp > max &&
             n.Q  > 0 ) { 
            max = n.wp;
            it  = &n;
        }
    }
    return it;
}

Tii Mariai::next_move() {
    Node root(NULL, make_tuple(-1, -1), EMPTY);
    roof = &root;
    itr  = 0;
    
    // Init tree: forced first expansion 
    init_tree(roof, *gb());
    // MCTS based on UCB1
    run_mcts(roof, *gb());

    #if PRINT_TREE
    print_tree(roof, 0);
    #endif 

    //return make_tuple(-1, -1);
    return pick_best(roof);
}

void Mariai::init_tree(Node* node, Board &b) {
    expand_node(node, b);
}

void Mariai::run_mcts(Node* node, Board &b) {
    for ( int n = 0; n < NPLAY; n++ ) {
        Node* head = node;
        Board vg   = b;
        bool  quit = false;

        // TREE POLICY: selection & expansion
        tie(quit, head) = select_path(head, vg);

        // DEFAULT POLICY: playout using Monte-Carlo method
        fast_rollout(vg, quit);

        // BACKUP: backpropagation
        backpropagation(head, vg.whose_turn());
        itr++;
        show_progress();
        if ( make_fast_decision(node) ) return;
	    if ( itr % PRUNING == 0 ) prune_tree(node);
    }
}

bool Mariai::move_check_quit_vg(Node* node, Board &vg) {
    Tii q = node->grd;
    int x = get<0>(q);
    int y = get<1>(q);
    vg.make_move(x, y, true);
    if ( vg.check_quit(x, y) ) return true; 
    vg.toggle_turn();
    return false;
}

tuple<bool, Node*> Mariai::select_path(Node* node, Board &vg) { 
    // TREE POLICY: selection
    while ( !node->leaf ) {
        node = &node->child[node->icQ[0]];
        if ( move_check_quit_vg(node, vg) ) 
        return make_tuple(true, node);
    }

    // TREE POLICY: expansion
    if ( is_expandable(node) ) {
        expand_node(node, vg);
        node = &node->child[node->icQ[0]];
        if ( move_check_quit_vg(node, vg) ) 
        return make_tuple(true, node);
    }
    return make_tuple(false, node);
}

bool Mariai::is_expandable(Node* node) {
    return node->visit >= NEXP;
}

void Mariai::expand_node(Node* node, Board &vg) {
    Board g = vg;
    gen_candy(g);
    int   idx = 0;
    for ( auto q : candy ) {
        insert_node(node, q, g.whose_turn());
        node->icQ.push_back(char(idx++));
    }
    node->leaf = false;
}

void Mariai::insert_node(Node* node, Tii q, Stone s) {
    node->child.push_back( Node(node, q, s) );
}

void Mariai::fast_rollout(Board &vg, bool quit) {
    while ( !quit ) {
        int x, y;
        while (1) {
            x = fastrand() % N;
            y = fastrand() % N;
            if ( !vg.make_move(x, y, false) ) break;
        }
        quit = vg.check_quit(x, y);
        if ( !quit ) vg.toggle_turn();
    }
}

void Mariai::backpropagation(Node* node, Stone turn) {
    while ( node != NULL ) {
        node->visit++;
        if ( turn == node->turn ) node->win++;
        node->Q  = calc_ucb(node);
        node->wp = 1. * node->win / node->visit;
        sort_icQ(node);
        node = node->prev;
    }
}

bool Mariai::make_fast_decision(Node* node) {
    if ( itr % FREQ_FD != 0 ) return false;
    else {
        vector<size_t> iv = sort_icV(node);
        if ( node->child[iv[0]].visit > VALUE_FD ) {
            return true;
        }
    }
    return false;
}

void Mariai::prune_tree(Node *node) {
    vector<size_t> iv = sort_icV(node);
    if ( iv.size() > PRUNE_RANK ) {
        for ( auto &nd : node->child ) {
            if ( nd.visit < node->child[iv[PRUNE_RANK-1]].visit ) {
                nd.child.clear();
                nd.leaf   = true;
                nd.Q      = -1;
                nd.wp     = -1;
            }
        }
    }
}

void Mariai::show_progress() {
    #if RUN  
    if ( itr % LINE_BUFFER == 0 ) 
        gd()->dump_progress(1.0 * itr / NPLAY);
    #endif
}

Tii Mariai::pick_best(Node* node) {
    Node* best = get_maxV_child(node);
    gb()->ewp  = 100 * best->wp;
    return best->grd;
}

void Mariai::print_tree(Node* node, int sw) {
    Node* head = node;
    string color;
    switch ( head->turn ) {
        case BLACK: color = "B";
                    break;
        case WHITE: color = "W";
                    break;
        default: color = "E";
    }
    if ( head->Q > -2 ) {
        cerr << setw(sw) << "["; 
        cerr << "(" << get<0>(head->grd) << ", " 
             << get<1>(head->grd) << ", "
             << color << "), "
             << "Q: "  << head->Q << ", "
             << "w: "  << head->win << ", "
             << "v: "  << head->visit << "]" << endl;
    }

    if ( !head->leaf ) {
        vector<size_t> idx = sort_icV(head);
        for ( auto i : idx ) {
            print_tree(&head->child[i], sw+3);
        }
    }
}

//=======================================================================

void Mariai::gen_candy(Board &b) {
    VTii sweet;
    candy.clear();
    analyze_pt(b, 1);
    if ( candy.size() < 5 ) {
        analyze_pt(b, 2);
    }

    if ( !candy.size() ) {
        analyze_pt(b, 0);
    }

    for ( auto q : candy ) {
        int i = get<0>(q);
        int j = get<1>(q);
        if ( !b.check_3_3(i, j) ) sweet.push_back(q);
    }
    candy.clear();
    candy = sweet;
}

void Mariai::analyze_pt(Board &b, int mode) {
    for ( int i = b.min_x; i <= b.max_x; i++ ) {
        for ( int j = b.min_y; j <= b.max_y; j++ ) {
            if ( b.get_stone(i, j) == EMPTY ) continue;
            find_pt_inline(b, i, j,  1,  0, mode);
            find_pt_inline(b, i, j, -1,  0, mode);
            find_pt_inline(b, i, j,  0,  1, mode);
            find_pt_inline(b, i, j,  0, -1, mode);
            find_pt_inline(b, i, j,  1,  1, mode);
            find_pt_inline(b, i, j,  1, -1, mode);
            find_pt_inline(b, i, j, -1,  1, mode);
            find_pt_inline(b, i, j, -1, -1, mode);
        }
    }
    uniq_vec(candy);
}

void Mariai::find_pt_inline(Board &b, int i, int j, int di, int dj, int mode) { 
    if ( mode == 0 ) {
        find_pt_each(b, i, j, di, dj, "sa"    );
    } else if ( mode == 1 ) {
        find_pt_each(b, i, j, di, dj, "xxa"   ) ||
        find_pt_each(b, i, j, di, dj, "ooa"   ) ||
        find_pt_each(b, i, j, di, dj, "xax"   ) ||
        find_pt_each(b, i, j, di, dj, "oao"   );
        find_pt_each(b, i, j, di, dj, "x_xa"  ) ||
        find_pt_each(b, i, j, di, dj, "o_oa"  );
        find_pt_each(b, i, j, di, dj, "xooo_a") ||
        find_pt_each(b, i, j, di, dj, "oxxx_a");
    } else if ( mode == 2 ) {
        find_pt_each(b, i, j, di, dj, "x=a"   ) ||
        find_pt_each(b, i, j, di, dj, "o=a"   );
    }
}

bool Mariai::find_pt_each(Board &b, int i, int j, int di, int dj, string pt) {
    int size = pt.length();
    char q = pt[0];
    char e = ( q == 'o' ) ? 'x' :
             ( q == 'x' ) ? 'o' : '_';
    for ( int s = 0; s < size; s++ ) {
        int x = i + s * di;
        int y = j + s * dj;
        if ( !b.in_range(x, y) ) return false;
        if ( !match_stones(b, pt[s], x, y) ) return false;

        if ( pt[s] == '=' ) {
            if ( di * dj != 0 ) return false;
            int found = 0;
            for ( int l = -1; l <= 1; l++ ) {
                for ( int m = -1; m <= 1; m++ ) {
                    if ( !b.in_range(x+l, y+m) ) continue;
                    if ( on_main_axis(l, m, di, dj) ) continue;
                    if ( match_stones(b, e, x+l, y+m) ) found++; 
                }
            }
            if ( !found ) return false;
        }
        // q++ method
        if ( pt[s] == '+' ) {
            if ( di * dj != 0 ) return false;
            for ( int l = -1; l <= 1; l++ ) {
                for ( int m = -1; m <= 1; m++ ) {
                    if ( !b.in_range(x+l, y+m) ) continue;
                    if ( on_main_axis(l, m, di, dj) &&
                         !match_stones(b, q, x+l, y+m) ) return false;
                }
            }
            for ( int l = -1; l <= 1; l++ ) {
                for ( int m = -1; m <= 1; m++ ) {
                    if ( !b.in_range(x+l, y+m) ) continue;
                    if ( !on_main_axis(l, m, di, dj) &&
                         b.get_stone(x+l, y+m) == EMPTY ) 
                        candy.push_back(make_tuple(x+l, y+m));
                }
            }
            return false;
        }
    }
    // piling up candies
    for ( int s = 0; s < size; s++ ) {
        int x = i + s * di;
        int y = j + s * dj;
        if ( pt[s] == 'a' ) 
            candy.push_back(make_tuple(x, y));
    }
    return true;
}


bool Mariai::on_main_axis(int x, int y, int di, int dj) {
    if ( di ==  0  && x ==  0 ) return true;
    if ( dj ==  0  && y ==  0 ) return true;
    if ( di == -dj && x == -y ) return true;
    if ( di ==  dj && x ==  y ) return true;
    return false;
}

bool Mariai::on_crux_wing(int x, int y, int di, int dj) {
    if ( di * dj == 0 && x * y != 0 ) return true;
    if ( di * dj != 0 && x * y == 0 ) return true;
    return false;
} 

bool Mariai::on_perp_wing(int x, int y, int di, int dj) {
    if ( di * dj == 0 && x * y == 0 ) return true;
    if ( di * dj != 0 && x * y != 0 ) return true;
    return false;
}

bool Mariai::match_stones(Board &b, char ch, int i, int j) {
    switch ( ch ) {
        case '+':
        case 's': return ( b.get_stone(i, j) != EMPTY ) ? true : false;
                  break;
        case 'a': 
        case '=': 
        case '_': return ( b.get_stone(i, j) == EMPTY ) ? true : false;
                  break;
        case 'x': return ( b.get_stone(i, j) == BLACK ) ? true : false;
                  break;
        case 'o': return ( b.get_stone(i, j) == WHITE ) ? true : false;
                  break;
        default:  return false;
    }
}

