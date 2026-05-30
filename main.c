#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>

enum Tetris
{
    O,
    I,
    S,
    Z,
    L,
    J,
    T
};

struct Shape
{
    int falls;
    int (*shape)[8];
    int pos_x;
    int pos_y;
};

void handle_input()
{
    if (_kbhit())
    {
        char c = _getch();
        if (c == 'w')
        {
        }
        else if (c == 'a')
        {
        }
        else if (c == 's')
        {
        }
        else if (c == 'd')
        {
        }
    }
}

void update_game(struct Shape *falling_shape, int (*board)[10], int (*shapes)[8])
{
    // Handle getting new piece
    if (falling_shape->falls != 1)
    {
        set_random_shape(falling_shape, shapes);
    }

    // Handle piece touching ground actual ground
    for (int i = 8; i >= 0; i--)
    {
        if (falling_shape->shape[i])
        {
            // Check if it is on actual ground
            if (!(((i > 3) && (falling_shape->pos_y <= 1)) || ((i < 4) && (falling_shape->pos_y == 0))))
            {
                continue;
            }
            // caught piece touching the grounds.
            draw(falling_shape, board);
            set_random_shape(falling_shape, shapes);
        }
    }
}

void set_random_shape(struct Shape *falling_shape, int (*shapes)[8])
{
    srand(time(NULL));
    int random_number = rand() % 7;

    falling_shape->falls = 1;
    falling_shape->shape = shapes[random_number];
    falling_shape->pos_x = 4;
    falling_shape->pos_y = 19;
}

void draw(struct Shape *falling_shape, int (*board)[10])
{
}

void erase(struct Shape *falling_shape, int (*board)[10])
{
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            if (!falling_shape->shape[x + 4 * y])
            {
                continue;
            }

            board[x - 1 + falling_shape->pos_x][falling_shape->pos_y - y] = 0;
        }
    }
}

void render()
{
}

//
//  The position is center by second upper square:
//      0 1 0 0
//      0 0 0 0
//

int main()
{
    int board[20][10] = {0};
    // clang-format off
    int shapes[7][8] = {
        {
            0, 1, 1, 0, 
            0, 1, 1, 0
        },
        {
            1, 1, 1, 1,
            0, 0, 0, 0
        },
        {
            0, 1, 1, 0,
            1, 1, 0, 0
        },
        {
            1, 1, 0, 0,
            0, 1, 1, 0
        },
        {
            1, 1, 1, 0,
            1, 0, 0, 0
        },
        {
            1, 0, 0, 0,
            1, 1, 1, 1,
        },
        {
            0, 1, 1, 1,
            0, 0, 1, 0
        }
    };
    // clang-format on
    int game_running = 1;

    struct Shape falling_shape = {0, -1, -1, -1};

    while (game_running)
    {
        handle_input();
        update_game(&falling_shape, &board, &shapes);
        render();

        sleep_small_amount();
    }

    return 0;
}