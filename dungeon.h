#include <getopt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    NORTH,
    EAST,
    SOUTH,
    WEST,
} Direction;

typedef struct Enemy {
    int id;
    Direction direction;
    int x_pos;  // column
    int y_pos;  // row
    struct Enemy* next;
} Enemy;

typedef struct Exit {
    int id;
    int x_pos;  // column
    int y_pos;  // row
    struct Exit* next;
} Exit;

typedef struct {
    int x_length;  // columns
    int y_length;  // rows

    int enemies_counter;
    int exits_counter;

    char** map;

    int x_player_pos;
    int y_player_pos;

    Enemy* enemies;
    Exit* exits;
    Enemy* enemiesFirstPtr;  // always expected to be at the beginning
    Exit* exitsFirstPtr;     // always expected to be at the beginning
} Level;

typedef enum GameState { PLAYING, LOST_DUE_TO_ENEMY_TOUCH, LEVEL_WON } GameState;

typedef enum ErrorState {
    SUCCESS,
    MULTIPLE_INPUT_FLAGS,
    MULTIPLE_OUTPUT_FLAGS,
    UNKNOWN_FLAG,
    FILE_IS_FOLDER,
    UNKNOWN_FILE,
    TOO_MANY_LEVEL_FILES,
    NO_PERMISSION,
    NO_SUCH_FILE,
    UNKNOWN_ERROR
} ErrorState;

typedef struct {
    int input_flag;
    int output_flag;
    char* input_filename;
    char* output_filename;
    char* level_filename;
    FILE* input;
    FILE* output;
    FILE* level_file;
    ErrorState error_state;
    char error_text[500];
} FlagsAndData;

// Level Init Functions
Level* initLengths(Level* level, FILE* file);

Level* addEnemy(Level* level, int row, int column, char enemy_char);

Level* addExit(Level* level, int row, int column);

Level* initLevel(Level* level, FILE* readFrom);

// Level Free Functions
void freeLevel(Level* level);

// Level Print Functions (Game Logic)
char getEnemyChar(Direction direction);

Direction getOppositeDirection(Direction c);

void printLevel(Level* level, FILE* out);

GameState nextFrame(Level* level);

// Util Functions

void closeFiles(FILE* input, FILE* output, FILE* level);

FlagsAndData* mallocFlagsAndData(FlagsAndData* data);

ErrorState readCommandLineArguements(FlagsAndData* data, int* argc, char* argv[]);

FlagsAndData* initFlagsAndData(FlagsAndData* data, int* argc, char* argv[]);

void freeFlagsAndData(FlagsAndData* data);

short handleErrors(FlagsAndData* data);

ErrorState openFileWithCheck(FlagsAndData* data, FILE** file, char* path, short readOrWrite);