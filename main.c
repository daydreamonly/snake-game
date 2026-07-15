#include "stdio.h"
#define WIDTH 20
#define HEIGHT 10

void draw_field() {
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            if ((i == 0 || i == HEIGHT - 1) || (j == 0 || j == WIDTH - 1))  {
                putchar('#');
            } else {
                printf(" ");
            }
        }
        printf("\n");
    }
}

int main() {
    draw_field();
    return 0;
}
