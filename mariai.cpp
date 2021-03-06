#include "mariai.h"

Mariai::Mariai(Board *b, Draw *d) : itr(0)
{
    p_board = b;
    p_draw = d;
    g_seed = random_device{}();
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
           UCB_C * pow( log(PLAYOUTS) / node->visit, UCB_POW);
}

void Mariai::sort_icQ(Node* node) {
    node = node->prev;
    if ( node == NULL ) return;
    int j = 0;
    int size = node->icQ.size();
    char tmp = 0;
    double tmpQ = 0;
    for ( int i = 1; i < size; i++ ) {
        tmp = node->icQ[i];
        tmpQ = node->child[tmp].Q;
        for ( j = i; j > 0 && tmpQ > node->child[node->icQ[j-1]].Q; j-- ) {
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
    Node *it = NULL;
    for ( auto &n : node->child ) {
        if ( n.visit > max && n.Q > 0 ) { 
            max = n.visit;
            it  = &n;
        }
    }
    return it;
}

Node* Mariai::get_maxW_child(Node* node) {
    double max = -1;
    Node *it = NULL;
    for ( auto &n : node->child ) {
        if ( n.wp > max && n.Q > 0 ) { 
            max = n.wp;
            it  = &n;
        }
    }
    return it;
}

Tii Mariai::next_move() {
    if ( gb()->moves == 0 ) return make_tuple(N/2, N/2);
    Node root(NULL, make_tuple(-1, -1), EMPTY);
    Node* roof = &root;
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

void Mariai::init_tree(Node* roof, Board &b) {
    expand_node(roof, b);
}

void Mariai::run_mcts(Node* roof, Board &b) {
    int i = (b.moves == 1) ? 3*PLAYOUTS/4 : 0;
    for ( int n = i; n < PLAYOUTS; n++ ) {
        Node* head = NULL;
        Board vg   = b;
        bool  quit = false;

        // TREE POLICY: selection & expansion
        tie(quit, head) = select_path(roof, vg);

        // DEFAULT POLICY: playout using Monte-Carlo method
        fast_rollout(vg, quit);

        // BACKUP: backpropagation
        quit = backpropagation(head, roof, vg.whose_turn());
        if ( quit ) return;
        else itr++;
        // if ( itr > ITER_PRUNING ) prune_tree(roof);
        show_progress();
    }
}

tuple<bool, Node*> Mariai::select_path(Node* node, Board &vg) { 
    // TREE POLICY: selection
    while ( !node->leaf ) {
        node = &node->child[node->icQ[0]];
        if ( move_check_quit_vg(node, vg) ) return make_tuple(true, node);
    }

    // TREE POLICY: expansion
    if ( is_expandable(node) ) { 
        expand_node(node, vg);
        node = &node->child[node->icQ[0]];
        if ( move_check_quit_vg(node, vg) ) return make_tuple(true, node);
    }
    return make_tuple(false, node);
}

void Mariai::expand_node(Node* node, Board &vg) {
    Board g = vg;
    gen_candy(g);
    int idx = 0;
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
            if ( !vg.make_move(x, y) ) break;
        }
        quit = vg.check_quit(x, y);
        if ( !quit ) vg.toggle_turn();
    }
}

bool Mariai::backpropagation(Node* node, Node* roof, Stone turn) {
    while ( node != NULL ) {
        node->visit++;
        // fast-decision
        if ( node != roof && node->visit > ITER_DECISION ) return true;
        if ( turn == node->turn ) node->win++;
        node->Q  = calc_ucb(node);
        node->wp = 1. * node->win / node->visit;
        sort_icQ(node);
        node = node->prev;
    }
    return false;
}

bool Mariai::is_expandable(Node* node) {
    return node->leaf && node->visit >= NEXP;
}

bool Mariai::move_check_quit_vg(Node* node, Board &vg) {
    Tii q = node->grd;
    int x = get<0>(q);
    int y = get<1>(q);
    vg.make_move(x, y);
    if ( vg.check_quit(x, y) ) return true; 
    vg.toggle_turn();
    return false;
}

void Mariai::show_progress() {
    if ( gd() == NULL ) return;
    if ( itr % LINE_BUFFER == 0 ) 
        gd()->dump_progress(1.0 * itr / PLAYOUTS);
}

Tii Mariai::pick_best(Node* node) {
    Node* best = get_maxV_child(node);
    gb()->eB = (best->turn == BLACK) ? 100*best->wp : 100*(1-best->wp);
    gb()->eW = 100 - gb()->eB;
    return best->grd;
}

void Mariai::prune_tree(Node *node) {
    vector<size_t> iv = sort_icV(node);
    if ( iv.size() > PRUNE_RANK ) {
        for ( auto &nd : node->child ) {
            if ( nd.visit < node->child[iv[PRUNE_RANK-1]].visit ) {
                nd.child.clear();
                nd.leaf = true;
                nd.Q    = -1;
                nd.wp   = -1;
            }
        }
    }
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
        cout << setw(sw) << "["; 
        cout << "(" << get<0>(head->grd) << ", " 
             << get<1>(head->grd) << ", " << color << "), "
             << "Q: " << head->Q << ", "
             << "w: " << head->win << ", "
             << "v: " << head->visit << "]" << endl;
    }
    if ( !head->leaf ) {
        vector<size_t> idx = sort_icV(head);
        for ( auto i : idx ) {
            print_tree(&head->child[i], sw+3);
        }
    }
}

// POLICY: NEXT MOVE CANDIDATES =========================================

void Mariai::gen_candy(Board &b) {
    VTii sweet;
    candy.clear();
    b.update_density();
    analyze_pattern(b, NORMAL);
    if ( (candy.size() < 5) || 
         (b.moves < 15 && b.density < CUT_DENSITY) ) analyze_pattern(b, SHORT);
    if ( !candy.size() ) analyze_pattern(b, NIL);
}

void Mariai::refine_candy(Board &b) {
    for ( auto it = candy.begin(); it != candy.end(); ) {
        int i, j;
        tie(i, j) = (*it);
        if ( b.check_3_3(i, j) ) it = candy.erase(it);
        else ++it;
    }
}

void Mariai::analyze_pattern(Board &b, Mode mode) {
    for ( int i = 0; i < N; i++ ) {
        for ( int j = 0; j < N; j++ ) {
            if ( b.get_stone(i, j) == EMPTY ) continue;
            find_pattern_inline(b, i, j,  1,  0, mode);
            find_pattern_inline(b, i, j, -1,  0, mode);
            find_pattern_inline(b, i, j,  0,  1, mode);
            find_pattern_inline(b, i, j,  0, -1, mode);
            find_pattern_inline(b, i, j,  1,  1, mode);
            find_pattern_inline(b, i, j,  1, -1, mode);
            find_pattern_inline(b, i, j, -1,  1, mode);
            find_pattern_inline(b, i, j, -1, -1, mode);
        }
    }
    uniq_vec(candy);
    refine_candy(b);
}

void Mariai::find_pattern_inline(Board &b, int i, int j, int di, int dj, Mode mode) { 
    if ( mode == NIL ) {
        find_pattern_each(b, i, j, di, dj, "sa");
    } else if ( mode == NORMAL ) {
        if ( b.get_stone(i, j) == BLACK ) {
            find_pattern_each(b, i, j, di, dj, "xxa"    ) ||
            find_pattern_each(b, i, j, di, dj, "xaxa"   ) || 
            find_pattern_each(b, i, j, di, dj, "xax"    ) || 
            find_pattern_each(b, i, j, di, dj, "xooo_a" );
        } else {
            find_pattern_each(b, i, j, di, dj, "ooa"    ) ||
            find_pattern_each(b, i, j, di, dj, "oaoa"   ) || 
            find_pattern_each(b, i, j, di, dj, "oao"    ) || 
            find_pattern_each(b, i, j, di, dj, "oxxx_a" );
        }
    } else if ( mode == SHORT ) {
        find_pattern_each(b, i, j, di, dj, "?=a"  );
    }
}

bool Mariai::find_pattern_each(Board &b, int i, int j, int di, int dj, string pt) {
    int size = pt.length();
    char turn = ( b.whose_turn() == BLACK ) ? 'x' : 'o';
    char enemy = ( turn == 'x' ) ? 'o' : 'x';
    for ( int s = 0; s < size; s++ ) {
        int x = i + s * di;
        int y = j + s * dj;
        char stone = ( pt[s] == '?' ) ? turn : pt[s];
        if ( !b.in_range(x, y) ) return false;
        if ( !match_stones(b, stone, x, y) ) return false;
        if ( stone == '=' ) {
            if ( di * dj != 0 ) return false;
            if ( !(match_stones(b, enemy, x-1, y-1) ||
                   match_stones(b, enemy, x-1, y+0) ||
                   match_stones(b, enemy, x-1, y+1) ||
                   match_stones(b, enemy, x+0, y-1) ||
                   match_stones(b, enemy, x+0, y+1) ||
                   match_stones(b, enemy, x+1, y-1) ||
                   match_stones(b, enemy, x+1, y+0) ||
                   match_stones(b, enemy, x+1, y+1)) ) {
                return false;
            }
        }
    }
    // piling up candies
    for ( int s = 0; s < size; s++ ) {
        int x = i + s * di;
        int y = j + s * dj;
        if ( pt[s] == 'a' ) candy.push_back(make_tuple(x, y));
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

