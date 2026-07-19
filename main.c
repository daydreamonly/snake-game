#include "stdio.h"
#include "termios.h"
#include <asm-generic/ioctls.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>
#define WIDTH 20
#define HEIGHT 10
#define CLEAR "\033[2J\033[H"

struct Point {
    int x;
    int y;
};

enum direction {up, down, left, right};
enum direction current_direction = left;

struct Point snake[100];
struct Point apple;
int snake_length = 3;
struct termios original, raw;

void reset_termios() {
    tcsetattr(fileno(stdin), TCSANOW, &original);
}

void init_termios() {
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
    int c1;
    int c2;
    int cs = getchar();
    if (cs == 27) {
        c1 = getchar();
        if (c1 == 91) {
            c2 = getchar();
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

int check_food() {
    if (snake[0].x == apple.x && snake[0].y == apple.y) {
        snake_length += 1;
        snake[snake_length - 1] = snake[snake_length - 2];
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

void draw_field(struct winsize window) {
    for (int i = 0; i < (window.ws_row / 2) - (HEIGHT / 2); i++) {
        printf("\n");
    }
    for(int i = 0; i < HEIGHT; i++) {
        for (int k = 0; k < (window.ws_col / 2) - (WIDTH / 2); k++) {
            printf(" ");
        }
        for(int j = 0; j < WIDTH; j++) {
            if ((i == 0 || i == HEIGHT - 1) || (j == 0 || j == WIDTH - 1))  {
                putchar('#');
            } else if (is_snake_segment(0, j, i)) {
                putchar('O');
            } else if (apple.x == j && apple.y == i) {
                putchar('@');
            } else {
                printf(" ");
            }
        }
        printf("\n");
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


int main() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    srand(time(NULL));
    init_termios();
    init_snake();
    init_apple();
    while (1) {
        clearerr(stdin);
        change_direction(read_input());
        printf(CLEAR);
        draw_field(w);
        usleep(500000);
        move_snake();
        check_food();
        if (check_colission()) {
            for (int k = 0; k < (w.ws_col / 2) - (9 / 2); k++) {
                printf(" ");
            }
            printf("Game over\n");
            break;
        }
    }
    return 0;
}
