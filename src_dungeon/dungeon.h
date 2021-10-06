#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <dirent.h>

typedef struct Dungeon{
    int x,y; // position des Spielers
    int width, height;
    int *field; // speicher nochmal alles codiert ab (fuer die Ausgabe)

    struct Monster *listMonster;
    int numMonster;

    struct Wall *listWall;
    int numWall;

    struct Exit *listExit;
    int numExit;
} Dungeon;

typedef struct Monster{
    int x,y; // Position des Spielers
    int angle; // Ausrichtung des Monsters

    int exist; // default=1, wenn es geloescht werden soll auf 0

    struct Monster *next;
} Monster;

typedef struct Wall{
    int x, y;
    struct Wall *next;
} Wall;

typedef struct Exit{
    int x, y;
    struct Exit *next;
} Exit;

int read_level(Dungeon *target_Dungeon, char *filename);
void add_wall(Dungeon *target_Dungeon, int x, int y);
void add_monster(Dungeon *target_Dungeon, int x, int y, int angle);
void add_exit(Dungeon *target_Dungeon, int x, int y);
void set_player(Dungeon *target_Dungeon, int x, int y);

void draw_field(Dungeon *target_Dungeon);
void create_field(Dungeon *target_Dungeon);

void print_dungeon(Dungeon *target_Dungeon, FILE *stream);

int game(Dungeon *level, FILE *out_stream, FILE *in_stream);
int check_game_state(Dungeon *dungeon);

void make_move(Dungeon *dungeon, int input, int *counter, FILE *out_stream);

void move_monster(Dungeon *dungeon);

void garbage_collection(Dungeon *dungeon);

FILE* get_output_stream(char* output_name, struct Dungeon *dungeon);
FILE* get_input_stream(char* input_name, struct Dungeon *dungeon);
void load_level(char* level_name, struct Dungeon *dungeon);
void print_game_result(int end, FILE* output_stream);
