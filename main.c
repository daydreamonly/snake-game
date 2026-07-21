#include "stdio.h"
#include "termios.h"
#include <asm-generic/ioctls.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#define WIDTH 20
#define HEIGHT 10
#define CLEAR "\033[2J\033[H"
#define ALT_SCREEN_ON "\033[?1049h"
#define ALT_SCREEN_OFF "\033[?1049l"
#define INITIAL_SNAKE_LENGTH 3

struct Point {
    int x;
    int y;
};

enum direction {up, down, left, right};
enum direction current_direction = left;

char field[WIDTH * HEIGHT];
struct Point snake[100];
struct Point apple;
int snake_length = INITIAL_SNAKE_LENGTH;
struct termios original, raw;

void set_cell(int x, int y, char c) {
    int index = WIDTH * y + x;
    field[index] = c;
}

void reset_termios() {
    tcsetattr(fileno(stdin), TCSANOW, &original);
    printf(ALT_SCREEN_OFF);
}

void init_termios() {
    printf(ALT_SCREEN_ON);
    tcgetattr(fileno(stdin), &original);
    if (tcgetattr(fileno(stdin), &original) == -1) {
        printf("tcgetattr failed\n");
    }
    raw = original;
    raw.c_lflag = raw.c_lflag & ~ICANON;
    raw.c_lflag = raw.c_lflag & ~ECHO;
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(fileno(stdin), TCSANOW, &raw);
    if (tcsetattr(fileno(stdin), TCSANOW, &raw) == -1) {
        printf("tcsetattr failed\n");
    }
    atexit(reset_termios);
}

int read_input() {
    int cs = getchar();
    if (cs == 27) {
        int c1 = getchar();
        if (c1 == 91) {
            int c2 = getchar();
            switch (c2) {
                case 65:
                    cs = 'w';
                    return cs;
                case 66:
                    cs = 's';
                    return cs;
                case 67:
                    cs = 'd';
                    return cs;
                case 68:
                    cs = 'a';
                    return cs;
            }
        }
    } else {
        return cs;
    }
    return 0;
}

int is_snake_segment(int start_index, int x, int y) {
    for (int i = start_index; i < snake_length; i++) {
        if (snake[i].x == x && snake[i].y == y) {
            return 1;
        }
    }
    return 0;
}

void init_apple() {
    do {
        apple.x = rand() % (WIDTH - 2) + 1;
        apple.y = rand() % (HEIGHT - 2) + 1;
    } while (is_snake_segment(0, apple.x, apple.y) != 0);
}


int check_colission() {
    if ((snake[0].y == 0 || snake[0].y == HEIGHT - 1) || (snake[0].x == 0 || snake[0].x == WIDTH - 1)) {
        return 1;
    } else if (is_snake_segment(1, snake[0].x, snake[0].y)) {
        return 1;
    }
    return 0;
}

int check_food(struct Point old_tail) {
    if (snake[0].x == apple.x && snake[0].y == apple.y) {
        snake_length += 1;
        snake[snake_length - 1] = old_tail;
        init_apple();
        return 1;
    }
    return 0;
}

void move_snake() {
    for (int i = snake_length - 1; i > 0; i--) {
        snake[i].x = snake[i - 1].x;
        snake[i].y = snake[i - 1].y;
    }

    switch (current_direction) {
        case up:
            snake[0].y -= 1;
            break;
        case down:
            snake[0].y += 1;
            break;
        case left:
            snake[0].x -= 1;
            break;
        case right: 
            snake[0].x += 1;
            break;
    }
}

void change_direction (char input) {
    switch (input) {
        case 'w':
            current_direction = up;
            break;
        case 'a':
            current_direction = left;
            break;
        case 's':
            current_direction = down;
            break;
        case 'd':
            current_direction = right;
            break;
    }
}

void init_field_buffer() {
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            if ((i == 0 || i == HEIGHT - 1) || (j == 0 || j == WIDTH - 1))  {
                set_cell(j, i, '#');
            } else if (is_snake_segment(0, j, i)) {
                set_cell(j, i, 'O');
            } else if (apple.x == j && apple.y == i) {
                set_cell(j, i, '@');
            } else {
                set_cell(j, i, ' ');
            }
        }
    }
}

void draw_field(struct winsize window) {
    for (int i = 0; i < WIDTH * HEIGHT; i ++) {
        if (i == 0) {
            for (int j = 0; j < (window.ws_row / 2) - (HEIGHT / 2); j++) {
                printf("\n");
            }
        }
        if (i % WIDTH == 0) {
            for (int k = 0; k < (window.ws_col / 2) - (WIDTH / 2); k++) {
                printf(" ");
            }
        }
        putchar(field[i]);
        if ((i + 1) % WIDTH == 0) {
            printf("\n");
        }
    }
}

void init_snake() {
    snake[0].x = WIDTH / 2;
    snake[0].y = HEIGHT / 2;

    for (int i = 1; i < snake_length; i++) {
        switch (current_direction) {
            case up:
                snake[i].y = snake[0].y + i;
                snake[i].x = snake[0].x;
                break;
            case down:
                snake[i].y = snake[0].y - i;
                snake[i].x = snake[0].x;
                break;
            case left:
                snake[i].x = snake[0].x + i;
                snake[i].y = snake[0].y;
                break;
            case right:
                snake[i].x = snake[0].x - i;
                snake[i].y = snake[0].y;
                break;
        }
    }
}

void update_display() {
    struct Point old_tail;
    old_tail = snake[snake_length - 1];
    move_snake();
    if (check_food(old_tail)) {
        set_cell(apple.x, apple.y, '@');
    } else {
        set_cell(old_tail.x, old_tail.y, ' ');
    }
    set_cell(snake[0].x, snake[0].y, 'O');
}

void handle_sigint(int sig) {
    exit(0);
}

void print_text(char text[], int gap) {
    int start_x = (WIDTH / 2) - (strlen(text) / 2);
    int start_y = HEIGHT / 2 - gap;
    for (int i = 0; i < strlen(text); i++) {
        set_cell(start_x + i, start_y, text[i]);
    }
}

void draw_game_over() {
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            if ((i == 0 || i == HEIGHT - 1) || (j == 0 || j == WIDTH - 1))  {
                set_cell(j, i, '#');
            } else {
                set_cell(j, i, ' ');
            }
        }
    }
    print_text("Game over", 2);
    print_text("r: restart", 1);
    print_text("q: quit", 0);
}

void play_game() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    snake_length = INITIAL_SNAKE_LENGTH;
    current_direction = left;
    init_snake();
    init_apple();
    init_field_buffer();
    while (1) {
        clearerr(stdin);
        change_direction(read_input());
        printf(CLEAR);
        draw_field(w);
        update_display();
        usleep(300000);
        if (check_colission()) {
            for (int k = 0; k < (w.ws_col / 2) - (9 / 2); k++) {
                printf(" ");
            }
            draw_game_over();
            printf(CLEAR);
            draw_field(w);
            while (1) {
                int key = getchar();
                clearerr(stdin);
                switch (key) {
                    case 'r':
                        return;
                    case 'q':
                        exit(0);
                }
                usleep(100000);
            }
        }
    }
}

int main() {
    srand(time(NULL));
    signal(SIGINT, handle_sigint);
    init_termios();
    while (1) {
        play_game();
    }
}
