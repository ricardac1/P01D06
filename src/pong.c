#include <stdio.h>
#include <termios.h>
#include <unistd.h>

#define width 80 + 3
#define height 25 + 2
#define racket 3
#define racket_horiz_shift 6
#define x_center (height - 2) / 2 + 1
#define y_center (width - 3) / 2 + 1

void make_field(int left_x, int right_x, int ball_x, int ball_y, int right_score, int left_score);
int move_racket_left(int left_x, char key);
int move_racket_right(int right_x, char key);
int move_ball_x(int ball_x, int ball_y, int ball_x_m);
int move_ball_y(int ball_y, int ball_y_m);
int move_ball_x_m(int ball_x, int ball_x_m);
int move_ball_y_m(int ball_y, int ball_y_m, int left, int right, int ball_x);
int count_left(int ball_y, int left_score);
int count_right(int ball_y, int right_score);

int main() {
    int ball_x = x_center, ball_y = y_center;
    int left = x_center - 3 + 1,
        right = x_center - 3 + 1;  // сдвиг ракеток | 0 = ракетка в верхних трех клетках
    int right_score = 0, left_score = 0;
    int ball_x_m = 1;
    int ball_y_m = 1;

    struct termios old, new;
    tcgetattr(STDIN_FILENO, &old);
    new = old;
    new.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new);
    fflush(stdout);

    do {
        make_field(left, right, ball_x, ball_y, right_score, left_score);
        char key = getchar();
        if (key != 'a' && key != 'z' && key != 'k' && key != 'm' && key != 'q' && key != ' ') {
            continue;
        }
        ball_x = move_ball_x(ball_x, ball_y, ball_x_m);
        ball_y = move_ball_y(ball_y, ball_y_m);
        ball_x_m = move_ball_x_m(ball_x, ball_x_m);
        ball_y_m = move_ball_y_m(ball_y, ball_y_m, left, right, ball_x);

        if (key == 'q') {
            printf("\033[H\033[J");
            printf(
                "\n    Game Over!    \n      %d "
                "| %d\n\n",
                right_score, left_score);
            return 0;
        }

        left = move_racket_left(left, key);
        right = move_racket_right(right, key);
        left_score = count_left(ball_y, left_score);
        right_score = count_right(ball_y, right_score);
    } while ((right_score < 21) && (left_score < 21));
    printf("\033[H\033[J");  // очиcтка поля
    printf(
        "\n    Game Over!  \n      %d | "
        "%d\n\n",
        right_score, left_score);
    return 0;
}

void make_field(int left_x, int right_x, int ball_x, int ball_y, int right_score, int left_score) {
    int left_y = racket_horiz_shift, right_y = width - racket_horiz_shift - 1;

    char border_1 = 45, border_2 = 124, ball_d = 111;

    printf("\033[H\033[J");
    printf("                    %d                                        %d                     \n",
           right_score, left_score);
    for (int x = 0; x < height; x++) {
        for (int y = 0; y < width; y++) {
            if (x == ball_x && y == ball_y) {
                printf("%c", ball_d);
                continue;
            }

            if (x == 0 || x == height - 1) {
                printf("%c", border_1);
                continue;
            }

            if (y == 0 || y == width - 1 || y == (width) / 2) {
                printf("%c", border_2);
                continue;
            }

            if (y == left_y && (x > left_x && x <= left_x + racket)) {
                printf("%c", border_2);
                continue;
            }

            if (y == right_y && (x > right_x && x <= right_x + 3)) {
                printf("%c", border_2);
                continue;
            }

            printf(" ");
        }
        printf("\n");
    }
}

int move_racket_left(int left_x, char key) {
    if (key == 'a' && left_x > 0) {
        left_x--;
    }
    if (key == 'z' && left_x <= height - racket - 3) {
        left_x++;
    }
    return left_x;
}

int move_racket_right(int right_x, char key) {
    if (key == 'k' && right_x > 0) {
        right_x--;
    }
    if (key == 'm' && right_x <= height - racket - 3) {
        right_x++;
    }
    return right_x;
}

int move_ball_x(int ball_x, int ball_y, int ball_x_m) {
    ball_x += ball_x_m;
    if (ball_y == 1 || ball_y == width - 1 - 1) {
        ball_x = x_center;
    }
    return ball_x;
}

int move_ball_y(int ball_y, int ball_y_m) {
    if (ball_y_m < 0) {
        ball_y--;
    } else {
        ball_y++;
    }
    if (ball_y == 0 || ball_y == width - 1) {
        ball_y = y_center;
    }
    return ball_y;
}

int move_ball_x_m(int ball_x, int ball_x_m) {
    if (ball_x == 1 || ball_x == height - 2) {
        ball_x_m *= -1;
    }
    return ball_x_m;
}

int move_ball_y_m(int ball_y, int ball_y_m, int left, int right, int ball_x) {
    if (ball_y - 1 == racket_horiz_shift && ball_x > left && ball_x <= left + racket) {
        ball_y_m *= -1;
    }
    if (ball_y + 1 == width - racket_horiz_shift - 1 && ball_x > right && ball_x <= right + racket) {
        ball_y_m *= -1;
    }
    return ball_y_m;
}

int count_left(int ball_y, int left_score) {
    if (ball_y == 1) {
        left_score++;
    }
    return left_score;
}

int count_right(int ball_y, int right_score) {
    if (ball_y == width - 2) {
        right_score++;
    }
    return right_score;
}
