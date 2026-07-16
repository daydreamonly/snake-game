#include "stdio.h"
#define WIDTH 20
#define HEIGHT 10

struct Point {
    int x;
    int y;
};

enum direction {up, down, left, right};
enum direction current_direction = left;

struct Point snake[100];
int snake_length = 3;

int is_snake_segment(int x, int y) {
    for (int i = 0; i < snake_length; i++) {
        if (snake[i].x == x && snake[i].y == y) {
            return 1;
        }
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

void draw_field() {
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            if ((i == 0 || i == HEIGHT - 1) || (j == 0 || j == WIDTH - 1))  {
                putchar('#');
            } else if (is_snake_segment(j, i)) {
                putchar('O');
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
    init_snake();
    for (int i = 0; i < 5; i++) {
        draw_field();
        move_snake();
    }
    return 0;
}
