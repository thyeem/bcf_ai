#include "test.h"

Test::Test() : dump_file("log/match_" + to_string(time(NULL)) + ".txt"), 
               fout(dump_file, ios::app),
               games(0),
               wins(0) {} 
Test::~Test() {
    fout.close();
}

void Test::run_test() {
    if ( AI_MATCH ) match_bet_AIs();
    if ( AI_QUIZ )  solve_quiz(QUIZ_NUM, PUT_BOARD);
}

void Test::match_bet_AIs() {
    print_param_info();
    for ( size_t i = 0; i < MATCH_NUM; i++ ) {
        if ( match_play(i+1) ) wins++;
        games++;
    }
    print_stats(true);
}

bool Test::match_play(int nth) {
    Board  b;
    int    lines = 24;
    Sofiai black( &b );
    Mariai white( &b, NULL );
    b.set_player(SOFIAI, MARIAI); 

    size_t start_at = time(NULL);
    bool   quit     = false;
    int    stop     = 0;
    int    pass     = 0;
    Tii    grd;

    while ( !quit ) {
        if ( !stop ) b.toggle_turn();
        if ( b.whose_turn() == BLACK ) {
            if ( !b.moves ) grd = make_tuple(N/2, N/2);
            else grd = black.next_move();
        } else {
            if ( !b.moves ) grd = make_tuple(N/2, N/2);
            else grd = white.next_move();
        }
        stop = b.make_move(get<0>(grd), get<1>(grd));

        if ( !stop ) {
            print_per_move(b, lines, pass, nth);
            quit = b.check_quit(get<0>(grd), get<1>(grd));
        }
    }
    print_per_game(b, lines, pass, nth, start_at);
    if ( b.get_player(b.whose_turn()) == MARIAI ) return true;
    else return false;
}


void Test::rewind_stdout(int lines, int pass) {
    if ( !pass ) return;
    for ( int l = 0; l < lines; l++ ) { 
        fputs("\033[A\033[2K", stdout); 
    }
}

void Test::print_param_info() {
    fout << setw(15) << "PLAYOUTS"   << " | " << PLAYOUTS   << endl;
    fout << setw(15) << "NEXP"       << " | " << NEXP       << endl;
    fout << setw(15) << "UCB_C"      << " | " << UCB_C      << endl;
    fout << setw(15) << "UCB_POW"    << " | " << UCB_POW    << endl;
    cout << setw(15) << "PLAYOUTS"   << " | " << PLAYOUTS   << endl;
    cout << setw(15) << "NEXP"       << " | " << NEXP       << endl;
    cout << setw(15) << "UCB_C"      << " | " << UCB_C      << endl;
    cout << setw(15) << "UCB_POW"    << " | " << UCB_POW    << endl;
}

void Test::print_per_move(Board &b, int &lines, int &pass, int nth) {
    Stone  fs   = ( b.whose_turn() == BLACK ) ? WHITE : BLACK;
    string turn = ( fs == BLACK ) ? "B" : "W";
//     double ewp  = ( fs == BLACK ) ? 100 - b.ewp : b.ewp;

    rewind_stdout(lines, pass);
    print_stats(false);
    cout << setw(3)   << nth     << " | ";
    cout << "moves: " << b.moves << ", ";
    cout << "turn: "  << b.get_nickname(fs) << "(" << turn << ", ";
//     cout << setprecision(1) << fixed << ewp << "%), ";
    cout << "BP: "    << b.get_score(BLACK) << ", ";
    cout << "WP: "    << b.get_score(WHITE) << endl;
    b.print_board(true);
    pass++;
}

void Test::print_stats(bool fdump) {
    double wp;
    if ( !games ) wp = 0;
    else wp = 100. * wins / games;
    cout << setw(3)   << "ST" << " | ";
    cout << "Mariai: "<< wins << " / " << games << " (";
    cout << setprecision(1) << fixed << wp << "%)" << endl;
    if ( fdump) {
        fout << setw(3)   << "ST" << " | ";
        fout << "Mariai WP: "<< wins << " / " << games << " (";
        fout << setprecision(1) << fixed << wp << "%)" << endl;
    }
}

void Test::print_per_game(Board &b, int &lines, int &pass, int nth, int start_at) {
    string nickname;
    string turn;
    if ( b.whose_turn() == BLACK ) {
        nickname = b.get_nickname(BLACK);
        turn = "B, ";
    } else {
        nickname = b.get_nickname(WHITE);
        turn = "W, ";
    }

    rewind_stdout(lines, pass);
    cout << setw(3) << nth        << " | ";
    cout << turn    << nickname   << " wins, " 
         << b.moves << " moves. " << flush;
    cout << setprecision(2)       << fixed 
         << ( time(NULL) - start_at ) / 60. << " mins." << endl;

    fout << setw(3) << nth        << " | ";
    fout << turn << nickname      << " wins, " 
         << b.moves << " moves. " << flush;
    fout << setprecision(2)       << fixed 
         << ( time(NULL) - start_at ) / 60. << " mins." << endl;

}

void Test::set_quizN(Board &b, int quizN) {
    if ( quizN == 1 ) {
        b.set_stone( 4,  4, BLACK);
        b.set_stone( 8,  8, WHITE);
        b.set_stone( 2,  2, BLACK);
        b.set_stone( 9,  9, WHITE);
        b.set_stone( 3,  3, BLACK);
        b.set_stone(10, 12, WHITE);
        b.set_stone(12, 12, BLACK);
    }

    else if ( quizN == 2 ) {
        b.set_stone( 8,  8, BLACK);
        b.set_stone( 7,  8, WHITE);
        b.set_stone( 8,  9, BLACK);
        b.set_stone( 8,  7, WHITE);
        b.set_stone( 8, 10, BLACK);
        b.set_stone( 6,  9, WHITE);
        b.set_stone(10,  9, BLACK);
        b.set_stone( 4,  9, WHITE);
        b.set_stone( 6,  8, BLACK);
        b.set_stone( 8, 11, WHITE);
        b.set_stone(10, 14, BLACK);
        b.set_stone( 7, 12, WHITE);
        b.set_stone(14, 14, BLACK);
        b.set_stone( 9, 10, WHITE);
        b.toggle_turn();
    }    

    else if ( quizN == 3 ) {
        b.set_stone( 5,  5, BLACK);
        b.set_stone( 5,  6, BLACK);
        b.set_stone( 5,  7, BLACK);
        b.set_stone( 5,  8, BLACK);
        b.set_stone( 4, 10, BLACK);
        b.set_stone( 6,  5, BLACK);
        b.set_stone( 7,  5, BLACK);
        b.set_stone( 7,  7, BLACK);
        b.set_stone( 6,  9, BLACK);
        b.set_stone( 5,  4, WHITE);
        b.set_stone( 5,  9, WHITE);
        b.set_stone( 6,  6, WHITE);
        b.set_stone( 6,  7, WHITE);
        b.set_stone( 6,  8, WHITE);
        b.set_stone( 6, 10, WHITE);
    }    

    else if ( quizN == 4 ) {
        b.set_stone( 5,  5, BLACK);
        b.set_stone( 5,  7, BLACK);
        b.set_stone( 4,  9, BLACK);
        b.set_stone( 3, 11, BLACK);
        b.set_stone( 6, 10, BLACK);
        b.set_stone( 7,  4, BLACK);
        b.set_stone( 7,  6, BLACK);
        b.set_stone( 7,  7, BLACK);
        b.set_stone( 7,  8, BLACK);
        b.set_stone( 9,  5, BLACK);
        b.set_stone(10,  9, BLACK);
        b.set_stone( 4,  5, WHITE);
        b.set_stone( 4, 10, WHITE);
        b.set_stone( 2, 11, WHITE);
        b.set_stone( 5,  6, WHITE);
        b.set_stone( 5,  9, WHITE);
        b.set_stone( 6,  5, WHITE);
        b.set_stone( 7,  5, WHITE);
        b.set_stone( 8,  5, WHITE);
        b.set_stone( 8,  9, WHITE);
        b.set_stone(10,  5, WHITE);
        b.set_stone( 6,  8, WHITE);
    }    

    else if ( quizN == 5 ) {
        b.set_stone( 4,  9, BLACK);
        b.set_stone( 5,  8, BLACK);
        b.set_stone( 6,  7, BLACK);
        b.set_stone( 7,  6, BLACK);
        b.set_stone( 3, 10, WHITE);
        b.set_stone( 6,  6, WHITE);
        b.set_stone( 8,  5, WHITE);
    }    

    else if ( quizN == 6 ) {
        b.set_stone( 6,  8, BLACK);
        b.set_stone(10,  8, BLACK);
        b.set_stone(10, 10, BLACK);
        b.set_stone(10, 12, BLACK);
        b.set_stone(11,  8, BLACK);
        b.set_stone(11, 10, BLACK);
        b.set_stone(12,  8, BLACK);
        b.set_stone(12, 10, BLACK);
        b.set_stone( 5,  7, WHITE);
        b.set_stone( 7,  9, WHITE);
        b.set_stone( 8, 10, WHITE);
        b.set_stone( 9,  7, WHITE);
        b.set_stone( 9, 11, WHITE);
        b.set_stone(10,  9, WHITE);
        b.set_stone(13, 10, WHITE);
    }  

    else if ( quizN == 7 ) {
        b.set_stone( 9,  9, BLACK);
        b.set_stone( 7,  7, BLACK);
        b.set_stone( 7,  9, BLACK);
        b.set_stone( 9,  7, BLACK);
        b.set_stone( 8,  5, BLACK);
        b.set_stone( 8,  8, WHITE);
        b.set_stone( 5,  8, WHITE);
        b.set_stone( 6,  7, WHITE);
        b.set_stone( 7,  6, WHITE);
    }
    else if ( quizN == 8 ) {
        b.set_stone( 7,  8, BLACK);
//        b.set_stone( 7,  9, BLACK);
//        b.set_stone( 4,  6, BLACK);
//        b.set_stone( 5,  6, BLACK);
        b.set_stone( 6,  8, WHITE);
//        b.set_stone( 6, 10, WHITE);
//        b.set_stone( 8,  9, WHITE);
        b.set_stone( 3,  3, WHITE);
    }
}

void Test::solve_quiz(int quizN, bool put_board) {
    Draw  *d = NULL;
    Board  b;
    set_quizN(b, quizN);
    Mariai m( &b, d );

//    b.moves = 10;
    Tii g = m.next_move();
    m.gen_candy(b);
    b.candy = m.candy;
    cout << "Best move: " << setw(2) << get<0>(g) << " " 
                          << setw(2) << get<1>(g) << endl;
    if ( put_board ) b.print_board(true);
}

