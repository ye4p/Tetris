#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <stdint.h>

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

enum Direction
{
    left,
    right
};

struct Shape
{
    int falls;
    int (*shape)[8];
    int pos_x;
    int pos_y;
};

int can_move_dir(struct Shape *falling_shape, int (*board)[10], int direction)
{
    if (direction == left)
    {
        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                if (!falling_shape->shape[x + y * 4])
                {
                    continue;
                }
                int shift = x - 1;
                if (0 <= (falling_shape->pos_x - shift))
                {
                    return 1;
                }
            }
        }
        return 0;
    }
    else
    {
        for (int x = 3; x > -1; x--)
        {
            for (int y = 0; y < 4; y++)
            {
                if (!falling_shape->shape[x + y * 4])
                {
                    continue;
                }
                int shift = x - 1;
                if (10 >= (falling_shape->pos_x + shift))
                {
                    return 1;
                }
            }
        }
        return 0;
    }
}

void handle_input(struct Shape *falling_shape, int (*board)[10])
{
    if (_kbhit())
    {
        char c = _getch();
        if (c == 'w')
        {
        }
        else if (c == 'a')
        {
            if (!can_move_dir(falling_shape, board, left))
            {
                return;
            }
            erase(&falling_shape, &board);
            falling_shape->pos_x--;
            draw(&falling_shape, &board);
        }
        else if (c == 's')
        {
            if (!can_move_dir(falling_shape, board, right))
            {
                return;
            }
            erase(&falling_shape, &board);
            falling_shape->pos_x++;
            draw(&falling_shape, &board);
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

    // TODO: rework for arrays size 16

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

void set_random_shape(struct Shape *falling_shape, int (*shapes)[16])
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
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 2; y++)
        {
            if (!falling_shape->shape[x + 4 * y])
            {
                continue;
            }

            board[x - 1 + falling_shape->pos_x][falling_shape->pos_y - y] = 1;
        }
    }
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
//  The position is centered by following square:
//      0 0 0 0
//      0 0 0 0
//      0 1 0 0
//      0 0 0 0
//

int main()
{
    int board[23][10] = {0}; // Add extra 3 lines on top as a buffer
    // clang-format off
    int shapes[7][16] = {
        {
            0, 0, 0, 0,
            0, 1, 1, 0, 
            0, 1, 1, 0,
            0, 0, 0, 0
        },
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
            1, 1, 1, 1,
            0, 0, 0, 0
        },
        {   0, 0, 0, 0,
            0, 1, 1, 0,
            1, 1, 0, 0,
            0, 0, 0, 0,
        },
        {   
            0, 0, 0, 0,
            1, 1, 0, 0,
            0, 1, 1, 0,
            0, 0, 0, 0
        },
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
            1, 1, 1, 0,
            1, 0, 0, 0
        },
        {
            0, 0, 0, 0,
            1, 0, 0, 0,
            1, 1, 1, 0,
            0, 0, 0, 0
        },
        {
            0, 0, 0, 0,
            0, 0, 0, 0,
            1, 1, 1, 0,
            0, 1, 0, 0
        }
    };
    // clang-format on
    int game_running = 1;

    struct Shape falling_shape = {0, -1, -1, -1};

    while (game_running)
    {
        handle_input(&falling_shape, &board);
        update_game(&falling_shape, &board, &shapes);
        render();

        sleep_small_amount();
    }

    return 0;
}