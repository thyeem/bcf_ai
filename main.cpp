#include "board.h"
#include "common.h"
#include "game.h"
#include "test.h"

int main() {
  while (1) {
    char ch;
    Game g;
    switch (ch = g.main_menu()) {
    case '1':
    case '2':
    case '3':
    case '4':
      g.select_menu(ch);
      g.play_game();
      endwin();
      break;
    case 'q':
      endwin();
      return 0;
    }
  }
}
