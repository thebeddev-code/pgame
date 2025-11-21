#include <math.h>
#include <stdio.h>
#include <unistd.h>

// Constants for screen size
static const int BORDERS_OFFSET = 3;
static const int SCREEN_WIDTH = 80 - 2;
static const int SCREEN_HEIGHT = 25 - 2;
static const int PLAYER_WIDTH = 1;
static const int PLAYER_HEIGHT = 3;
// Player 1 paddle position and size
int g_player1_x = BORDERS_OFFSET - 1;
int g_player1_y = 0;

// Player 2 paddle position and size
int g_player2_x = SCREEN_WIDTH - BORDERS_OFFSET;
int g_player2_y = 0;

// Ball position
int g_ball_x = SCREEN_WIDTH / 2;
int g_ball_y = SCREEN_HEIGHT / 2;

// Ball previous position (for clearing)
int g_ball_prev_x = SCREEN_WIDTH / 2;
int g_ball_prev_y = SCREEN_HEIGHT / 2;

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

    // draw_rectangle(g_ball_x, g_ball_y, g_ball_x + 1, g_ball_y + 1, '+');

    fflush(stdout);
    return 0;
}

void draw_ui(char border_draw_char) {
    for (int y = 0; y < SCREEN_HEIGHT + BORDERS_OFFSET; ++y) {
        for (int x = 0; x < SCREEN_WIDTH + BORDERS_OFFSET; ++x) {
            // Draw the top edge
            if (y == 0 && (x < SCREEN_WIDTH + BORDERS_OFFSET)) {
                draw_rectangle(x, y, x + 1, y + 1, border_draw_char);
            }
            // Draw the left edge
            if (x == 0 && (y < SCREEN_HEIGHT + BORDERS_OFFSET)) {
                draw_rectangle(x, y, x + 1, y + 1, border_draw_char);
            }
            // Draw the bottom edge
            if (y == SCREEN_HEIGHT + BORDERS_OFFSET - 1 && (x < SCREEN_WIDTH + BORDERS_OFFSET)) {
                draw_rectangle(x, y, x + 1, y + 1, border_draw_char);
            }
            // Draw the right edge
            if (x == SCREEN_WIDTH + BORDERS_OFFSET - 1 && (y < SCREEN_HEIGHT + BORDERS_OFFSET)) {
                draw_rectangle(x, y, x + 1, y + 1, border_draw_char);
            }
        }
    }
}

void update() {
    g_ball_x += g_ball_dir_x;
    g_ball_y += g_ball_dir_y;
}

int capture_input() {
    int done = 0;
    char prevKey = 0;

    while (!done) {
        move_cursor(SCREEN_WIDTH + BORDERS_OFFSET + 1, SCREEN_HEIGHT + BORDERS_OFFSET);
        printf("\033[J");
        int ch = getchar();

        if (g_player_index == 0) {
            switch (ch) {
                case 'a':  // move up
                    if (g_player1_y > 0) {
                        g_player1_y -= 1;
                    }
                    prevKey = 'a';
                    break;
                case 'z':  // move down
                    if (g_player1_y + 1 < SCREEN_HEIGHT) {
                        g_player1_y += 1;
                    }
                    prevKey = 'z';
                    break;
                case '\n':
                    // End turn ONLY if last key was a move key (from a specific player)
                    // Since in case of player whose turn is next was pressing 'enter'
                    // That would trigger move, even tho the active player has done no move
                    if (prevKey == 'a' || prevKey == 'z') {
                        done = 1;
                    }
                    break;
                case ' ':
                    done = 1;
                    break;
                default:
                    break;
            }
        }

        if (g_player_index == 1) {
            switch (ch) {
                case 'k':  // move up
                    if (g_player2_y > 0) {
                        g_player2_y -= 1;
                    }
                    prevKey = 'k';
                    break;
                case 'm':  // move down
                    if (g_player2_y + 1 < SCREEN_HEIGHT) {
                        g_player2_y += 1;
                    }
                    prevKey = 'm';
                    break;
                case '\n':
                    if (prevKey == 'k' || prevKey == 'm') {
                        done = 1;
                    }
                    break;
                case ' ':
                    done = 1;
                    break;
                default:
                    break;
            }
        }
    }

    g_player_index = (g_player_index == 0) ? 1 : 0;
    return 0;
}

int main(void) {
    g_player1_y = (int)floor(SCREEN_HEIGHT / 2);
    g_player2_y = (int)floor(SCREEN_HEIGHT / 2);
    // Main game loop
    // [TODO]: remove later or make conditional
    int FPS = (1000 / 60) * 1000;
    while (1) {
        while (1) {
            printf("\e[1;1H\e[2J");
            draw();
            draw_ui('*');
            if (capture_input()) {
            };

            update();
            // [TODO]: remove later or make conditional
            usleep(FPS);
        }
        break;
    }
    return 0;
}
