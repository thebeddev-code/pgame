#include <math.h>
#include <stdio.h>

// This removes any char logging to terminal
// void disable_echo() {
//   struct termios t;
//   tcgetattr(STDIN_FILENO, &t);
//   t.c_lflag &= ~ECHO;
//   tcsetattr(STDIN_FILENO, TCSAFLUSH, &t);
// }

struct Vector2 {
  int x;
  int y;
};

struct Drawable {
  int x;
  int y;
  int x1;
  int y1;
};

struct Player {
  struct Vector2 position;
  int width;
  int height;
};

struct Drawable make_player_drawable(struct Player player) {
  struct Drawable d;
  d.x = player.position.x;
  d.x1 = player.position.x + player.width -
         1; // subtract 1 because coordinates are inclusive
  d.y = player.position.y;
  d.y1 = player.position.y + player.height - 1; // subtract 1 for same reason
  return d;
}

static const int g_WIDTH = 80;
static const int g_HEIGHT = 25;
static const int g_PLAYER_HEIGHT = 3;
static const int g_PLAYER_WIDTH = 3;

void move_cursor(int x, int y) { printf("\033[%d;%dH", y, x); }

int draw(struct Drawable drawables[], int count) {
  for (int i = 0; i < count; i++) {
    struct Drawable d = drawables[i];
    for (int y = d.y; y <= d.y1; y++) {
      for (int x = d.x; x <= d.x1; x++) {
        move_cursor(x + 1, y + 1);
        putchar('#');
      }
    }
  }
  fflush(stdout);
  return 0;
}

void update(struct Drawable drawables[], struct Player players[], int count) {
  for (int i = 0; i < count; i++) {
    drawables[i].x = players[i].position.x;
    drawables[i].x1 = players[i].width + players[i].position.x - 1;
    drawables[i].y = players[i].position.y;
    drawables[i].y1 = players[i].height + players[i].position.y - 1;
  }
}

int input(struct Player *player) {
  char c = getchar();
  switch (c) {
  case 'a':
    player->position.y += 1;
    return 0;
  case 'z':
    player->position.y -= 1;
    return 0;
  case ' ': {
    return 1;
  }
  }

  return 1;
}

int main(void) {
  struct Player players[] = {
      {.position = {.x = 0, .y = (int)floor(g_HEIGHT / 2)},
       .width = 1,
       .height = 3},
      {.position = {.x = g_WIDTH, .y = (int)floor(g_HEIGHT / 2)},
       .width = 1,
       .height = 3}};

  struct Drawable drawables[] = {
      make_player_drawable(players[0]),
      make_player_drawable(players[1]),
      {.x = 0, .x1 = 0, .y = 0, .y1 = 0},
      {.x = g_WIDTH, .x1 = g_WIDTH, .y = 0, .y1 = 0}};

  int playerIndex = 0;
  //   disable_echo();
  int drawablesLength = sizeof(drawables) / sizeof(drawables[0]);
  while (1) {
    printf("\e[1;1H\e[2J");

    if (playerIndex == 0) {
      struct Drawable indicator1 = drawables[2];
      indicator1.x1 = 0;
      struct Drawable indicator2 = drawables[3];
      indicator2.x1 = 0;
    } else {
      struct Drawable indicator1 = drawables[2];
      indicator1.x1 = -1;
      struct Drawable indicator2 = drawables[3];
      indicator2.x1 = g_WIDTH;
    }

    draw(drawables, drawablesLength);
    if (input((&players[playerIndex])) == 1) {
      playerIndex = playerIndex == 0 ? 1 : 0;
    };

    update(drawables, players, 2);
  }
  return 0;
}
