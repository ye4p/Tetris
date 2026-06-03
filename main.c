#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <stdint.h>
#include <windows.h>

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
    right,
    down
};

typedef struct Shape
{
    int falls;
    int (*shape)[16];
    int shape_type;
    int pos_x;
    int pos_y;
} Shape;

//
//
//      FUNCTION DECLARATIONS
//
//

void update_game(Shape *falling_shape, int(*board), int (*shapes)[16]);
void handle_input(Shape *falling_shape, int(*board));
void render(int(*board));
void sleep_small_amount(void);
int find_board_square(Shape *falling_shape, int x, int y);
int can_move_dir(Shape *falling_shape, int(*board), int direction);
void set_random_shape(Shape *falling_shape, int (*shapes)[16]);
void draw(Shape *falling_shape, int(*board));
void erase(Shape *falling_shape, int(*board));
int can_rotate(Shape *falling_shape, int(*board), int(*arr));
int *rotate(Shape *falling_shape, int(*arr));
int check_board_row(int(*board));
void clear_row(int(*board), int row);
void bring_board_down(int(*board), int row);
void play_fall_animation(int(*board));

//
//
//

void sleep_small_amount(void)
{
    int sleep_amount = 500;
    int sleep_interval = 100;
    while (!_kbhit() && sleep_amount)
    {
        Sleep(sleep_interval);
        sleep_amount -= sleep_interval;
    }
}

int find_board_square(Shape *falling_shape, int x, int y)
{
    return (falling_shape->pos_x + x - 1) + (falling_shape->pos_y + y) * 10;
}

int can_move_dir(Shape *falling_shape, int(*board), int direction)
{
    erase(falling_shape, board); // Necessary so that won't consider itself as an obstacle
    if (direction == left)
    {
        for (int x = 0; x < 4; x++)
        {
            for (int y = 0; y < 4; y++)
            {
                if (!(*falling_shape->shape)[x + y * 4])
                {
                    continue;
                }

                // Check if OOB
                if ((falling_shape->pos_x - 3 + x) < 0)
                {
                    return 0;
                }

                // Check if hits another piece
                int square = find_board_square(falling_shape, x - 1, y);

                if (board[square])
                {
                    return 0;
                }
            }
        }
        return 1;
    }
    else if (direction == right)
    {
        for (int x = 3; x > -1; x--)
        {
            for (int y = 0; y < 4; y++)
            {
                if (!(*falling_shape->shape)[x + y * 4])
                {
                    continue;
                }

                // Check if OOB
                if ((falling_shape->pos_x + x - 1) > 9)
                {
                    return 0;
                }

                // Check if hits another piece
                int square = find_board_square(falling_shape, x + 1, y); // +1 cause checking if touches right

                // TODO: FIX WRONG Y indexing that is used to calculate square

                //  0   2
                //  1   1
                //  2   0
                //  3   -1

                if (board[square])
                {
                    return 0;
                }
            }
        }
        return 1;
    }
    else if (direction == down)
    {
        for (int y = 3; y > -1; y--)
        {
            for (int x = 0; x < 4; x++)
            {
                if (!((*falling_shape->shape)[x + y * 4]))
                {
                    continue;
                }

                // Handle other shapes below and ground
                int square = find_board_square(falling_shape, x - 1, y - 1);

                if (square < 0 || board[square])
                {
                    return 0;
                }
            }
        }
        return 1;
    }
}

void handle_input(Shape *falling_shape, int(*board))
{
    if (_kbhit())
    {
        char c = _getch();
        if (c == 'w')
        {
            int arr[16] = {0};
            rotate(falling_shape, arr);

            if (!can_rotate(falling_shape, board, arr))
            {
                return;
            }
            erase(falling_shape, board);

            memcpy(falling_shape->shape, arr, sizeof(arr));

            draw(falling_shape, board);
        }
        else if (c == 'a')
        {
            if (!can_move_dir(falling_shape, board, left))
            {
                return;
            }
            erase(falling_shape, board);
            falling_shape->pos_x--;
            draw(falling_shape, board);
        }
        else if (c == 'd')
        {
            if (!can_move_dir(falling_shape, board, right))
            {
                return;
            }
            erase(falling_shape, board);
            falling_shape->pos_x++;
            draw(falling_shape, board);
        }
        else if (c == 's')
        {
            if (!can_move_dir(falling_shape, board, down))
            {
                return;
            }
            erase(falling_shape, board);
            falling_shape->pos_y--;
            draw(falling_shape, board);
        }
    }
}

void update_game(Shape *falling_shape, int(*board), int (*shapes)[16])
{

    // Handle getting new piece
    if (falling_shape->falls != 1)
    {
        // printf("updating shape object");
        // Set new shape
        set_random_shape(falling_shape, shapes);

        // Play animation
        play_fall_animation(board);
    }

    if (!can_move_dir(falling_shape, board, down))
    {
        draw(falling_shape, board); // Might be extra draw that isn't actually needed
        set_random_shape(falling_shape, shapes);

        // Play animation
        play_fall_animation(board);
        return;
    }

    erase(falling_shape, board);
    falling_shape->pos_y--;
    draw(falling_shape, board);
}

void set_random_shape(Shape *falling_shape, int (*shapes)[16])
{
    srand(time(NULL));
    int random_number = rand() % 7;

    falling_shape->falls = 1;
    falling_shape->shape_type = random_number;
    falling_shape->shape = &shapes[random_number];
    falling_shape->pos_x = 4;
    falling_shape->pos_y = 21;
}

void draw(Shape *falling_shape, int(*board))
{
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            if (!((*falling_shape->shape)[x + 4 * y]))
            {
                continue;
            }

            int square = find_board_square(falling_shape, x - 1, y);
            board[square] = 1;
        }
    }
}

void erase(Shape *falling_shape, int(*board))
{
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            if (!((*falling_shape->shape)[x + 4 * y]))
            {
                continue;
            }

            int square = find_board_square(falling_shape, x - 1, y);
            board[square] = 0;

            // board[x - 1 + falling_shape->pos_x][falling_shape->pos_y - y] = 0;
        }
    }
}

int can_rotate(Shape *falling_shape, int(*board), int(*arr))
{
    erase(falling_shape, board);
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            if (!arr[x + 4 * y] && !(*falling_shape->shape)[x + 4 * y])
                continue;

            int square = find_board_square(falling_shape, x, y);

            // Check for being OOB
            int real_x = x - 2 + falling_shape->pos_x;
            int real_y = y + falling_shape->pos_y;
            if (real_x > 9 || real_x < 0 || real_y < 0)
            {
                draw(falling_shape, board);
                // printf("can't rotate, OOB with x: %i, and y:%i\n", real_x, real_y);
                return 0;
            }

            // Check for intersection with other shapes
            if (board[square])
            {
                // printf("can't rotate, space is taken\n");
                draw(falling_shape, board);
                return 0;
            }
        }
    }
    draw(falling_shape, board);
    return 1;
}

int *rotate(Shape *falling_shape, int(*arr))
{
    // Should rotate shape 90 degrees clockwise

    if (falling_shape->shape_type < 2)
    {
        int i = 0;

        for (int x = 0; x < 4; x++)
        {
            for (int y = 3; y > -1; y--)
            {
                arr[x + 4 * y] = (*falling_shape->shape)[i];
                i++;
            }
        }
    }
    else
    {
        int i = 4;

        for (int x = 0; x < 4; x++)
        {
            for (int y = 3; y > -1; y--)
            {
                if (x == 3 || y == 0)
                {
                    i += 1;
                    continue;
                }
                arr[x + 4 * y] = (*falling_shape->shape)[i];
                i++;
            }
        }
    }
}

//  0 0 0 0         0 1 0 0
//  0 0 0 0     --> 0 1 0 0
//  1 1 1 1         0 1 0 0
//  0 0 0 0         0 1 0 0

//  0 0 0 0         0 0 0 0
//  1 0 0 0     --> 0 1 1 0
//  1 1 1 0         0 1 0 0
//  0 0 0 0         0 1 0 0

// Returns row that is going to be removed
int check_board_row(int(*board))
{
    for (int y = 0; y < 22; y++)
    {
        int count = 0;
        for (int x = 0; x < 10; x++)
        {
            if (board[x + 10 * y])
            {
                count++;
            }
        }
        if (count == 10)
        {
            return y;
        }
    }
    return -1;
}

void clear_row(int(*board), int row)
{
    for (int x = 0; x < 10; x++)
    {
        board[x + 10 * row] = 0;
    }
}

void bring_board_down(int(*board), int row)
{
    render(board);
    Sleep(500);
    for (int y = row; y < 22; y++)
    {
        int count = 0;
        for (int x = 0; x < 10; x++)
        {
            board[x + y * 10] = board[x + (y + 1) * 10];
            if (!board[x + (y + 1) * 10])
                count++;
            board[x + (y + 1) * 10] = 0;
        }
        render(board);
        Sleep(500);
        if (count == 10)
            return;
    }
}

void play_fall_animation(int(*board))
{
    int row = check_board_row(board);
    while (row != -1)
    {
        clear_row(board, row);
        bring_board_down(board, row);
        row = check_board_row(board);
    }
}

void render(int(*board))
{
    for (int y = 22; y > -2; y--)
    {
        // printf("value of y is %i\n", y);
        if (y == -1)
        {
            printf("######################\n");
            continue;
        }
        for (int x = -1; x < 11; x++)
        {
            // printf("value of x is %i\n", x);
            if (x == -1 || x == 10)
            {
                printf("|");
                continue;
            }
            if (board[x + 10 * y] == 1)
            {
                // printf("1 at squares %i and %i", x, y);
                printf("[]");
            }
            // else if (board[x + 10 * y] == 2)
            // {
            //     printf("{}");
            // }
            // else if (board[x + 10 * y] == 3)
            // {
            //     printf("/\\");
            // }
            else
            {
                printf("  ");
            }
        }
        printf("\n");
    }
}

//
//      [][][]
//        []
//

//
//  The position is centered by following square:
//      0 0 0 0
//      0 0 0 0
//      0 1 0 0
//      0 0 0 0
//

//
//
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0            0 0 0 0
//  0 0 0 0 0 0 0 0 0 0            0 0 0 0
//  0 1 1 1 0 0 0 0 0 0     <--    1 1 1 0
//  0 1 0 0 0 0 0 0 0 0            1 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//  0 0 0 0 0 0 0 0 0 0
//      ^
//  0 1 2 3 4 5 6 7 8 9

int main()
{
    int board[230] = {0}; // Add extra 3 lines on top as a buffer
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

    int empty_arr[16]= {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };
    // clang-format on

    int game_running = 1;

    Shape falling_shape; // {1, &empty_arr, -1, -1};
    falling_shape.falls = 0;
    falling_shape.shape = malloc(sizeof(int) * 16);
    falling_shape.shape = &empty_arr;
    falling_shape.pos_x = -1;
    falling_shape.pos_y = -1;

    // set_random_shape(&falling_shape, shapes);

    // falling_shape.pos_y -= 3;
    // draw(&falling_shape, board);

    // falling_shape.pos_y -= 3;
    // int arr[16] = {0};
    // rotate(&falling_shape, arr);
    // memcpy(falling_shape.shape, arr, sizeof(arr));

    // draw(&falling_shape, board);

    // falling_shape.pos_y -= 3;
    // rotate(&falling_shape);
    // draw(&falling_shape, board);

    // falling_shape.pos_y -= 3;
    // rotate(&falling_shape);
    // draw(&falling_shape, board);

    render(board);

    while (game_running)
    {
        handle_input(&falling_shape, board);
        update_game(&falling_shape, board, shapes);
        render(board);

        sleep_small_amount();
    }
    free(falling_shape.shape);
    return 0;
}