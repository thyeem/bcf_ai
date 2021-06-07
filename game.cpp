#include "game.h"

Game::Game() : auto_play(true) {
    p_board = new Board;
    p_draw  = new Draw;
}

Game::~Game() {
    delete p_board;
    delete p_draw;
}

Board *Game::gb() {
    return p_board;
}

Draw *Game::gd() {
    return p_draw;
}

char Game::main_menu() {
    clear();
    mvprintw(LINES/2-8, COLS/2-20, "SOFIA WELCOMES YOU.");
    mvprintw(LINES/2-6, COLS/2-20, "[1] PLAYING WITH HUMAN");
    mvprintw(LINES/2-4, COLS/2-20, "[2] PLAYING WITH SOFIAI");
    mvprintw(LINES/2-2, COLS/2-20, "[3] PLAYING WITH MARIAI");
    mvprintw(LINES/2+0, COLS/2-20, "[4] WATCHING A GAME BETWEEN AIs");
    mvprintw(LINES/2+2, COLS/2-20, "[q] EXIT");
    mvprintw(LINES/2+4, COLS/2-20, "PRESS THE KEY TO CONTINUE");
    char ch = getch();
    return ch;
}

void Game::select_menu(char ch) {
    // playing with human
    if ( ch == '1' ) gb()->set_player(HUMAN, HUMAN);

    // playing with SOFIAI
    if ( ch == '2' ) {
        while ( 1 ) {
            move(LINES/2+6, COLS/2-20);
            printw("DO YOU ALLOW SOFIAI TO PLAY FIRST? [y/n]: ");
            ch = getch();
            if ( ch == 'y' || ch == 'Y' ) {
                gb()->set_player(SOFIAI, HUMAN);
                return;
            } else if ( ch == 'n' || ch == 'N' ) {
                gb()->set_player(HUMAN, SOFIAI);
                return;
            }
        }
    }
    // playing with MARIAI
    if ( ch == '3' ) {
        while ( 1 ) {
            move(LINES/2+6, COLS/2-20);
            printw("DO YOU ALLOW MARIAI TO PLAY FIRST? [y/n]: ");
            ch = getch();
            if ( ch == 'y' || ch == 'Y' ) {
                gb()->set_player(MARIAI, HUMAN);
                return;
            } else if ( ch == 'n' || ch == 'N' ) {
                gb()->set_player(HUMAN, MARIAI);
                return;
            }
        }
    }

    // playing bet AIs
    if ( ch == '4' ) {
        while ( 1 ) {
            move(LINES/2+6, COLS/2-20);
            printw("DO YOU LET AIs PAUSE BETWEEN EACH THEIR MOVE? [y/n]: ");
            gb()->set_player(SOFIAI, MARIAI);
            ch = getch();
            if ( ch == 'y' || ch == 'Y' ) {
                auto_play = false;
                return;
            } else if ( ch == 'n' || ch == 'N' ) {
                auto_play = true;
                return;
            }
        }
    }
}

void Game::play_game() {
    bool   quit = false;
    int    stop = 0;
    Player player;
    Tii    grd;
    Sofiai sofia( gb() );
    Mariai maria( gb(), gd());
    gd()->init_screen_win( gb() );

    while ( !quit ) {
        grd = make_tuple(-1, -1);
        if ( gb()->whose_turn() == BLACK ) {
            player = gb()->get_player(BLACK);
        } else {
            player = gb()->get_player(WHITE);
        }

        // get next move: who plays
        if ( player == HUMAN ) {
            grd = gd()->get_move();
            if ( get<0>(grd) == -1 ) return;
        }
        if ( player == SOFIAI ) {
            grd = sofia.next_move();
            if ( !auto_play ) gd()->pause_auto_play();
        }
        if ( player == MARIAI ) {
            grd = maria.next_move();
            if ( !auto_play ) gd()->pause_auto_play();
        }

        // when missed next move
        if ( get<0>(grd) == -1 ) {
            gd()->wipe_out_msg();
            gd()->dump_msg("Fatal error occurred. next move not found. Press q to exit");
            gd()->wait_for_q();
            return;
        }

        stop = gb()->make_move(get<0>(grd), get<1>(grd));
        Board b = *gb();

        #if PUT_CANDY
        b.toggle_turn();
        maria.gen_candy(b);
        for ( auto q : maria.candy ) {
            int i = get<0>(q);
            int j = get<1>(q);
            b.set_stone(i, j, CANDY);
        }
        b.toggle_turn();
        #endif

        gd()->update_screen(stop, get<1>(grd), 2 * get<0>(grd), &b);
        if ( !stop ) {
            quit = gb()->check_quit(get<0>(grd), get<1>(grd));
            gb()->toggle_turn();
        }
    }
    gd()->dump_who_won( gb() );
}


