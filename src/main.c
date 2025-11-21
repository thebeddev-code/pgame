#include <math.h>
#include <stdio.h>
#include <unistd.h>

// Constants for screen size
static const int WIDTH = 80;
static const int HEIGHT = 25;
static const int PLAYER_WIDTH = 1;
static const int PLAYER_HEIGHT = 3;
// Player 1 paddle position and size
int g_player1_x = 0;
int g_player1_y = 0;

// Player 2 paddle position and size
int g_player2_x = WIDTH - 1;
int g_player2_y = 0;

// Ball position
int g_ball_x = WIDTH / 2;
int g_ball_y = HEIGHT / 2;

// Ball previous position (for clearing)
int g_ball_prev_x = WIDTH / 2;
int g_ball_prev_y = HEIGHT / 2;

// Ball direction
int g_ball_dir_x = 1;
int g_ball_dir_y = 0;

int g_player_index = 0;

void move_cursor(int x, int y) { printf("\033[%d;%dH", y, x); }

void draw_rectangle(int x_start, int y_start, int x1, int y1, char draw_char) {
    for (int y = y_start; y < y1; ++y) {
        for (int x = x_start; x < x1; ++x) {
            move_cursor(x + 1, y + 1);
            putchar(draw_char);
        }
    }
}

int draw() {
    // Draw player 1 paddle
    draw_rectangle(g_player1_x, g_player1_y, g_player1_x + PLAYER_WIDTH, g_player1_y + PLAYER_HEIGHT, '|');
    // Draw player 2 paddle
    draw_rectangle(g_player2_x, g_player2_y, g_player2_x + PLAYER_WIDTH, g_player2_y + PLAYER_HEIGHT, '|');

    draw_rectangle(g_ball_x, g_ball_y, g_ball_x + 1, g_ball_y + 1, '+');

    fflush(stdout);
    return 0;
}

void update() {
    g_ball_x += g_ball_dir_x;
    g_ball_y += g_ball_dir_y;
}

int input() {
    int ch = getchar();
    switch (ch) {
        case 'a':
            if (g_player_index == 0 && g_player1_y + 1 < HEIGHT) g_player1_y += 1;
            return 0;
        case 'z':
            if (g_player_index == 0 && g_player1_y - 1 >= 0) g_player1_y -= 1;
            return 0;
        case 'k':
            if (g_player_index == 1 && g_player2_y + 1 < HEIGHT) g_player2_y += 1;
            return 0;
        case 'm':
            if (g_player_index == 1 && g_player2_y - 1 >= 0) g_player2_y -= 1;
            return 0;
        case ' ':
            return 1;  // skip turn
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
            if (input(playerIndex)) {
            };

            update();
            // [TODO]: remove later or make conditional
            usleep(FPS);
        }
    }
}

int main(void) {
    g_player1_y = (int)floor(HEIGHT / 2);
    g_player2_y = (int)floor(HEIGHT / 2);
    // Main game loop
    while (1) {
        game_loop();
        break;
    }
    return 0;
}
