#include <ncurses.h>
#include <stdio.h>
// Constants for screen size
static const int BORDERS_OFFSET = 1;
static const int GOAL_OFFSET_FROM_BORDER = 5;
static const int PLAYER_OFFSET_FROM_WALL = 2;
static const int SCREEN_WIDTH = 80;
static const int SCREEN_HEIGHT = 23;
static const int PLAYER_WIDTH = 1;
static const int PLAYER_HEIGHT = 3;
static const int CURSOR_OFFSET_Y = 1;
static const int MAX_SCORE = 21;
static const int GAME_WINDOW_LEFT_OFFSET = 15;
// [---] Declaring our game state

// Player
int g_player1_x;
int g_player2_x;

int g_player1_y;
int g_player2_y;

// Ball
int g_ball_x;
int g_ball_y;

int g_ball_prev_x;
int g_ball_prev_y;

int g_ball_dir_x;
int g_ball_dir_y;

int g_ball_target_player;

// Active player index

// Score
int g_game_score_pl_1;
int g_game_score_pl_2;

// State

// Input state
int key_a = 0, key_z = 0, key_k = 0, key_m = 0;

// Initializing our game state
void init_game_state(int reset_score) {
    // Player
    g_player1_x = 1 + PLAYER_OFFSET_FROM_WALL;
    g_player2_x = SCREEN_WIDTH - 2 - PLAYER_OFFSET_FROM_WALL;

    g_player1_y = (SCREEN_HEIGHT / 2) - (PLAYER_HEIGHT / 2);
    g_player2_y = (SCREEN_HEIGHT / 2) - (PLAYER_HEIGHT / 2);

    // Ball
    g_ball_x = SCREEN_WIDTH / 2;
    g_ball_y = SCREEN_HEIGHT / 2;
    // Ball prev pos
    g_ball_prev_x = g_ball_x;
    g_ball_prev_y = g_ball_y;
    // Ball dir
    g_ball_dir_x = -1;
    g_ball_dir_y = 0;

    g_ball_target_player = 1;

    if (reset_score == 1) {
        // Score
        g_game_score_pl_1 = 0;
        g_game_score_pl_2 = 0;
    }
}

void move_cursor(int x, int y) { printf("\033[%d;%dH", y, x + GAME_WINDOW_LEFT_OFFSET); }

// --- DRAW ---

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

    draw_rectangle(g_ball_x, g_ball_y, g_ball_x + 1, g_ball_y + 1, '.');

    return 0;
}

void draw_text(int x, int y, char text) {
    move_cursor(x, y);
    printf("%c", text);
}

void draw_ui() {
    // The left and right
    // for (int y = 0; y < SCREEN_HEIGHT; ++y) {
    //     draw_rectangle(0, y, 1, y + 1, '|');
    //     draw_rectangle(SCREEN_WIDTH - 1, y, SCREEN_WIDTH, y + 1, '|');
    // }
    // The top and bottom
    for (int x = 0; x < SCREEN_WIDTH; ++x) {
        char border_draw_char = (x == 0 || x == SCREEN_WIDTH - 1) ? '+' : '-';
        draw_rectangle(x, 0, x + 1, 1, border_draw_char);
        draw_rectangle(x, SCREEN_HEIGHT - 1, x + 1, SCREEN_HEIGHT, border_draw_char);
    }

    // The mid line
    for (int y = 0; y < SCREEN_HEIGHT - BORDERS_OFFSET; ++y) {
        draw_rectangle(SCREEN_WIDTH / 2, y, (SCREEN_WIDTH / 2) + 1, y + 1, '|');
    }

    int text_offset = 1;
    move_cursor((SCREEN_WIDTH / 2) - 8, 1);
    printf("%s", "...:::P O N G:::..");
    // We can't use arrays and thus sprintf
    move_cursor(2 + text_offset, 2);
    if (g_ball_dir_x == -1) {
        printf("[*] P1: %d", g_game_score_pl_1);  // Active Player 1
    } else {
        printf("[ ] P1: %d", g_game_score_pl_1);  // Inactive Player 1
    }

    move_cursor(SCREEN_WIDTH - 8 - BORDERS_OFFSET - text_offset, 2);
    if (g_ball_dir_x == 1) {
        printf("[*] P2: %d", g_game_score_pl_2);
    } else {
        printf("[ ] P2: %d", g_game_score_pl_2);
    }

    move_cursor(0, SCREEN_HEIGHT + BORDERS_OFFSET);
    printf("...:::player 1: a/z or w/s (up/down) ; player 2: k/m or arrow up/down (up/down):::...");
}

// --- INPUT ---
void reset_keys() { key_a = 0, key_z = 0, key_k = 0, key_m = 0; }

void handle_input() {
    reset_keys();

    int ch;
    while ((ch = getch()) != ERR) {
        switch (ch) {
            case 'a':
                key_a = 1;
                break;
            case 'z':
                key_z = 1;
                break;
            case 'k':
                key_k = 1;
                break;
            case 'm':
                key_m = 1;
                break;
            case KEY_UP:
                key_k = 1;
                break;
            case KEY_DOWN:
                key_m = 1;
                break;
            case 'w':
                key_a = 1;
                break;
            case 's':
                key_z = 1;
                break;
        }
    }

    // Update positions based on key states
    if (key_a && g_player1_y > 0) g_player1_y--;
    if (key_z && g_player1_y < SCREEN_HEIGHT - PLAYER_HEIGHT) g_player1_y++;
    if (key_k && g_player2_y > 0) g_player2_y--;
    if (key_m && g_player2_y < SCREEN_HEIGHT - PLAYER_HEIGHT) g_player2_y++;
}

// --- COLLISION ---

void handle_ball_motion(int player_index) {
    if (player_index == 0) {
        g_ball_dir_x = 1;
        g_ball_dir_y = g_ball_y == g_player1_y ? -1 : g_ball_y == g_player1_y + PLAYER_HEIGHT - 1 ? 1 : 0;
        // Setting the prev position to avoid collision check
        g_ball_prev_x = g_ball_x;
        g_ball_prev_y = g_ball_y;
        // Only stop the game if the ball receiving player got the ball
        if (g_ball_target_player == 0) {
            g_ball_target_player = 1;
        };
    }

    if (player_index == 1) {
        g_ball_dir_x = -1;  // Move left
        g_ball_dir_y = g_ball_y == g_player2_y ? -1 : g_ball_y == g_player2_y + PLAYER_HEIGHT - 1 ? 1 : 0;
        // Setting the prev position to avoid collision check
        g_ball_prev_x = g_ball_x;
        g_ball_prev_y = g_ball_y;
        // Only stop the game if the ball receiving player got the ball
        if (g_ball_target_player == 1) {
            g_ball_target_player = 0;
        };
    }
}

void handle_collision() {
    // Checking collision with the player
    if (g_ball_x != g_ball_prev_x || g_ball_y != g_ball_prev_y) {
        // PLAYER 1
        int is_colliding_x_pl_1 = g_ball_x - 1 == g_player1_x;
        // Checking for collision
        if (is_colliding_x_pl_1 && g_ball_y >= g_player1_y && g_ball_y < g_player1_y + PLAYER_HEIGHT) {
            handle_ball_motion(0);
        }
        // EDGE CASES 45 DEG
        // only check for diagonal motion
        if (g_ball_dir_y > 0) {
            // PLAYER 1 EDGE CASES
            if (is_colliding_x_pl_1 && g_ball_y + 1 == g_player1_y) {
                //  Repositioning the ball to be on the paddle
                g_ball_y = g_player1_y;
                handle_ball_motion(0);
            }

            if (is_colliding_x_pl_1 && g_ball_y - 1 == g_player1_y + PLAYER_HEIGHT - 1) {
                //  Repositioning the ball to be on the paddle
                g_ball_y = g_player1_y + PLAYER_HEIGHT - 1;
                handle_ball_motion(0);
            }
        }

        // PLAYER 2
        int is_colliding_x_pl_2 = g_ball_x + 1 == g_player2_x;
        // Checking for collision
        if (is_colliding_x_pl_2 && g_ball_y >= g_player2_y && g_ball_y < g_player2_y + PLAYER_HEIGHT) {
            handle_ball_motion(1);
            // only check for diagonal motion
        }
        // EDGE CASES 45 DEG
        if (g_ball_dir_y > 0) {
            if (is_colliding_x_pl_2 && g_ball_y + 1 == g_player2_y) {
                g_ball_y = g_player2_y;
                handle_ball_motion(1);
            }

            if (is_colliding_x_pl_2 && g_ball_y - 1 == g_player2_y + PLAYER_HEIGHT - 1) {
                g_ball_y = g_player2_y + PLAYER_HEIGHT - 1;
                handle_ball_motion(1);
            }
        }
    }
    // Checking collision with the top/bottom borders
    // If so reflect ball's y dir
    if (g_ball_y - 1 <= BORDERS_OFFSET) {
        g_ball_dir_y = 1;
    }
    if (g_ball_y + 1 >= SCREEN_HEIGHT - BORDERS_OFFSET) {
        g_ball_dir_y = -1;
    }
    // Checking if hits the left/right borders
    // If so increase, the ball sender's score
    if (g_ball_x - 1 <= BORDERS_OFFSET - GOAL_OFFSET_FROM_BORDER) {
        g_game_score_pl_2 += 1;
    }

    if (g_ball_x + 1 >= SCREEN_WIDTH - BORDERS_OFFSET + GOAL_OFFSET_FROM_BORDER) {
        g_game_score_pl_1 += 1;
    }
}

// --- UPDATE ---

void update() {
    g_ball_x += g_ball_dir_x;
    g_ball_y += g_ball_dir_y;
}

void draw_end_game_screen();
int prompt_play_again();
void show_final_message();

// MAIN GAME

int main(void) {
    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE);
    keypad(stdscr, TRUE);
    curs_set(0);
    // Main game loop
    // [TODO]: remove later or make conditional
    int FPS = (1000 / 30) * 1000;
    while (1) {
        init_game_state(1);
        int prev_game_score_pl_1 = g_game_score_pl_1;
        int prev_game_score_pl_2 = g_game_score_pl_2;

        while (1) {
            printf("\e[1;1H\e[2J");
            draw_ui();
            draw();
            // flushing output, so that instead of being stored in the buffer it would be ouputted to the
            // screen
            fflush(stdout);
            // By default it seems that getchar causes stdout to fflush

            //  Exiting loop once score reaches 21 points
            if (g_game_score_pl_1 >= MAX_SCORE || g_game_score_pl_2 >= MAX_SCORE) {
                break;
            }

            handle_input();
            handle_collision();
            update();

            // Reset game state if scores have updated
            if (prev_game_score_pl_1 < g_game_score_pl_1 || prev_game_score_pl_2 < g_game_score_pl_2) {
                init_game_state(0);
                prev_game_score_pl_1 = g_game_score_pl_1;
                prev_game_score_pl_2 = g_game_score_pl_2;
            }
            // [TODO]: remove later or make conditional
            napms(25);
        }
        draw_end_game_screen();
        if (prompt_play_again() == 0) {
            break;
        };
    }
    show_final_message();
    return 0;
}

// END GAME

void draw_end_game_screen() {
    // Clear screen using ANSI escape code
    printf("\033[2J");

    // Centered title
    move_cursor(SCREEN_WIDTH / 2 - 11, SCREEN_HEIGHT / 2 - 4);
    printf("=== GAME OVER ===");

    // Display scores
    move_cursor(SCREEN_WIDTH / 2 - 15, SCREEN_HEIGHT / 2 - 2);
    printf("Player 1 Score: %d", g_game_score_pl_1);

    move_cursor(SCREEN_WIDTH / 2 - 15, SCREEN_HEIGHT / 2 - 1);
    printf("Player 2 Score: %d", g_game_score_pl_2);

    // Determine and display winner
    move_cursor(SCREEN_WIDTH / 2 - 15, SCREEN_HEIGHT / 2 + 1);
    if (g_game_score_pl_1 > g_game_score_pl_2) {
        printf("Player 1 Wins!");
    } else if (g_game_score_pl_2 > g_game_score_pl_1) {
        printf("Player 2 Wins!");
    } else {
        printf("It's a Tie!");
    }

    // Prompt to exit
    move_cursor(SCREEN_WIDTH / 2 - 15, SCREEN_HEIGHT / 2 + 3);
    printf("Press Enter to exit...");

    // Flush output to ensure immediate display
    fflush(stdout);
}

int prompt_play_again() {
    char choice;
    while (1) {
        printf("Do you want to play again? (Y/N): ");
        scanf(" %c", &choice);  // Space before %c skips whitespace/newline

        if (choice == 'Y' || choice == 'y') {
            return 1;  // Play again
        } else if (choice == 'N' || choice == 'n') {
            return 0;  // Exit
        } else {
            printf("Invalid input. Please enter Y or N.\n");
        }
    }
}

void show_final_message() {
    // Clear screen
    printf("\033[2J");

    // Center the message
    move_cursor(SCREEN_WIDTH / 2 - 7, SCREEN_HEIGHT / 2);
    printf("PONGI-PONG!");

    // Add a blank line below
    move_cursor(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 + 1);
    printf("--------------------");

    fflush(stdout);
}
