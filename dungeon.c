#include "dungeon.h"
#include <errno.h>
// #include <time.h>

/* Viel Erfolg! */

/**
 * reads file and sets number of rows/columns
 * O(n)
 * @param level Level*
 * @param file FILE*
 * @return updated Level*
 **/
Level* initLengths(Level* level, FILE* file) {
    int max = 0;
    int current = 0;
    int c = 0;
    level->y_length = 0;
    while ((c = fgetc(file)) != EOF) {
        if (c == '\n') {
            if (max < current) max = current;
            current = 0;
            level->y_length++;
        }
        current++;
    }
    rewind(file);
    level->x_length = max;

    return level;
}

/**
 * adds Enemy to level on position row|column
 * @param level Level*
 * @param row int
 * @param column int
 * @param enemy_char char
 * @return updated Level*
 **/
Level* addEnemy(Level* level, int row, int column, char enemy_char) {
    Enemy* enemy = NULL;
    enemy = malloc(sizeof(Enemy));
    enemy->id = level->enemies_counter;
    enemy->x_pos = column;
    enemy->y_pos = row;
    enemy->next = NULL;
    enemy->direction = enemy_char == '^'   ? NORTH
                       : enemy_char == '>' ? EAST
                       : enemy_char == 'v' ? SOUTH
                                           : WEST;
    if (level->enemiesFirstPtr == NULL) {
        level->enemiesFirstPtr = enemy;
        level->enemies = enemy;
    } else {
        level->enemies->next = enemy;
        level->enemies = enemy;
    }
    level->enemies_counter++;

    return level;
}

/**
 * adds Exit to level on position row|column
 * @param level Level*
 * @param row int
 * @param column int
 * @return updated Level*
 **/
Level* addExit(Level* level, int row, int column) {
    Exit* exit = NULL;
    exit = malloc(sizeof(Exit));
    exit->id = level->exits_counter;
    exit->x_pos = column;
    exit->y_pos = row;
    exit->next = NULL;

    if (level->exitsFirstPtr == NULL) {
        level->exitsFirstPtr = exit;
        level->exits = exit;
    } else {
        level->exits->next = exit;
        level->exits = exit;
    }

    level->exits_counter++;

    return level;
}

/**
 * called to free malloced Level struct
 * @param level Level*
 * @param readFrom FILE* to read/init level from
 * @return updated Level*
 **/
Level* initLevel(Level* level, FILE* readFrom) {
    level = malloc(sizeof(Level));
    initLengths(level, readFrom);
    level->map = malloc((sizeof(char*)) * (level->y_length + 1));
    // printf("x: %d y: %d \n", level->x_length, level->y_length);
    level->exits = NULL;
    level->enemies = NULL;
    level->exitsFirstPtr = NULL;
    level->enemiesFirstPtr = NULL;
    level->enemies_counter = 0;
    level->exits_counter = 0;

    int charPtr = 0;
    int column = 0;
    int row = 0;
    level->map[0] = malloc((level->x_length + 1) * sizeof(char));
    while (((charPtr = fgetc(readFrom)) != EOF)) {
        // printf("Row: %d, Column: %d, Char: %c \n", row, column, charPtr);

        if (charPtr == '\n') {  // reached end of line
            if (column < level->x_length) {
                level->map[row][column] = '\0';
            }
            column = 0;
            row++;
            level->map[row] = malloc((level->x_length + 1) * sizeof(char));
            continue;
        }

        if (charPtr == '#') {  // write Wall to Level
            level->map[row][column] = charPtr;
            column++;
            continue;
        } else {  // write EmptySpace to Level
            level->map[row][column] = ' ';
        }
        // add Player
        if (charPtr == 'S') {
            level->x_player_pos = column;
            level->y_player_pos = row;
        }
        // add Exit
        else if (charPtr == 'A') {
            addExit(level, row, column);
        }
        // add Enemy
        else if (charPtr == '^' || charPtr == '>' || charPtr == 'v' || charPtr == '<') {
            addEnemy(level, row, column, charPtr);
        }
        column++;
    }
    free(level->map[row]);

    level->x_length--;
    level->enemies = level->enemiesFirstPtr;
    level->exits = level->exitsFirstPtr;

    // printf("%d %d \n", level->exits_counter, level->enemies_counter);
    // printf("%d %d \n", level->x_length, level->y_length);

    return level;
}

/**
 * called to free malloced Level struct
 * @param level Level*
 * @return void
 **/
void freeLevel(Level* level) {
    // free enemies
    level->enemies = level->enemiesFirstPtr;
    while (level->enemies) {
        Enemy* tmp = level->enemies->next;
        free(level->enemies);
        level->enemies = tmp;
    }
    // free exits
    level->exits = level->exitsFirstPtr;
    while (level->exits) {
        Exit* tmp = level->exits->next;
        free(level->exits);
        level->exits = tmp;
    }
    // free map
    for (int row = 0; row < level->y_length; row++) {
        free(level->map[row]);
    }
    free(level->map);
    free(level);
}

/**
 * returns char according to direction of enemy
 * @param direction Direction enum
 * @return char according to direction of enemy
 **/
char getEnemyChar(Direction direction) {
    switch (direction) {
        case NORTH:
            return '^';
        case EAST:
            return '>';
        case SOUTH:
            return 'v';
        case WEST:
            return '<';
    }
    return '_';
}

/**
 * prints given level to FILE* output
 * @param level Level*
 * @param out FILE* of output
 * @return void
 **/
void printLevel(Level* level, FILE* out) {
    for (int row = 0; row < level->y_length; row++) {               // print each row
        for (int column = 0; column < level->x_length; column++) {  // print each column
            if (level->map[row][column] == '\0') break;             // early exit
            short object_priority = 0;

            // check whether to print an Exit and go to next loop if so
            level->exits = level->exitsFirstPtr;
            while (level->exits) {
                if (level->exits->x_pos == column && level->exits->y_pos == row) {
                    object_priority = 3;
                    break;
                }
                level->exits = level->exits->next;
            }
            if (object_priority == 3) {
                fprintf(out, "%c", 'A');
                continue;
            }

            // check whether to print an Enemy and go to next loop if so
            level->enemies = level->enemiesFirstPtr;
            while (level->enemies) {
                if (level->enemies->x_pos == column && level->enemies->y_pos == row) {
                    object_priority = 2;
                    break;
                }
                level->enemies = level->enemies->next;
            }
            if (object_priority == 2) {
                fprintf(out, "%c", getEnemyChar(level->enemies->direction));
                continue;
            }

            // check whether to print the Player and go to next loop if so
            if (level->x_player_pos == column && level->y_player_pos == row) {
                object_priority = 1;
                fprintf(out, "%c", 'S');
                continue;
            }

            // default: print "map" as is
            fprintf(out, "%c", level->map[row][column]);

            // end of one column loop
        }
        // print end of line
        fprintf(out, "\n");
        // end of one row loop
    }
    // end of both loops
}

/**
 * util function to inverse directions
 * @param c Direction to inverse
 * @return opposite Direction
 **/
Direction getOppositeDirection(Direction c) {
    if (c == NORTH)
        return SOUTH;
    else if (c == SOUTH)
        return NORTH;
    else if (c == WEST)
        return EAST;
    else if (c == EAST)
        return WEST;
    return SOUTH;
}

/**
 * Gamelogic happens (except of player movement) here.
 * e.g. enemy movement and checking whether player is eaten or has won.
 * @param level Level*
 * @return GameState
 **/
GameState nextFrame(Level* level) {
    GameState currentState = PLAYING;

    // Enemies loop (updates Enemies)
    level->enemies = level->enemiesFirstPtr;
    while (level->enemies) {
        /*printf("current positon enemy: %d %d %d \n", level->enemies->id, level->enemies->x_pos,
               level->enemies->y_pos); */
        Direction current = level->enemies->direction;
        int nextRowPos = level->enemies->y_pos;
        int nextColumnPos = level->enemies->x_pos;

        if (current == NORTH) {
            nextRowPos--;
            nextRowPos = nextRowPos <= 0 ? 0 : nextRowPos;
        } else if (current == SOUTH) {
            nextRowPos++;
            nextRowPos = nextRowPos >= level->y_length ? level->y_length : nextRowPos;
        } else if (current == EAST) {
            nextColumnPos++;
            nextColumnPos = nextColumnPos >= level->x_length ? level->x_length : nextColumnPos;
        } else if (current == WEST) {
            nextColumnPos--;
            nextColumnPos = nextColumnPos <= 0 ? 0 : nextColumnPos;
        }
        // printf("next positon enemy: %d %d %d \n", level->enemies->id, nextColumnPos,
        // nextRowPos);

        if (level->map[nextRowPos][nextColumnPos] == '#') {
            // hit a wall, so change direction to opposite direction
            level->enemies->direction = getOppositeDirection(current);
        } else if (level->map[nextRowPos][nextColumnPos] == ' ') {
            level->exits = level->exitsFirstPtr;
            short hitAPseodoWall = 0;  // due to poor design decisions, we need to check whether an
                                       // enemy has hit an Exit, which behaves like a wall
            while (level->exits) {
                if (level->exits->x_pos == nextColumnPos && level->exits->y_pos == nextRowPos) {
                    level->enemies->direction = getOppositeDirection(current);
                    hitAPseodoWall = 1;
                    break;
                }
                level->exits = level->exits->next;
            }
            // if Enemy actually can move to the new position
            if (!hitAPseodoWall) {
                level->enemies->x_pos = nextColumnPos;
                level->enemies->y_pos = nextRowPos;
            }
        }
        // if Enemy hit the Player
        if (level->enemies->x_pos == level->x_player_pos &&
            level->enemies->y_pos == level->y_player_pos) {
            currentState = LOST_DUE_TO_ENEMY_TOUCH;
        }
        level->enemies = level->enemies->next;
    }

    // Exits loop (checks whether Player has won)
    level->exits = level->exitsFirstPtr;
    while (level->exits) {
        if (level->exits->x_pos == level->x_player_pos &&
            level->exits->y_pos == level->y_player_pos) {
            currentState = LEVEL_WON;
        }
        level->exits = level->exits->next;
    }

    return currentState;
}

/**
 * helper function to malloc FlagsAndData struct (used to initialize it later)
 * @param data FlagsAndData*
 * @return malloced FlagsAndData struct
 **/
FlagsAndData* mallocFlagsAndData(FlagsAndData* data) {
    data = malloc(sizeof(FlagsAndData));

    data->input = NULL;
    data->output = NULL;
    data->level_file = NULL;

    data->input_flag = 0;
    data->output_flag = 0;

    return data;
}

/**
 * helper function to test file errors
 * @param data FlagsAndData struct to write error text into
 * @param file FILE* to read or write
 * @param path path string to file
 * @param readOrWrite boolean: 1 if read mode, 0 if write mode
 * @return malloced FlagsAndData struct
 **/
ErrorState openFileWithCheck(FlagsAndData* data, FILE** file, char* path, short readOrWrite) {
    errno = 0;
    *file = readOrWrite ? fopen(path, "r+") : fopen(path, "w");
    if (errno == 0) {
        return SUCCESS;
    }
    if (errno == EISDIR) {
        sprintf(data->error_text, "Fehler: ...'%-.20s'... ist ein Ordner. ", path);
        return FILE_IS_FOLDER;
    }
    if (errno == EPERM || errno == EACCES) {
        sprintf(data->error_text, "Fehler: Nicht genug Rechte um auf ...'%-.20s'... zuzugreifen. ",
                path);
        return NO_PERMISSION;
    }
    if (errno == ENOENT) {
        sprintf(data->error_text, "Fehler: Datei ...'%-.20s'... existiert nicht. ", path);
        return NO_SUCH_FILE;
    }
    return UNKNOWN_ERROR;
}

/**
 * helper function to read command line arguements and write them in FlagsAndData struct
 * @param data FlagsAndData* to data
 * @param argc int* to argc
 * @param argv char** to argv
 * @return ErrorState
 **/
ErrorState readCommandLineArguements(FlagsAndData* data, int* argc, char* argv[]) {
    int input_args_counter = 0;

    int option;
    while ((option = getopt(*argc, argv, "i:o:")) != -1) {
        switch (option) {
            case 'i':
                if (data->input_flag != 1) {
                    data->input_flag = 1;
                    data->input_filename = optarg;
                } else {
                    sprintf(data->error_text, "Fehler: Doppelte Flag. '-i' \n");
                    return MULTIPLE_INPUT_FLAGS;
                }
                break;
            case 'o':
                if (data->output_flag != 1) {
                    data->output_flag = 1;
                    data->output_filename = optarg;
                } else {
                    sprintf(data->error_text, "Fehler: Doppelte Flag. '-o' \n");
                    return MULTIPLE_OUTPUT_FLAGS;
                }
                break;
            default:
                sprintf(data->error_text, "Fehler: Unbekannte Flag. '-%c' \n", option);
                return UNKNOWN_FLAG;
        }
    }
    // Funktionsargumente abseits von Flags und deren Argumenten
    for (int i = optind; i < *argc; i++) {
        input_args_counter++;
        // wir erwarten nur ein Funktionsargument, deswegen kann man das so machen
        data->level_filename = argv[i];
    }
    if (input_args_counter >= 2) {
        sprintf(data->error_text,
                "Fehler: Nicht genau eine Leveldatei! Anzahl : %d, Letzte gelesene Eingabe: %s",
                input_args_counter, data->level_filename);
        return TOO_MANY_LEVEL_FILES;
    }

    return SUCCESS;
}

/**
 * function to initialize a new FlagsAndData struct
 * @param data FlagsAndData* (, which should be initialized with NULL before calling
 * this function)
 * @param argc int* to argc
 * @param argv char** to argv
 * @return fully initialized FlagsAndData struct
 **/
FlagsAndData* initFlagsAndData(FlagsAndData* data, int* argc, char* argv[]) {
    data = mallocFlagsAndData(data);

    data->error_state = readCommandLineArguements(data, argc, argv);
    if (data->error_state != SUCCESS) return data;

    if (!data->input_flag)
        data->input = stdin;
    else {
        data->error_state = openFileWithCheck(data, &data->input, data->input_filename, 1);
        if (data->error_state != SUCCESS) return data;
    }

    if (!data->output_flag)
        data->output = stdout;
    else {
        data->error_state = openFileWithCheck(data, &data->output, data->output_filename, 0);
        if (data->error_state != SUCCESS) return data;
    }

    if (data->level_filename == NULL)
        data->level_file = fopen("./level/1.txt", "r");
    else {
        data->error_state = openFileWithCheck(data, &data->level_file, data->level_filename, 1);
        if (data->error_state != SUCCESS) return data;
    }

    return data;
}

/**
 * util function to close all files opened in this game
 * @param input FILE* to desired input
 * @param output FILE* to desired output
 * @param level FILE* to loaded level file
 * @return void
 **/
void closeFiles(FILE* input, FILE* output, FILE* level) {
    if (input && input != stdin) fclose(input);
    if (output && output != stdout) fclose(output);
    if (level) fclose(level);
}

/**
 * util function to free FlagsAndData struct
 * @param data FlagsAndData*
 * @return void
 **/
void freeFlagsAndData(FlagsAndData* data) {
    if (data == NULL) return;
    closeFiles(data->input, data->output, data->level_file);

    free(data);
}

/**
 * util function to handle errors, which could happen while initializing FlagsAndData struct
 * @param data FlagsAndData*
 * @return short value mapped to ErrorState
 **/
short handleErrors(FlagsAndData* data) {
    if (data->error_state == SUCCESS) return 0;  // early exit

    fprintf(stderr, "%s", data->error_text);

    if (data->error_state == MULTIPLE_INPUT_FLAGS || data->error_state == MULTIPLE_OUTPUT_FLAGS ||
        data->error_state == TOO_MANY_LEVEL_FILES || data->error_state == UNKNOWN_FLAG) {
        return 3;
    }
    if (data->error_state == FILE_IS_FOLDER) {
        return 2;
    }
    if (data->error_state == UNKNOWN_FILE || data->error_state == NO_PERMISSION) {
        return 1;
    }

    return data->error_state;
}

int main(int argc, char* argv[]) {
    // clock_t start_time = clock();
    FlagsAndData* data = NULL;
    data = initFlagsAndData(data, &argc, argv);
    int error_state = handleErrors(data);

    if (error_state != 0) {
        freeFlagsAndData(data);
        return error_state;
    }

    Level* level = NULL;
    level = initLevel(level, data->level_file);

    printLevel(level, data->output);

    char control = 0;
    int turn = 1;
    while (1) {
        fscanf(data->input, "%c", &control);
        if (!(control == 'w' || control == 'a' || control == 's' || control == 'd')) {
            continue;
        }
        switch (control) {
            case 'w':
                if (level->map[level->y_player_pos - 1][level->x_player_pos] == ' ')
                    level->y_player_pos--;

                break;
            case 'a':
                if (level->map[level->y_player_pos][level->x_player_pos - 1] == ' ')
                    level->x_player_pos--;

                break;
            case 's':
                if (level->map[level->y_player_pos + 1][level->x_player_pos] == ' ')
                    level->y_player_pos++;

                break;
            case 'd':
                if (level->map[level->y_player_pos][level->x_player_pos + 1] == ' ')
                    level->x_player_pos++;

                break;
        }
        fprintf(data->output, "%d %c\n", turn, control);
        turn++;

        GameState currentState = nextFrame(level);
        printLevel(level, data->output);
        if (currentState == LOST_DUE_TO_ENEMY_TOUCH) {
            fprintf(data->output, "Du wurdest von einem Monster gefressen.\n");
            break;
        }
        if (currentState == LEVEL_WON) {
            fprintf(data->output, "Gewonnen!\n");
            break;
        }
    }

    freeLevel(level);
    freeFlagsAndData(data);
    // closeFiles(data->input, data->output, data->level_file);

    // double elapsed_time = (double)(clock() - start_time) / CLOCKS_PER_SEC;
    // printf("Done in %f seconds\n", elapsed_time);
    return 0;
}