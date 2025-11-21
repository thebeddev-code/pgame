#include <math.h>
#include <stdio.h>
#include <unistd.h>

// Constants for screen size
static const int g_WIDTH = 80;
static const int g_HEIGHT = 25;
static const int g_PLAYER_WIDTH = 1;
static const int g_PLAYER_HEIGHT = 3;
// Player 1 paddle position and size
int player1_x = 0;
int player1_y = 0;

// Player 2 paddle position and size
int player2_x = g_WIDTH - 1;
int player2_y = 0;

// Ball position
int ball_x = g_WIDTH / 2;
int ball_y = g_HEIGHT / 2;

// Ball previous position (for clearing)
int ball_prev_x = g_WIDTH / 2;
int ball_prev_y = g_HEIGHT / 2;

// Ball direction
int ball_dir_x = 1;
int ball_dir_y = 0;

void make_player_drawable() {}

void move_cursor(int x, int y) { printf("\033[%d;%dH", y, x); }

void draw_rectangle(int xStart, int yStart, int x1, int y1) {
  for (int y = yStart; y < y1; ++y) {
    for (int x = xStart; x < x1; ++x) {
      move_cursor(x + 1, y + 1);
      putchar('#');
    }
  }
}

int draw() {
  // Draw player 1 paddle
  draw_rectangle(player1_x, player1_y, player1_x + g_PLAYER_WIDTH,
                 player1_y + g_PLAYER_HEIGHT);
  // Draw player 2 paddle
  draw_rectangle(player2_x, player2_y, player2_x + g_PLAYER_WIDTH,
                 player2_y + g_PLAYER_HEIGHT);

  draw_rectangle(ball_x, ball_y, ball_x + 1, ball_y + 1);

  fflush(stdout);
  return 0;
}

void update() {
  ball_x += ball_dir_x;
  ball_y += ball_dir_y;
}

int input(int playerIndex) {
  int ch = getchar();
  switch (ch) {
  case 'a':
    if (playerIndex == 0 && player1_y + 1 < g_HEIGHT)
      player1_y += 1;
    return 0;
  case 'z':
    if (playerIndex == 0 && player1_y - 1 >= 0)
      player1_y -= 1;
    return 0;
  case 'k':
    if (playerIndex == 1 && player2_y + 1 < g_HEIGHT)
      player2_y += 1;
    return 0;
  case 'm':
    if (playerIndex == 1 && player2_y - 1 >= 0)
      player2_y -= 1;
    return 0;
  case ' ':
    return 1; // skip turn
  default:
    return 0;
  }
}

int game_loop() {
  // [TODO]: remove later or make conditional
  int FPS = (1000 / 60) * 1000;
  while (1) {
    int playerIndex = 0;
    while (1) {
      printf("\e[1;1H\e[2J");
      draw();
      // if (input(playerIndex)) {
      // };

      update();
      // [TODO]: remove later or make conditional
      usleep(FPS);
    }
  }
}

int main(void) {
  player1_y = (int)floor(g_HEIGHT / 2);
  player2_y = (int)floor(g_HEIGHT / 2);
  // Main game loop
  while (1) {
    game_loop();
    break;
  }
  return 0;
}