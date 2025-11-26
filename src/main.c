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
static const int PLAYER_1_BALL_COLLISION = 1;
static const int PLAYER_2_BALL_COLLISION = 2;
static const int BALL_COLLISION_WALLS_VERTICAL = 3;
static const int BALL_COLLISION_WALLS_HORIZONTAL = 4;
static const int PLAYER_1_45deg_BALL_COLLISION_TOP = 5;
static const int PLAYER_1_45deg_BALL_COLLISION_BOTTOM = 6;
static const int PLAYER_2_45deg_BALL_COLLISION_TOP = 7;
static const int PLAYER_2_45deg_BALL_COLLISION_BOTTOM = 8;

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

void draw(int player1_x, int player1_y, int player2_x, int player2_y, int ball_x, int ball_y) {
    // Draw player 1 paddle
    draw_rectangle(player1_x, player1_y, player1_x + PLAYER_WIDTH, player2_y + PLAYER_HEIGHT, '|');
    // Draw player 2 paddle
    draw_rectangle(player2_x, player2_y, player2_x + PLAYER_WIDTH, player2_y + PLAYER_HEIGHT, '|');

    draw_rectangle(ball_x, ball_y, ball_x + 1, ball_y + 1, '@');
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
    // if (g_player_index == 0) {
    //     printf("[*] P1: %d", g_game_score_pl_1);  // Active Player 1
    // } else {
    //     printf("[ ] P1: %d", g_game_score_pl_1);  // Inactive Player 1
    // }

    move_cursor(SCREEN_WIDTH - 8 - BORDERS_OFFSET - text_offset, 2);
    // if (g_player_index == 1) {
    //     printf("[*] P2: %d", g_game_score_pl_2);
    // } else {
    //     printf("[ ] P2: %d", g_game_score_pl_2);
    // }

    move_cursor(0, SCREEN_HEIGHT + BORDERS_OFFSET);
    printf("...:::player 1: a/z (up/down) ; player 2: k/m (up/down) ; SPACEBAR to Skip turn:::...");
}

// --- INPUT ---

void reset_player_cursor() {
    move_cursor(BORDERS_OFFSET, SCREEN_HEIGHT + BORDERS_OFFSET + CURSOR_OFFSET_Y);
    printf("\033[J");
    printf("---> ");
}

int read_player1_input_dy() {
    int done = 0;
    int temp_y = 0;
    char prev_key;

    while (!done) {
        reset_player_cursor();
        int ch = getchar();

        switch (ch) {
            case 'a':
                temp_y -= 1;
                break;
            case 'z':
                temp_y += 1;
                break;
            case '\n':
                if (temp_y != 0) done = 1;
            case ' ':
                done = 1;
                break;
        }
    }

    return temp_y;
}

int read_player2_input_dy() {
    int done = 0;
    int temp_y = 0;

    while (!done) {
        reset_player_cursor();
        int ch = getchar();

        switch (ch) {
            case 'k':
                temp_y -= 1;
                break;
            case 'm':
                temp_y += 1;
            case '\n':
                if (temp_y != 0) done = 1;
            case ' ':
                done = 1;
                break;
        }
    }

    return temp_y;
}

// --- COLLISION ---

// void handle_ball_motion(int player_index) {
//     if (player_index == 0) {
//         g_ball_dir_x = 1;
//         g_ball_dir_y = g_ball_y == g_player1_y ? -1 : g_ball_y == g_player1_y + PLAYER_HEIGHT - 1 ? 1 : 0;
//         // Setting the prev position to avoid collision check
//         g_ball_prev_x = g_ball_x;
//         g_ball_prev_y = g_ball_y;
//         // Only stop the game if the ball receiving player got the ball
//         if (g_ball_target_player == 0) {
//             g_ball_target_player = 1;
//             game_pause();
//         };
//     }

//     if (player_index == 1) {
//         g_ball_dir_x = -1;  // Move left
//         g_ball_dir_y = g_ball_y == g_player2_y ? -1 : g_ball_y == g_player2_y + PLAYER_HEIGHT - 1 ? 1 : 0;
//         // Setting the prev position to avoid collision check
//         g_ball_prev_x = g_ball_x;
//         g_ball_prev_y = g_ball_y;
//         // Only stop the game if the ball receiving player got the ball
//         if (g_ball_target_player == 1) {
//             g_ball_target_player = 0;
//             game_pause();
//         };
//     }
// }

int check_collision(int player1_x, int player1_y, int player2_x, int player2_y, int ball_x, int ball_y,
                    int ball_dx, int ball_dy, int ball_prev_x, int ball_prev_y) {
    int collision_type = 0;
    if (ball_x != ball_prev_x || ball_y != ball_prev_y) {
        // PLAYER 1
        int is_colliding_x_pl_1 = ball_x - 1 == player1_x;
        if (is_colliding_x_pl_1 && ball_y >= player1_y && ball_y < player1_y + PLAYER_HEIGHT) {
            collision_type = PLAYER_1_BALL_COLLISION;
        }
        // EDGE CASES 45 DEG
        if (ball_dy > 0) {
            if (is_colliding_x_pl_1 && ball_y + 1 == player1_y)
                collision_type = PLAYER_1_45deg_BALL_COLLISION_TOP;
            if (is_colliding_x_pl_1 && ball_y - 1 == player1_y + PLAYER_HEIGHT - 1)
                collision_type = PLAYER_1_45deg_BALL_COLLISION_BOTTOM;
        }
        // PLAYER 2
        int is_collidinx_pl_2 = ball_x + 1 == player2_x;
        if (is_collidinx_pl_2 && ball_y >= player2_y && ball_y < player2_y + PLAYER_HEIGHT) {
            collision_type = PLAYER_2_BALL_COLLISION;
        }
        // EDGE CASES 45 DEG
        if (ball_dy > 0) {
            if (is_collidinx_pl_2 && ball_y + 1 == player2_y)
                collision_type = PLAYER_2_45deg_BALL_COLLISION_TOP;
            if (is_collidinx_pl_2 && ball_y - 1 == player2_y + PLAYER_HEIGHT - 1)
                collision_type = PLAYER_2_45deg_BALL_COLLISION_BOTTOM;
        }
    }
    if (ball_y - 1 <= BORDERS_OFFSET || ball_y + 1 >= SCREEN_HEIGHT - BORDERS_OFFSET)
        collision_type = BALL_COLLISION_WALLS_VERTICAL;
    if (ball_x - 1 <= BORDERS_OFFSET || ball_x + 1 >= SCREEN_WIDTH - BORDERS_OFFSET)
        collision_type = BALL_COLLISION_WALLS_HORIZONTAL;
    return collision_type;
}

void draw_end_game_screen();
int prompt_play_again();
void show_final_message();

void gameLoop(int FPS, int player1_x, int player2_x, int player1_y, int player2_y, int player1_dy,
              int player2_dy, int ball_x, int ball_y, int ball_prev_x, int ball_prev_y, int ball_dx,
              int ball_dy, int turn_player_index, int is_game_paused, int turns_taken, int game_score_pl_1,
              int game_score_pl_2, int prev_game_score_pl_1, int prev_game_score_pl_2, int player_turn) {
    while (1) {
        printf("\e[1;1H\e[2J");  // Clear screen
        draw_ui();
        draw(player1_x, player1_y, player2_x, player2_y, ball_x, ball_y);
        fflush(stdout);

        if (is_game_paused) {
            if (player_turn == 0) {
                player1_dy = read_player1_input_dy();
                player_turn++;
            } else {
                player2_dy = read_player2_input_dy();
                is_game_paused = 0;
                player_turn = 0;
            }
        }

        int collision_type = check_collision(player1_x, player1_y, player2_x, player2_y, ball_x, ball_y,
                                             ball_dx, ball_dy, ball_prev_x, ball_prev_y);

        if (collision_type != 0) {
            if ((collision_type == PLAYER_1_BALL_COLLISION && turn_player_index == 1) ||
                (collision_type == PLAYER_2_BALL_COLLISION && turn_player_index == 0)) {
                ball_dx = -ball_dx;
            } else if (collision_type == BALL_COLLISION_WALLS_VERTICAL) {
                ball_dy = -ball_dy;
            } else if (collision_type == BALL_COLLISION_WALLS_HORIZONTAL) {
                if (turn_player_index == 1) game_score_pl_1++;
                if (turn_player_index == 0) game_score_pl_2++;  // Fixed increment
            } else {
                turn_player_index = !turn_player_index;
                is_game_paused = 1;
                ball_dx = ball_dy = 0;
            }

            // Handle angled bounces
            if (collision_type == PLAYER_1_45deg_BALL_COLLISION_TOP)
                ball_dy = -player1_y;
            else if (collision_type == PLAYER_1_45deg_BALL_COLLISION_BOTTOM)
                ball_dy = -player1_y - PLAYER_HEIGHT + 1;
            else if (collision_type == PLAYER_2_45deg_BALL_COLLISION_TOP)
                ball_dy = -player2_y;
            else if (collision_type == PLAYER_2_45deg_BALL_COLLISION_BOTTOM)
                ball_dy = -player2_y - PLAYER_HEIGHT + 1;
        }

        // Update positions
        player1_y += player1_dy;
        player2_y += player2_dy;
        player1_dy = player2_dy = 0;

        if (!is_game_paused) {
            ball_prev_x = ball_x;
            ball_prev_y = ball_y;
            ball_x += ball_dx;
            ball_y += ball_dy;
        }

        usleep(FPS);
    }
}
// MAIN GAME

int main(void) {
    // Main game loop
    // [TODO]: remove later or make conditional
    int FPS = (1000 / 25) * 1000;

    int player1_x = 1 + PLAYER_OFFSET_FROM_WALL;
    int player2_x = SCREEN_WIDTH - 2 - PLAYER_OFFSET_FROM_WALL;

    int player1_y = (SCREEN_HEIGHT / 2) - (PLAYER_HEIGHT / 2);
    int player2_y = (SCREEN_HEIGHT / 2) - (PLAYER_HEIGHT / 2);

    int player1_dy = 0;
    int player2_dy = 0;

    // Ball
    int ball_x = SCREEN_WIDTH / 2;
    int ball_y = SCREEN_HEIGHT / 2;
    // Ball prev pos
    int ball_prev_x = ball_x;
    int ball_prev_y = ball_y;
    // Ball dir
    int ball_dx = -1;
    int ball_dy = 0;

    int turn_player_index = 1;

    // State
    int is_game_paused = 0;
    int turns_taken = 0;

    int game_score_pl_1 = 0;
    int game_score_pl_2 = 0;

    int prev_game_score_pl_1 = 0;
    int prev_game_score_pl_2 = 0;

    int player_turn = 0;

    gameLoop(FPS, player1_x, player2_x, player1_y, player2_y, player1_dy, player2_dy, ball_x, ball_y,
             ball_prev_x, ball_prev_y, ball_dx, ball_dy, turn_player_index, is_game_paused, turns_taken,
             game_score_pl_1, game_score_pl_2, prev_game_score_pl_1, prev_game_score_pl_2, player_turn);

    // draw_end_game_screen();
    // if (prompt_play_again() == 0) {
    //     break;
    // };

    // show_final_message();
    return 0;
}

// END GAME

// void draw_end_game_screen() {
//     // Clear screen using ANSI escape code
//     printf("\033[2J");

//     // Centered title
//     move_cursor(SCREEN_WIDTH / 2 - 11, SCREEN_HEIGHT / 2 - 4);
//     printf("=== GAME OVER ===");

//     // Display scores
//     move_cursor(SCREEN_WIDTH / 2 - 15, SCREEN_HEIGHT / 2 - 2);
//     printf("Player 1 Score: %d", game_score_pl_1);

//     move_cursor(SCREEN_WIDTH / 2 - 15, SCREEN_HEIGHT / 2 - 1);
//     printf("Player 2 Score: %d", game_score_pl_2);

//     // Determine and display winner
//     move_cursor(SCREEN_WIDTH / 2 - 15, SCREEN_HEIGHT / 2 + 1);
//     if (game_score_pl_1 > game_score_pl_2) {
//         printf("Player 1 Wins!");
//     } else if (game_score_pl_2 > game_score_pl_1) {
//         printf("Player 2 Wins!");
//     } else {
//         printf("It's a Tie!");
//     }

//     // Prompt to exit
//     move_cursor(SCREEN_WIDTH / 2 - 15, SCREEN_HEIGHT / 2 + 3);
//     printf("Press Enter to exit...");

//     // Flush output to ensure immediate display
//     fflush(stdout);
// }

// int prompt_play_again() {
//     char choice;
//     while (1) {
//         printf("Do you want to play again? (Y/N): ");
//         scanf(" %c", &choice);  // Space before %c skips whitespace/newline

//         if (choice == 'Y' || choice == 'y') {
//             return 1;  // Play again
//         } else if (choice == 'N' || choice == 'n') {
//             return 0;  // Exit
//         } else {
//             printf("Invalid input. Please enter Y or N.\n");
//         }
//     }
// }

// void show_final_message() {
//     // Clear screen
//     printf("\033[2J");

//     // Center the message
//     move_cursor(SCREEN_WIDTH / 2 - 7, SCREEN_HEIGHT / 2);
//     printf("PONGI-PONG!");

//     // Add a blank line below
//     move_cursor(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2 + 1);
//     printf("--------------------");

//     fflush(stdout);
// }
