#include <stdio.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>

#define ARRAY_ROWS 200

char** map;
int rows = 11;
int columns = 30;
int pos[2] = {5,14};
int history[ARRAY_ROWS][2];
int gameover = 0;
int points = 1;
char moving = 'u';

// SETUP FOR THE SCREEN AND GAMEPLAY LOOP
// --------------------------------------

void Sleep(unsigned int milliSeconds)
{
    struct timespec req = {0};

    time_t seconds  = (int) (milliSeconds / 1000);
    milliSeconds    = milliSeconds - (seconds * 1000);
    req.tv_sec      = seconds;
    req.tv_nsec     = milliSeconds * 1000000L;

    while (nanosleep(&req, &req) == -1)
        continue;
}

void setupCurses()
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, true);
}

void unsetCurses()
{
    keypad(stdscr, false);
    nodelay(stdscr, false);
    nocbreak();
    echo();
    endwin();
}

// ARRAY FOR THE SNAKE BODY
// ------------------------

struct TwoDArray {
    int arr[ARRAY_ROWS][2];
};

void initialize(struct TwoDArray* array)
{
    for (int i = 0; i < ARRAY_ROWS; i++) {
        for (int j = 0; j < 2; j++) {
            array->arr[i][j] = 0;
        }
    }
}

void insertAtBeginning(struct TwoDArray* array, int newItem1, int newItem2)
{
    memmove(array->arr + 1, array->arr, (ARRAY_ROWS - 1) * 2 * sizeof(int));

    array->arr[0][0] = newItem1;
    array->arr[0][1] = newItem2;
}

void printArray(struct TwoDArray* array)
{
    for (int i = 0; i < ARRAY_ROWS; i++) {
        for (int j = 0; j < 2; j++) {
            printf("%d ", array->arr[i][j]);
        }
        printf("\n");
    }
}

// MAP FUNCTIONS
// -------------

void allocMap()
{
    map = malloc(sizeof(char*) * rows);
    for (int i = 0; i < rows; i++)
    {
        map[i] = malloc(sizeof(char) * (columns+1));
    }
}

void freeMap()
{
    for (int i = 0; i < rows; i++)
    {
        free(map[i]);
    }
    free(map);
}

void readmap()
{
    FILE* f;
    f = fopen("map.txt", "r");

    if (f == 0)
    {
        printf("Couldn't read map");
        exit(1);
    }

    allocMap();
    
    for (int i = 0; i < rows; i++)
    {
        fscanf(f, "%s", map[i]);
    }
    fclose(f);
}

void printMap()
{
    for (int i = 0; i < rows; i++)
    {
        printw("%s\n", map[i]);
    }
}

genApple()
{
    int x = rand() % rows;
    int y = rand() % columns;

    if (map[x][y] == '@' || map[x][y] == '%')
    {
        genApple();
    }
    else { map[x][y] = '*'; }
}

// PLAYER FUNCTIONS
// ----------------

void moveBody(struct TwoDArray* body)
{
    for (size_t i = 0; i < points; i++)
    {
        map[body->arr[i][0]][body->arr[i][1]]  = '%';
    }

    if (points > 0)
        map[body->arr[points-1][0]][body->arr[points-1][1]] = '.';
    
}

void moveHead(char direction, struct TwoDArray* body)
{
    int x = pos[0];
    int y = pos[1];
    map[x][y] = '@';
    
    switch (direction)
    {
    case 'l':
        if (pos[1] > 0 && map[x][y-1] == '%')
        {
            gameover = 1;
            break;
        }
        if (pos[1] > 0)
        {
            if (map[x][y-1] != '*')
                map[x][y] = '.';
            else {
                points++;
                genApple();
            }
            
            insertAtBeginning(body, pos[0], pos[1]); 
            moveBody(body);        
            
            pos[1]--;
        }
        else
        {
            map[x][y] = '.';
            if (map[x][columns-1] == '.')
                pos[1] = columns-1;
            else {gameover=1;}
        }
        break;
    
    case 'r':
        if (pos[1] > 0 && map[x][y+1] == '%')
        {
            gameover = 1;
            break;
        }
        if (pos[1] < columns-1)
        {
            if (map[x][y+1] != '*')
                map[x][y] = '.';
            else {
                points++;
                genApple();
            }
            insertAtBeginning(body, pos[0], pos[1]); 
            moveBody(body);        

            pos[1]++;
        }
        else
        {
            map[x][y] = '.';
            if (map[x][0] == '.')
                pos[1] = 0;
            else {gameover=1;}
        }
        break;
    
    case 'u':
        if (pos[0] > 0 && map[x-1][y] == '%')
        {
            gameover = 1;
            break;
        }
        if (pos[0] > 0)
        {
            if (map[x-1][y] != '*')
                map[x][y] = '.';
            else {
                points++;
                genApple();
            }
            insertAtBeginning(body, pos[0], pos[1]); 
            moveBody(body);        

            pos[0]--;
        }
        else
        {
            map[x][y] = '.';
            if (map[rows-1][y] == '.')
                pos[0] = rows-1;
            else {gameover=1;}
        }
        break;
    
    case 'd':
        if (pos[0] > rows-1 && map[x+1][y] == '%')
        {
            gameover = 1;
            break;
        }
        if (pos[0] < rows-1)
        {
            if (map[x+1][y] != '*')
                map[x][y] = '.';
            else {
                points++;
                genApple();
            }
            insertAtBeginning(body, pos[0], pos[1]); 
            moveBody(body);        

            pos[0]++;
        }
        else
        {
            map[x][y] = '.';
            if (map[0][y] == '.')
                pos[0] = 0;
            else {gameover=1;}
        }
        
        break;

    default:
        break;
    }

    x = pos[0];
    y = pos[1];
    map[x][y] = '@';
}

// .................................................................

int main()
{
    readmap();

    struct TwoDArray body;
    initialize(&body);

    setupCurses();
    move(5, 10);
    printw("Press any key to start.");
    refresh();

    int input = getch();
    nodelay(stdscr, true);

    genApple();

    do {
        erase();
        printMap();
        printw("\nSCORE  %d", points-1);
        moveHead(moving, &body);
        input = getch();

        Sleep(140);

        if (input != ERR) 
        {
            if ((input == KEY_DOWN || input == 's') && moving != 'u') moving = 'd';
            else if ((input == KEY_LEFT || input == 'a') && moving != 'r') moving = 'l';
            else if ((input == KEY_RIGHT || input == 'd') && moving != 'l') moving = 'r';
            else if ((input == KEY_UP || input == 'w') && moving != 'd') moving = 'u';
            else if (input == 'q') gameover = 1;
        }
        
        refresh();
    } while (!gameover);

    move(5, 10);
    printw("  ------GAME OVER------");
    refresh();

    input = getch();

    freeMap();
    unsetCurses();
    exit(EXIT_SUCCESS);
}
