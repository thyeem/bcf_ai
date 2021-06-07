#include "draw.h"

Draw::Draw() {
    init_screen_std();
}

Draw::~Draw() {}

Coords Draw::get_move() {
    MEVENT event;
    int x, y;
    int w, h;
    int ch;
    bool mouse = false;
    while ( !mouse && (ch = wgetch(win)) != '\n' ) {
        if ( ch == 'q' ) return make_tuple(-1, -1);
        getmaxyx(stdscr, h, w);
        getyx(win, y, x);
        switch ( ch ) {
            case KEY_UP:
                if ( y > 0 ) wmove(win, y-1, x);
                break;
            case KEY_DOWN:
                if ( y < N-1 ) wmove(win, y+1, x);
                break;
            case KEY_LEFT:
                if ( x > 0 ) wmove(win, y, x-2);
                break;
            case KEY_RIGHT:
                if ( x < 2*(N-1) ) wmove(win, y, x+2);
                break;
            case KEY_MOUSE:
                if ( getmouse(&event) == OK && ( event.bstate & BUTTON1_PRESSED ) ) {
                    int win_x = event.x - (w-2*N)/2 -1;
                    int win_y = event.y - (h-N)/2 -1;
                    if ( win_x >= 0 && win_x < 2*N && win_y >= 0 && win_y < N ) {
                        wmove(win, win_y, win_x);
                        mouse = true;
                    }
                }
                break;
        }
    }
    getyx(win, y, x);
    return make_tuple(x/2, y);
}

void Draw::wipe_out_msg() {
    char blank[50];
    memset(blank, ' ', sizeof blank - 1);
    blank[sizeof blank - 1] = '\0';
    dump_msg(blank);
    refresh();
}

void Draw::wipe_out_prg() {
    char blank[50];
    memset(blank, ' ', sizeof blank - 1);
    blank[sizeof blank - 1] = '\0';
    move((LINES-N)/2 + 21, (COLS-2*N)/2);
    printw("%s", blank);
    refresh();
}

void Draw::dump_debug(const char* msg) {
    move((LINES-N)/2 + 23, (COLS-2*N)/2);
    printw("%s", msg);

}

void Draw::dump_board(Board *b) {
    for (int i = 0; i < N; i++ ) {
        for (int j = 0; j < N; j++ ) {
            wattron(win, COLOR_PAIR(b->get_stone(i, j)));
            if ( b->get_stone(i, j) == EMPTY ) {
                mvwaddch(win, j, 2*i, ACS_BULLET);
            } else if ( b->get_stone(i, j) == CANDY ) {
                mvwaddch(win, j, 2*i, ACS_BULLET);
            } else if ( b->get_stone(i, j) == BLACK ) {
                mvwaddch(win, j, 2*i, 'x');
            } else {
                mvwaddch(win, j, 2*i, 'o');
            }
            wattroff(win, COLOR_PAIR(b->get_stone(i, j)));

            if ( b->is_last_move(i, j) && b->moves > 0 ) {
                wattron(win, COLOR_PAIR(3));
                if ( b->get_stone(i, j) == BLACK ) mvwaddch(win, j, 2*i, 'x');
                else mvwaddch(win, j, 2*i, 'o');
                wattroff(win, COLOR_PAIR(3));
            }
        }
    }
    wrefresh(win);
}

void Draw::dump_turn(Board *b) {
    move((LINES-N)/2 - 3, (COLS-2*N)/2);
    printw("Whose Turn: ");
    if (b->whose_turn() == WHITE) {
        attron(COLOR_PAIR(1));
        addstr("Black");
        attroff(COLOR_PAIR(1));
    } else {
        attron(COLOR_PAIR(2));
        addstr("White");
        attroff(COLOR_PAIR(2));
    }
    printw("    ");
    printw("Moves: ");
    printw("%d", b->moves);
}

void Draw::dump_score(Board *b) {
    move((LINES-N)/2 - 2, (COLS-2*N)/2);
    attron(COLOR_PAIR(1));
    addstr("Black");
    attroff(COLOR_PAIR(1));
    printw(" [%s]: %d", b->get_nickname(BLACK).c_str(), b->get_score(BLACK));
    printw("    ");
    attron(COLOR_PAIR(2));
    addstr("White");
    attroff(COLOR_PAIR(2));
    printw(" [%s]: %d", b->get_nickname(WHITE).c_str(), b->get_score(WHITE));
}

void Draw::dump_progress (double pct) {
    int val = (int)(pct * 100);
    int lp  = (int)(pct * PBWIDTH);
    int rp  = (int)PBWIDTH - lp;
    if ( val >= 100 ) {
        val = 100;
        lp  = PBWIDTH;
        rp  = 0;
    }
    move((LINES-N)/2 + 21, (COLS-2*N)/2);
    printw("[%.*s%*s] %3d%%", lp, PBSTR, rp, "", val);
    refresh();
    wrefresh(win);
}

void Draw::dump_msg(const char* msg) {
    mvprintw((LINES-N)/2 - 1, (COLS-2*N)/2, "%s: %s", "Message", msg);
    refresh();
    wrefresh(win);
}

void Draw::dump_EWP(double eB, double eW) {
    mvprintw((LINES-N)/2-1, (COLS-2*N)/2, "Message: [EWP] B(%.1f%%) - W(%.1f%%)", eB, eW);
    refresh();
    wrefresh(win);
}

void Draw::dump_who_won(Board *b) {
    string winner = ( b->whose_turn() == BLACK ) ? "WHITE" : "BLACK";
    winner = winner + " won. Press q to exit";
    dump_msg(winner.c_str());
    wait_for_q();
}

void Draw::init_screen_std() {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    refresh();
    start_color();
    mousemask(BUTTON1_PRESSED, NULL);
    mouseinterval(0);
    init_pair(0, COLOR_WHITE,  COLOR_BLACK);
    init_pair(1, COLOR_BLUE,   COLOR_BLACK);
    init_pair(2, COLOR_RED,    COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_GREEN,  COLOR_BLACK);
}

void Draw::init_screen_win(Board *b) {
    clear();
    this->wrap = newwin(N+2, 2*N+3, (LINES-N)/2, (COLS-2*N)/2);
    this->win = derwin(wrap, N, 2*N, 1, 2);
    keypad(win, TRUE);
    refresh();
    box(wrap, 0, 0);
    wrefresh(wrap);
    update_screen(9, N/2, N-1, b);
}

void Draw::update_screen(int stop, int y, int x, Board *b) {
    wipe_out_msg();
    wipe_out_prg();
    if ( stop == 1 ) dump_msg("Oops! Are you serious?");
    if ( stop == 2 ) dump_msg("That's an illegal move. 3-3 connected.");
    if ( stop == 9 ) dump_msg("Get started a new game.");
    if ( stop == 0 ) dump_EWP(b->eB, b->eW);
    dump_board(b);
    dump_turn(b);
    dump_score(b);
    refresh();
    wmove(win, y, x);
    wrefresh(win);
}

void Draw::pause_auto_play() {
    dump_msg("Press space key to continue");
    if ( wait_for_s() ) return;
}

void Draw::wait_for_q() {
    char ch;
    while ( (ch = wgetch(win)) != 'q' ) {}
}

bool Draw::wait_for_s() {
    char ch;
    while ( (ch = wgetch(win)) != ' ' ) {
        if ( ch == 'q' ) return true;
    }
    return false;
}


