#include <stdio.h>
#include <unistd.h>

// Constants for screen size
static const int BORDERS_OFFSET = 1;
static const int PLAYER_OFFSET_FROM_WALL = 2;
static const int SCREEN_WIDTH = 80;
static const int SCREEN_HEIGHT = 23;
static const int PLAYER_WIDTH = 1;
static const int PLAYER_HEIGHT = 3;
static const int CURSOR_OFFSET_Y = 1;
static const int MAX_SCORE = 21;

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
int g_player_index;

// Score
int g_game_score_pl_1;
int g_game_score_pl_2;

// State

int g_is_game_paused;
int g_turns_taken;

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
    g_ball_dir_y = 1;

    g_ball_target_player = 1;
    // current active player index
    g_player_index = 0;

    // State
    g_is_game_paused = 1;
    g_turns_taken = 0;

    if (reset_score == 1) {
        // Score
        g_game_score_pl_1 = 0;
        g_game_score_pl_2 = 0;
    }
}

void move_cursor(int x, int y) { printf("\033[%d;%dH", y, x); }

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

    draw_rectangle(g_ball_x, g_ball_y, g_ball_x + 1, g_ball_y + 1, '@');

    return 0;
}

void draw_text(int x, int y, char text) {
    move_cursor(x, y);
    printf("%c", text);
}

void draw_ui() {
    // The top and bottom
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        draw_rectangle(0, y, 1, y + 1, '|');
        draw_rectangle(SCREEN_WIDTH - 1, y, SCREEN_WIDTH, y + 1, '|');
    }
    // The left and right
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
    if (g_player_index == 0) {
        printf("[*] P1: %d", g_game_score_pl_1);  // Active Player 1
    } else {
        printf("[ ] P1: %d", g_game_score_pl_1);  // Inactive Player 1
    }

    move_cursor(SCREEN_WIDTH - 8 - BORDERS_OFFSET - text_offset, 2);
    if (g_player_index == 1) {
        printf("[*] P2: %d", g_game_score_pl_2);
    } else {
        printf("[ ] P2: %d", g_game_score_pl_2);
    }

    move_cursor(0, SCREEN_HEIGHT + BORDERS_OFFSET);
    printf("...:::player 1: a/z (up/down) ; player 2: k/m (up/down) ; SPACEBAR to Skip turn:::...");
}

void game_pause() {
    g_is_game_paused = 1;
    g_turns_taken = 0;
}

// --- INPUT ---

int capture_input() {
    int done = 0;
    char prevKey = 0;

    while (!done) {
        move_cursor(BORDERS_OFFSET, SCREEN_HEIGHT + BORDERS_OFFSET + CURSOR_OFFSET_Y);
        printf("\033[J");
        printf("---> ");
        int ch = getchar();

        if (g_player_index == 0) {
            switch (ch) {
                case 'a':  // move up
                    if (g_player1_y > 0) {
                        g_player1_y -= 1;
                        prevKey = 'a';
                    }
                    break;
                case 'z':  // move down
                    if (g_player1_y + 1 < SCREEN_HEIGHT - PLAYER_HEIGHT) {
                        g_player1_y += 1;
                        prevKey = 'z';
                    }
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
                        prevKey = 'k';
                    }
                    break;
                case 'm':  // move down
                    if (g_player2_y + 1 < SCREEN_HEIGHT - PLAYER_HEIGHT) {
                        g_player2_y += 1;
                        prevKey = 'm';
                    }
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

    return 0;
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
            game_pause();
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
            game_pause();
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
    if (g_ball_x - 1 <= BORDERS_OFFSET) {
        g_game_score_pl_2 += 1;
        g_player_index = 0;
    }

    if (g_ball_x + 1 >= SCREEN_WIDTH - BORDERS_OFFSET) {
        g_game_score_pl_1 += 1;
        g_player_index = 1;
    }
}

// --- UPDATE ---

void update() {
    if (g_is_game_paused == 0) {
        g_ball_x += g_ball_dir_x;
        g_ball_y += g_ball_dir_y;
    }
}

void switch_active_player() { g_player_index = (g_player_index == 0) ? 1 : 0; }

void draw_end_game_screen();
int prompt_play_again();
void show_final_message();

// MAIN GAME

int main(void) {
    // Main game loop
    // [TODO]: remove later or make conditional
    int FPS = (1000 / 25) * 1000;
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

            if (g_is_game_paused == 1) {
                capture_input();
                switch_active_player();
                g_turns_taken += 1;
            }
            // unpause the game since the players taken their turns
            if (g_turns_taken == 2) {
                g_is_game_paused = 0;
            }
            handle_collision();
            update();

            // Reset game state if scores have updated
            if (prev_game_score_pl_1 < g_game_score_pl_1 || prev_game_score_pl_2 < g_game_score_pl_2) {
                init_game_state(0);
                prev_game_score_pl_1 = g_game_score_pl_1;
                prev_game_score_pl_2 = g_game_score_pl_2;
            }
            // [TODO]: remove later or make conditional
            usleep(FPS);
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
