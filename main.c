#include "stdio.h"
#define WIDTH 20
#define HEIGHT 10

struct Point {
    int x;
    int y;
};

struct Point snake[100];
int snake_length;

void draw_field() {
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            if ((i == 0 || i == HEIGHT - 1) || (j == 0 || j == WIDTH - 1))  {
                putchar('#');
            } else if (i == snake[0].y && j == snake[0].x) {
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
    snake_length = 1;
}

int main() {
    init_snake();
    draw_field();
    return 0;
}
