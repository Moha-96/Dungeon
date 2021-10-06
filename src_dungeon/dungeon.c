#include "dungeon.h"

int main(int argc, char *argv[]){
    int opt;
    char *input_name = NULL;
    char *output_name = NULL;
    char *level_name = NULL;

    while((opt = getopt(argc, argv, "i:o:")) != -1){
        switch(opt){
            case 'i':
                if(input_name != NULL){
                    fprintf(stderr, "Too many input arguments\n"); 
                    return 3;
                }
                input_name = optarg;
                break;
            case 'o':
                if(output_name != NULL){
                    fprintf(stderr, "Too many output arguments\n"); 
                    return 3;
                }
                output_name = optarg;
                break;
        }
    }
    
    if(optopt != 0){
        fprintf(stderr, "unknown argument\n"); 
        return 3;
    }

    if(optind < argc){
        level_name = argv[optind];
    }

    if(optind < argc - 1){
        fprintf(stderr, "Too many positional arguments\n"); 
        return 3;
    }

    struct Dungeon *dungeon;
    dungeon = malloc(sizeof(Dungeon));
    dungeon->field = NULL;
    dungeon->listMonster = NULL;
    dungeon->listWall = NULL;
    dungeon->listExit = NULL;

    load_level(level_name, dungeon);
    FILE *output_stream = get_output_stream(output_name, dungeon);
    FILE *input_stream = get_input_stream(input_name, dungeon);

    int end = game(dungeon, output_stream, input_stream);
    print_game_result(end, output_stream);
    fclose(output_stream);
    fclose(input_stream);

    garbage_collection(dungeon);
    return 0;
}

void print_game_result(int end, FILE* output_stream){
    switch(end){
        case 1:
            fprintf(output_stream, "%s", "Du wurdest von einem Monster gefressen.\n");
            break;
        case 2:
            fprintf(output_stream, "%s", "Gewonnen!\n");
            break;
    }
}

void load_level(char* level_name, struct Dungeon *dungeon){
    int error;
    char default_level[] = "level/1.txt";
    
    if(level_name == NULL)
        error = read_level(dungeon, default_level);
    else{
        DIR *dir = opendir(level_name);
        if(dir != NULL){
            closedir(dir);
            fprintf(stderr, "Eingabe ist keine Datei sondern ein Ordner!\n"); 
            error = 2;
        }else{
            error = read_level(dungeon, level_name);
        }
    }
    if(error) {
        garbage_collection(dungeon);
        exit(error);
    }
}

FILE* get_input_stream(char* input_name, struct Dungeon *dungeon){
    FILE* input_stream;

    if(input_name == NULL)
        input_stream = stdin;
    else{
        DIR *dir = opendir(input_name);
        if(dir != NULL){
            closedir(dir);
            fprintf(stderr, "Eingabe ist keine Datei sondern ein Ordner!\n"); 
            garbage_collection(dungeon);
            exit(2);
        }
        input_stream = fopen(input_name, "r");
        if(input_stream == NULL) {
            fprintf(stderr, "Eingabe nicht lesbar!\n");
            garbage_collection(dungeon);
            exit(1);
        }
    }
    return input_stream;
}

FILE* get_output_stream(char* output_name, struct Dungeon *dungeon){
    FILE* output_stream;
    if(output_name == NULL)
        output_stream = stdout;
    else
        output_stream = fopen(output_name, "w");
    if(output_stream == NULL) {
        fprintf(stderr, "Ausgabe nicht schreibbar!\n");
        garbage_collection(dungeon);
        exit(1);
    }
    return output_stream;
}

int read_level(Dungeon *target_Dungeon, char *filename){
    char ch;
    int x = 0, y = 0, xmax = 0;
    FILE *input;
	input = fopen(filename, "r");

    if(input == NULL){
        fprintf(stderr, "Eingabe nicht lesbar!\n");
		return 1;
	}

    while((ch = fgetc(input)) != EOF){
        if(ch == '\n'){
            if(xmax < x){ 
				xmax = x;
			}
            x = 0;
            ++y;
            continue;
        }

		switch(ch){
			case 'S':
				set_player(target_Dungeon, x, y);
				break;
			case '#':
				add_wall(target_Dungeon, x, y);
				break;
			case 'A':
				add_exit(target_Dungeon, x, y);
				break;
			case '^':
				add_monster(target_Dungeon, x, y, 0);
				break;
			case '>':
				add_monster(target_Dungeon, x, y, 1);
				break;
			case 'v':
				add_monster(target_Dungeon, x, y, 2);
				break;
			case '<':
				add_monster(target_Dungeon, x, y, 3);
				break;
		}
        ++x;
    }

    target_Dungeon->width = xmax;
    target_Dungeon->height = y;

    create_field(target_Dungeon);

    fclose(input);

    return 0;
}

void add_wall(Dungeon *target_Dungeon, int x, int y){
    struct Wall *newWall;
    newWall = malloc(sizeof(Wall));
    newWall->x = x;
    newWall->y = y;

    newWall->next = target_Dungeon->listWall;
    ++(target_Dungeon->numWall);

    target_Dungeon->listWall = newWall;
}

void add_monster(Dungeon *target_Dungeon, int x, int y, int angle){
    Monster *newMonster;
    newMonster = malloc(sizeof(Monster));

    newMonster->x = x;
    newMonster->y = y;
    newMonster->angle = angle;

    newMonster->next = target_Dungeon->listMonster;

    target_Dungeon->listMonster = newMonster;
}

void add_exit(Dungeon *target_Dungeon, int x, int y){
    struct Exit *newExit;
    newExit = malloc(sizeof(Exit));
    newExit->x = x;
    newExit->y = y;

    newExit->next = target_Dungeon->listExit;
    ++(target_Dungeon->numExit);

    target_Dungeon->listExit = newExit;
}

void set_player(Dungeon *target_Dungeon, int x, int y){
    target_Dungeon->x = x;
    target_Dungeon->y = y;
}

void draw_field(Dungeon *target_Dungeon){
    int width = target_Dungeon->width;
    int height = target_Dungeon->height;

    for(int i = 0; i< width*height; ++i){
        target_Dungeon->field[i] = 0;
    }

    target_Dungeon->field[target_Dungeon->x + width * target_Dungeon->y] = 1;

    struct Monster *monster_ptr = NULL;
    monster_ptr = target_Dungeon->listMonster;
    while(monster_ptr != NULL){
        target_Dungeon->field[monster_ptr->x + width * monster_ptr->y] = 30 + monster_ptr->angle;
        monster_ptr = monster_ptr->next;
    }
    
    Wall *wall_ptr = NULL;
    wall_ptr = target_Dungeon->listWall;
    while(wall_ptr != NULL){
        target_Dungeon->field[wall_ptr->x + width * wall_ptr->y] = 2;
        wall_ptr = wall_ptr->next;
    }

    Exit *exit_ptr = NULL;
    exit_ptr = target_Dungeon->listExit;
    while(exit_ptr != NULL){
        target_Dungeon->field[exit_ptr->x + width * exit_ptr->y] = 4;
        exit_ptr = exit_ptr->next;
    }
}

void create_field(Dungeon *target_Dungeon){
    int width = target_Dungeon->width;
    int height = target_Dungeon->height;
    target_Dungeon->field = malloc(width*height*sizeof(int));

    draw_field(target_Dungeon);    
}

void print_dungeon(Dungeon *target_Dungeon, FILE *stream){
    int width = target_Dungeon->width;
    for(int i = 0; i < target_Dungeon->height; ++i){
        for(int j = 0; j < width; ++j){
            switch(target_Dungeon->field[i * width + j]){
				case 0:
					fprintf(stream, "%c", ' ');
					break;
				case 1:
					fprintf(stream, "%c", 'S');
					break;
				case 2:
					fprintf(stream, "%c", '#');
					break;
				case 4:
					fprintf(stream, "%c", 'A');
					break;
				case 30:
					fprintf(stream, "%c", '^');
					break;
				case 31:
					fprintf(stream, "%c", '>');
					break;
				case 32:
					fprintf(stream,"%c", 'v');
					break;
				case 33:
					fprintf(stream,"%c", '<');
					break;
				}	
        }
        fprintf(stream, "%c", '\n');
    }
}

int game(Dungeon *level, FILE *out_stream, FILE *in_stream){
    int counter = 1;
    int *count_ptr;
    count_ptr = &counter;
    char input;
    int game_state = 0;
    print_dungeon(level, out_stream);

    while(game_state == 0){
        input = fgetc(in_stream);
        if(input == '\n') 
			continue;

        if(input == EOF)
            break;

        switch(input){
            case 'w':
                make_move(level, 0, count_ptr, out_stream);
                fprintf(out_stream, "%c", input);
                fprintf(out_stream, "%c", '\n');
                move_monster(level);
                print_dungeon(level, out_stream);
                break;
            case 'd':
                make_move(level, 1, count_ptr, out_stream);
                fprintf(out_stream, "%c", input);
                fprintf(out_stream, "%c", '\n');
                move_monster(level);
                print_dungeon(level, out_stream);
                break;
            case 's':
                make_move(level, 2, count_ptr, out_stream);
                fprintf(out_stream, "%c", input);
                fprintf(out_stream, "%c", '\n');
                move_monster(level);
                print_dungeon(level, out_stream);
                break;
            case 'a':
                make_move(level, 3, count_ptr, out_stream);
                fprintf(out_stream, "%c", input);
                fprintf(out_stream, "%c", '\n');
                move_monster(level);
                print_dungeon(level, out_stream);
                break;
            case EOF:
                return 0;
                break;
            default:
                break;
        }
        game_state = check_game_state(level);

        if(game_state != 0){
			break;
		}
    }
    return game_state;
}

int check_game_state(Dungeon *dungeon){
    if(dungeon->field[dungeon->y * dungeon->width + dungeon->x] > 10) {
        return 1; // verloren
    }
    if(dungeon->field[dungeon->y * dungeon->width + dungeon->x] == 4) {
        return 2; // gewonnen
    }
    return 0;
}

void make_move(Dungeon *dungeon, int input, int *counter, FILE *out_stream){
    fprintf(out_stream, "%i", *counter);
    fprintf(out_stream, "%c", ' ');

    *counter = *counter + 1;

    switch(input){
        case 0:
            if(dungeon->y == 0){ 
				break; // Ist schon oben
			}
            if(dungeon->field[(dungeon->y - 1)* dungeon->width + dungeon->x] == 2){
                break; // Wand
            }
            --(dungeon->y);
            break;
        case 1:
            if(dungeon->x == (dungeon->width - 1)){ 
				break; // Ist schon oben
			}
            if(dungeon->field[(dungeon->x + 1) + dungeon->width * dungeon->y] == 2){
                break; // Wand
            }
            dungeon->x = dungeon->x + 1;
            break;
        case 2:
            if(dungeon->y == (dungeon->height - 1)){ 
				break; // Ist schon unten
			}
            if(dungeon->field[(dungeon->y + 1)* dungeon->width + dungeon->x] == 2){
                break; // Wand
            }
            dungeon->y = dungeon->y + 1;
            break;
        case 3:
            if(dungeon->x == 0){ 
				break; // Ist schon oben
			}
            if(dungeon->field[(dungeon->x - 1) + dungeon->width * dungeon->y] == 2){
                break; // Wand
            }
            dungeon->x = dungeon->x - 1;
            break;
    }
    draw_field(dungeon);
}

void move_monster(Dungeon *dungeon){
    Monster *monster_ptr;
    monster_ptr = dungeon->listMonster;
    while(monster_ptr != NULL){
        switch(monster_ptr->angle){
            case 0:
                if(monster_ptr->y == 0) {
                    monster_ptr->angle = 2; // flippen
                    break; // Ist schon oben
                }
                if(dungeon->field[(monster_ptr->y - 1)* dungeon->width + monster_ptr->x] == 2 ||
                   dungeon->field[(monster_ptr->y - 1)* dungeon->width + monster_ptr->x] == 4) {
                    monster_ptr->angle = 2; // flippen
                    break; // Wand
                }
                --(monster_ptr->y);
                break;
            case 1:
                if(monster_ptr->x == (dungeon->width) - 1) {
                    monster_ptr->angle = 3;
                    break; // Ist schon oben
                }
                if(dungeon->field[(monster_ptr->x + 1)+ dungeon->width * monster_ptr->y] == 2 ||
                   dungeon->field[(monster_ptr->x + 1)+ dungeon->width * monster_ptr->y] == 4){
                    monster_ptr->angle = 3;
                    break;
                }
                ++(monster_ptr->x);
                break;
            case 2:
                if(monster_ptr->y == (dungeon->height) - 1) {
                    monster_ptr->angle = 0;
                    break;
                }
                if(dungeon->field[(monster_ptr->y + 1)* dungeon->width + monster_ptr->x] == 2 ||
                   dungeon->field[(monster_ptr->y + 1)* dungeon->width + monster_ptr->x] == 4){
                    monster_ptr->angle = 0;
                    break;
                }
                ++(monster_ptr->y);
                break;
            case 3:
                if(monster_ptr->x == 0) {
                    monster_ptr->angle = 1;
                    break;
                }
                if(dungeon->field[(monster_ptr->x - 1)+ dungeon->width * monster_ptr->y] == 2 ||
                   dungeon->field[(monster_ptr->x - 1)+ dungeon->width * monster_ptr->y] == 4){
                    monster_ptr->angle = 1;
                    break;
                }
                --(monster_ptr->x);
                break;
        }
        monster_ptr = monster_ptr->next;
    }
    draw_field(dungeon);
}

void garbage_collection(Dungeon *dungeon){
    Monster *monster_ptr1;
    Monster *next1= NULL;
    Wall *wall_ptr1;
    Wall *next2 = NULL;
    Exit *exit_ptr1;
    Exit *next3 = NULL;

    monster_ptr1 = dungeon->listMonster;
    while(monster_ptr1 != NULL){
        next1 = monster_ptr1->next;
        free(monster_ptr1);
        monster_ptr1 = next1;
    }
    
    wall_ptr1 = dungeon->listWall;
    while(wall_ptr1 != NULL){
        next2 = wall_ptr1->next;
        free(wall_ptr1);
        wall_ptr1=next2;
    }

    exit_ptr1 = dungeon->listExit;
    while(exit_ptr1 != NULL){
        next3 = exit_ptr1->next;
        free(exit_ptr1);
        exit_ptr1=next3;
    }

    free(dungeon->field);
    free(dungeon);
}
