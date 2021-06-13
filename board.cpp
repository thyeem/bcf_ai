#include "board.h"

Board::Board()
    : turn(BLACK), scoreB(0), scoreW(0), last_x(-1), last_y(-1), moves(0),
      inf_x(N / 2 - 2), sup_x(N / 2 + 2), inf_y(N / 2 - 2), sup_y(N / 2 + 2),
      eB(50.0), eW(50.0) {
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      board[i][j] = EMPTY;
    }
  }
}

Board::~Board() {}

Stone Board::get_stone(int x, int y) { return board[x][y]; }

void Board::set_stone(int x, int y, Stone s) {
  board[x][y] = s;
  if (x - 2 < inf_x)
    inf_x = (x - 2 < 0) ? 0 : x - 2;
  if (x + 2 > sup_x)
    sup_x = (x + 2 >= N) ? N - 1 : x + 2;
  if (y - 2 < inf_y)
    inf_y = (y - 2 < 0) ? 0 : y - 2;
  if (y + 2 > sup_y)
    sup_y = (y + 2 >= N) ? N - 1 : y + 2;
}

int Board::get_score(Stone s) { return (s == BLACK) ? scoreB : scoreW; }

void Board::set_score(Stone s, int score) {
  if (s == BLACK) {
    scoreB = score;
  } else {
    scoreW = score;
  }
}

bool Board::is_last_move(int x, int y) { return (x == last_x && y == last_y); }

void Board::set_last_move(int x, int y) {
  last_x = x;
  last_y = y;
}

void Board::toggle_turn() {
  turn = (turn == BLACK) ? WHITE : BLACK;
  last = (turn == BLACK) ? WHITE : BLACK;
}

Stone Board::last_turn() { return last; }

Stone Board::whose_turn() { return turn; }

string Board::get_nickname(Stone s) {
  Player player = (s == BLACK) ? playerB : playerW;
  switch (player) {
  case HUMAN:
    return "Human";
    break;
  case SOFIAI:
    return "Sofiai";
    break;
  case MARIAI:
    return "Mariai";
    break;
  default:
    return "Unknown";
  }
}

Player Board::get_player(Stone s) { return (s == BLACK) ? playerB : playerW; }

void Board::set_player(Player pB, Player pW) {
  playerB = pB;
  playerW = pW;
}

int Board::make_move(int x, int y) {
  if (board[x][y] != EMPTY) {
    return 1;
  } else {
    if (check_3_3(x, y))
      return 2;
    moves++;
    set_stone(x, y, turn);
    set_last_move(x, y);
    if (BCF)
      bite_move(x, y);
    return 0;
  }
}

bool Board::in_range(int x, int y) {
  if (x < 0 || x >= N)
    return 0;
  if (y < 0 || y >= N)
    return 0;
  return 1;
}

bool Board::check_quit(int x, int y) {
  if (scoreB >= WP)
    return true;
  if (scoreW >= WP)
    return true;
  if (is_nstones_made(x, y, 0, GOAL))
    return true;
  if (is_nstones_made(x, y, 1, GOAL))
    return true;
  if (is_nstones_made(x, y, 2, GOAL))
    return true;
  if (is_nstones_made(x, y, 3, GOAL))
    return true;
  return false;
}

bool Board::is_nstones_made(int x, int y, int npi_over_four, int n) {
  // npi_over_four in Z/4Z = {0, 1, 2, 3}
  // meaning the angle of n * pi/4 with the line

  switch (npi_over_four) {
  case 0:
    return count_straight(x, y, 1, 0) + count_straight(x, y, -1, 0) + 1 == n;
  case 1:
    return count_straight(x, y, 1, 1) + count_straight(x, y, -1, -1) + 1 == n;
  case 2:
    return count_straight(x, y, 0, 1) + count_straight(x, y, 0, -1) + 1 == n;
  case 3:
    return count_straight(x, y, 1, -1) + count_straight(x, y, -1, 1) + 1 == n;
  }
  return false;
}

int Board::count_straight(int x, int y, int dx, int dy) {
  int count = 0;
  while (1) {
    x += dx;
    y += dy;
    if (!in_range(x, y))
      break;
    if (board[x][y] != turn)
      break;
    count++;
  }
  return count;
}

bool Board::check_3_3(int x, int y) {
  int count = 0;
  if (is_nstones_made_open(x, y, 0, 3))
    count++;
  if (is_nstones_made_open(x, y, 1, 3))
    count++;
  if (is_nstones_made_open(x, y, 2, 3))
    count++;
  if (count == 0)
    return false;
  if (is_nstones_made_open(x, y, 3, 3))
    count++;
  return (count > 1) ? true : false;
}

bool Board::is_nstones_made_open(int x, int y, int npi_over_four, int n) {
  // npi_over_four in Z/4Z = {0, 1, 2, 3}
  // meaning the angle of n * pi/4 with the line
  int fst, snd;
  switch (npi_over_four) {
  case 0:
    fst = count_straight_open(x, y, 1, 0);
    if (fst == -1)
      return false;
    snd = count_straight(x, y, -1, 0);
    return (snd == -1) ? false : fst + 1 + snd == n;
  case 1:
    fst = count_straight_open(x, y, 1, 1);
    if (fst == -1)
      return false;
    snd = count_straight(x, y, -1, -1);
    return (snd == -1) ? false : fst + 1 + snd == n;
  case 2:
    fst = count_straight_open(x, y, 0, 1);
    if (fst == -1)
      return false;
    snd = count_straight(x, y, 0, -1);
    return (snd == -1) ? false : fst + 1 + snd == n;
  case 3:
    fst = count_straight_open(x, y, 1, -1);
    if (fst == -1)
      return false;
    snd = count_straight(x, y, -1, 1);
    return (snd == -1) ? false : fst + 1 + snd == n;
  }
  return false;
}

int Board::count_straight_open(int x, int y, int dx, int dy) {
  int count = 0;
  while (1) {
    x += dx;
    y += dy;
    if (!in_range(x, y))
      return -1;
    if (board[x][y] == EMPTY)
      return count;
    if (board[x][y] != turn)
      return -1;
    count++;
  }
}

void Board::bite_move(int x, int y) {
  bite_move_towards(x, y, -1, 0);
  bite_move_towards(x, y, 1, 0);
  bite_move_towards(x, y, 0, -1);
  bite_move_towards(x, y, 0, 1);
  bite_move_towards(x, y, 1, 1);
  bite_move_towards(x, y, -1, 1);
  bite_move_towards(x, y, 1, -1);
  bite_move_towards(x, y, -1, -1);
}

void Board::bite_move_towards(int x, int y, int dx, int dy) {
  if (!in_range(x + 3 * dx, y + 3 * dy))
    return;

  if (board[x + 3 * dx][y + 3 * dy] == turn &&
      board[x + 2 * dx][y + 2 * dy] == last &&
      board[x + 1 * dx][y + 1 * dy] == last) {

    board[x + 2 * dx][y + 2 * dy] = EMPTY;
    board[x + 1 * dx][y + 1 * dy] = EMPTY;

    if (turn == BLACK)
      scoreB += 2;
    else
      scoreW += 2;
  }
}

void Board::read_board(string file) {
  ifstream fin(file);
  if (!fin.good())
    return;

  vector<string> data;
  vector<string> grid;
  string token;

  while (getline(fin, token, ':')) {
    data.emplace_back(token);
  }
  fin.close();

  last_x = stoi(data[0]);
  last_y = stoi(data[1]);
  moves = stoi(data[2]);
  turn = (stoi(data[3]) == 1) ? BLACK : WHITE;
  last = (turn == BLACK) ? WHITE : BLACK;
  scoreB = stoi(data[4]);
  scoreW = stoi(data[5]);
  eB = stoi(data[6]) / 10.;
  eW = stoi(data[7]) / 10.;
  grid = vector<string>(data.begin() + 8, data.end());

  for (int i = 0; i < N * N; i++) {
    int x = i % N;
    int y = i / N;
    Stone stone = (stoi(grid[i]) == 1)    ? BLACK
                  : (stoi(grid[i]) == -1) ? WHITE
                                          : EMPTY;
    board[y][x] = stone;
  }
}

void Board::write_board(string file) {
  ofstream fout(file);
  int iturn = (turn == BLACK) ? -1 : 1;
  fout << last_x << ":";
  fout << last_y << ":";
  fout << moves << ":";
  fout << iturn << ":";
  fout << scoreB << ":";
  fout << scoreW << ":";
  fout << int(eB * 10) << ":";
  fout << int(1000 - (eB * 10)) << ":";

  for (int j = 0; j < N; j++) {
    for (int i = 0; i < N; i++) {
      int stone = (board[j][i] == BLACK) ? 1 : (board[j][i] == WHITE) ? -1 : 0;
      if ((i == N - 1) && (j == N - 1))
        fout << stone;
      else
        fout << stone << ":";
    }
  }
  fout.close();
}

void Board::print_board(bool dump_candy) {
  if (dump_candy) {
    for (auto q : candy) {
      int i = get<0>(q);
      int j = get<1>(q);
      board[i][j] = CANDY;
    }
  }

  cout << endl;
  cout << setw(4) << " ";

  for (int i = 0; i < N; i++) {
    cout << i % 10 << " ";
  }
  cout << endl;
  for (int i = 0; i < N; i++) {

    cout << setw(3) << i % 10 << " ";

    for (int j = 0; j < N; j++) {
      if (board[i][j] == CANDY) {

        cout << TC_GREEN << "+" << TC_RESET << " ";

      } else if (is_last_move(i, j)) {

        if (board[i][j] == BLACK)
          cout << TC_GREEN << "x" << TC_RESET << " ";
        if (board[i][j] == WHITE)
          cout << TC_GREEN << "o" << TC_RESET << " ";
      } else {

        if (board[i][j] == BLACK) {
          cout << TC_BLUE << "x" << TC_RESET << " ";
        } else if (board[i][j] == WHITE) {
          cout << TC_RED << "o" << TC_RESET << " ";
        } else {
          cout << "\u2219"
               << " ";
        }
      }
    }
    cout << endl;
  }
  cout << endl;
}
