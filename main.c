#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <stdbool.h>
#include <time.h>

/*
* Snake game
* The program uses mostly local variables, so all functions have arguments.
* If some variables are made global, then the number of function arguments can be reduced and the code will be simpler.
*/

#define WIDTH 100
#define HEIGHT 40
#define LEN_SNAKE_START 10
#define SCORE_FOR_WIN_LEVEL 5
#define MAX_NUM_OF_LEVELS 3
#define SYMBOL_IN_MAP ' '
#define SIZE_MOVE 2

typedef struct
{
    int x;
    int y;
    char type;
    int dx;
    int dy;
} SNAKE;

typedef enum {up, down, left, right} Tmove;

void show_Start();
void init_Map(char(*map)[WIDTH+1], size_t size, int level, char type);
void init_Snake(SNAKE snake[], int len_body);
void put_snake_in_Map(char(*map)[WIDTH+1], const SNAKE snake[], int len_body);
void put_obj_in_Map(char(*map)[WIDTH+1], bool* obj_in_Map);
void move_Snake(char(*map)[WIDTH+1], SNAKE snake[], Tmove move, int (*move_mas)[SIZE_MOVE], int len_body);
void check_Collision(char(*map)[WIDTH+1], SNAKE snake[], int* len_body, int* life, bool* obj_in_Map, Tmove* move, Tmove* old, int* score);
void show_Map(const char(*map)[WIDTH+1], int level, int life, int score);
void show_Level(int lvl);
Tmove snake_Control(Tmove move, Tmove* old);
void show_End(int life);
void check_show_Win(char(*map)[WIDTH+1], size_t size, int* level, int* len_body, SNAKE snake[], bool* obj_in_Map, Tmove* move, Tmove* old, int* score);

int main()
{
    srand(time(NULL));

    int level = 1;
    int len_body = LEN_SNAKE_START;
    int life = 3;
    int score = 0;
    bool obj_in_Map = false;
    SNAKE* snake = malloc(len_body * sizeof(SNAKE));
    if (snake == NULL)
    {
        puts("Error allocate memory for snake");
        return 1;
    }
    Tmove move, old;  // Variables to control the snake and prevent reversal
    move = old = right;
    int move_mas[][SIZE_MOVE] = {{0, -1}, {0, 1}, {-1, 0}, {1, 0}}; // array which has paire constantes for moving snake
    char Map[HEIGHT][WIDTH+1];

    init_Map(Map, sizeof(Map), level, SYMBOL_IN_MAP);
    init_Snake(snake, len_body);
    show_Start();
    show_Level(level);

    do
    {
        put_snake_in_Map(Map, snake, len_body);
        put_obj_in_Map(Map, &obj_in_Map);
        show_Map(Map, level, life, score);
        move = snake_Control(move, &old);
        move_Snake(Map, snake, move, move_mas, len_body);
        check_Collision(Map, snake, &len_body, &life, &obj_in_Map, &move, &old, &score);
        show_End(life);
        check_show_Win(Map, sizeof(Map), &level, &len_body, snake, &obj_in_Map, &move, &old, &score);

        Sleep( 140 - (score < 5 ? ( (score + 1) * 10) : 60 ) );
    }
    while(GetKeyState(VK_ESCAPE) >= 0);

    free(snake);
    snake = NULL;
    return 0;
}

void show_Start()
{
    puts("Welcome to the SNAKE GAME from Alex_K");
    printf("Please use the console window size more than WIDTH - %d and HEIGHT - %d\n", WIDTH+10, HEIGHT);
    puts("Control of snake - W S A D, Exit - Ecs");
    printf("To win the level you need to collect %d symbols ""O"", the game has %d levels\n", SCORE_FOR_WIN_LEVEL, MAX_NUM_OF_LEVELS);
    puts("Press 's' for start");
    while (getch() != 's');
}

void setcur(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void show_Level(int lvl)
{
    system("cls");
    setcur(WIDTH/2-4, HEIGHT/2-3);
    printf("LEVEL %d", lvl);
    Sleep(2000);
}

void init_Map(char(*map)[WIDTH+1], size_t size, int level, char type)
{
    memset(map, type, size);
    for(int i = 0; i < WIDTH; i++)
        map[0][i] = map[HEIGHT-1][i] = '#';
    map[0][WIDTH] = map[HEIGHT-1][WIDTH] = '\0';
    for(int i = 1; i < HEIGHT-1; i++)
    {
        map[i][0] = map[i][WIDTH-1] = '#';
        map[i][WIDTH] = '\0';
    }
    if (level == 2)
    {
        for(int i = 1; i < HEIGHT/4; i++)
            for(int j = 1; j < WIDTH/4; j++)
                map[i][j] = '#';
        for(int i = HEIGHT/4*3; i < HEIGHT-1; i++)
            for(int j = 1; j < WIDTH/4; j++)
                map[i][j] = '#';
        for(int i = 1; i < HEIGHT/4; i++)
            for(int j = WIDTH/4*3; j < WIDTH-1; j++)
                map[i][j] = '#';
        for(int i = HEIGHT/4*3; i < HEIGHT-1; i++)
            for(int j = WIDTH/4*3; j < WIDTH-1; j++)
                map[i][j] = '#';
    }
    if (level == 3)
    {
        for(int i = 1; i < HEIGHT/3; i++)
            for(int j = WIDTH/3; j < WIDTH/3*2; j++)
                map[i][j] = '#';
        for(int i = HEIGHT/3*2; i < HEIGHT-1; i++)
            for(int j = WIDTH/3; j < WIDTH/3*2; j++)
                map[i][j] = '#';
    }
}

void show_Map(const char(*map)[WIDTH+1], int level, int life, int score)
{
    setcur(0,0);
    for(int i = 0; i < HEIGHT; i++)
    {
        printf("%s", map[i]);
        if (i == 3) printf(" level: %d", level);
        if (i == 4) printf("  life: %d", life);
        if (i == 5) printf(" score: %d", score);
        if (i != HEIGHT-1)
            printf("\n");
    }
}

void init_Snake(SNAKE snake[],int len_body)
{
    snake[0] = (SNAKE){WIDTH/2, HEIGHT/2, '@', 1, 0};
    for(int i = 1; i < len_body; i++)
        snake[i] = (SNAKE){WIDTH/2-i, HEIGHT/2, '#', 1, 0};
}

void put_snake_in_Map(char(*map)[WIDTH+1], const SNAKE snake[], int len_body)
{
    for(int i = 0; i < len_body; i++)
    {
        map[snake[i].y][snake[i].x] = snake[i].type;
    }
}

void put_obj_in_Map(char(*map)[WIDTH+1], bool* obj_in_Map)
{
    if (*obj_in_Map == true)
        return;
    else
    {
        int i, j;
        do
        {
            i = rand() % (HEIGHT-2) + 1;
            j = rand() % (WIDTH-2) + 1;
        }
        while(map[i][j] == '#' || map[i][j] == '@');
        map[i][j] = 'O';
        *obj_in_Map = true;
    }
}

void move_Snake(char(*map)[WIDTH+1], SNAKE snake[], Tmove move, int (*move_mas)[SIZE_MOVE], int len_body)
{
    snake[0].dx = move_mas[move][0];                        // give the head new offset values
    snake[0].dy = move_mas[move][1];
    map[snake[len_body-1].y][snake[len_body-1].x] = ' ';    // make the place of the snake's tail in the map empty

    // we move the entire body of the snake according to the displacement values ​​of each element
    for(int i = 0; i < len_body; i++)
    {
        snake[i].x += snake[i].dx;
        snake[i].y += snake[i].dy;
    }
    // we transfer all the values ​​of the movements of each element from the tail to the head
    for(int i = len_body-1; i > 0; i--)
    {
        snake[i].dx = snake[i-1].dx;
        snake[i].dy = snake[i-1].dy;
    }
}

void delete_Snake(char(*map)[WIDTH+1], SNAKE snake[], int len_body)
{
    for(int i = 1; i < len_body; i++)
        map[snake[i].y][snake[i].x] = ' ';
}

void check_Collision(char(*map)[WIDTH+1], SNAKE snake[], int* len_body, int* life, bool* obj_in_Map, Tmove* move, Tmove* old, int* score)
{
    if(map[snake[0].y][snake[0].x] == 'O')
    {
        (*score)++;
        system ("color 7F");
        Sleep (10);
        system ("color 0F");
        *obj_in_Map = false;
        (*len_body)++;
        snake = realloc(snake, sizeof(SNAKE) * (*len_body));
        if (snake == NULL)
        {
            puts("Error allocate memory for snake");
            exit(2);
        }
        snake[*len_body-1].x = snake[*len_body-2].x - snake[*len_body-2].dx;
        snake[*len_body-1].y = snake[*len_body-2].y - snake[*len_body-2].dy;
        snake[*len_body-1].type = snake[*len_body-2].type;
        snake[*len_body-1].dx = snake[*len_body-2].dx;
        snake[*len_body-1].dy = snake[*len_body-2].dy;
    }
    if(map[snake[0].y][snake[0].x] == '#')
    {
        system ("color 4F");
        Sleep (100);
        system ("color 0F");
        (*life)--;
        delete_Snake(map, snake, *len_body);
        if(*life == 0)
            return;
        *move = *old = right;
        init_Snake(snake, *len_body);
    }
}

Tmove snake_Control(Tmove move, Tmove* old)
{
    // Variable old for not reverse moving
    if(GetKeyState ('W') < 0) move = up;
    if(GetKeyState ('S') < 0) move = down;
    if(GetKeyState ('A') < 0) move = left;
    if(GetKeyState ('D') < 0) move = right;
    if( (*old == right && move == left) || (*old == left && move == right) || (*old == up && move == down) || (*old == down && move == up) )
        return *old;
    else
    {
        *old = move;
        return move;
    }
}

void show_End(int life)
{
    if (life != 0)
        return;
    else
    {
        system("cls");
        setcur(WIDTH/2-4, HEIGHT/2-3);
        printf("GAME OVER");
        Sleep(2000);
        exit(0);
    }
}

void check_show_Win(char(*map)[WIDTH+1], size_t size, int* level, int* len_body, SNAKE snake[], bool* obj_in_Map, Tmove* move, Tmove* old, int* score)
{
    if (*score < SCORE_FOR_WIN_LEVEL)
        return;
    if (*score >= SCORE_FOR_WIN_LEVEL)
    {
        (*level)++;
    }
    if (*level <= MAX_NUM_OF_LEVELS && *score >= SCORE_FOR_WIN_LEVEL)
    {
        (*score) = 0;
        show_Level(*level);
        *len_body = LEN_SNAKE_START;
        snake = realloc(snake, sizeof(SNAKE) * (*len_body));
        if (snake == NULL)
        {
            puts("Error allocate memory for snake");
            exit(3);
        }
        *obj_in_Map = false;
        *move = *old = right;
        init_Map(map, size, *level, SYMBOL_IN_MAP);
        init_Snake(snake, *len_body);
    }
    if (*level > MAX_NUM_OF_LEVELS && *score >= SCORE_FOR_WIN_LEVEL)
    {
        system("cls");
        setcur(WIDTH/2-8, HEIGHT/2-3);
        printf("YOU WIN GAME!!!");
        Sleep(2000);
        exit(0);
    }
}
