/**
 *  Credit to Datacorelinux for the score renderer
 * **/

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define GRID_W 640
#define GRID_H 480
#define CELL_SIZE 20
#define DELAY 100
#define CELLS_X (GRID_W/CELL_SIZE)
#define CELLS_Y (GRID_H/CELL_SIZE)

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
int GRID_X1, GRID_X2, GRID_Y1, GRID_Y2;
int SCREEN_W, SCREEN_H;

typedef enum{
	UP, DOWN, LEFT, RIGHT
}DIRECTION;

typedef struct node{
	int x, y;
	struct node* next;
	struct node* prev;
}node;

typedef struct{
	int x, y;
}food;

DIRECTION curr_dir = RIGHT;
node* head;
node* tail;
food* apple;
int length = 1;

node* init_node(int, int);
bool initialize();
void shutdown();
void draw_grid();
void draw_snake();
void draw_food();
bool move_snake();
bool update();
void grow();
void reset();

int main()
{
	srand(time(NULL));
	atexit(shutdown);
	if(!initialize()){
		exit(1);
	}

	SDL_Event event;
	bool quit = false;	

	while(!quit){
		if(!update()){
			reset();
		}
		SDL_Delay(DELAY);

		while(SDL_PollEvent(&event)){
			switch(event.type){
				case SDL_QUIT:
					quit = true;
					break;
				case SDL_KEYUP:
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym){
						case SDLK_ESCAPE:
							quit = true;
							break;
						case SDLK_w:
							if(curr_dir != DOWN){
								curr_dir = UP;
							}
							break;
						case SDLK_s:
							if(curr_dir != UP){
								curr_dir = DOWN;
							}
							break;
						case SDLK_a:
							if(curr_dir != RIGHT){
								curr_dir = LEFT;
							}
							break;
						case SDLK_d:
							if(curr_dir != LEFT){
								curr_dir = RIGHT;
							}
							break;
					}
					break;
			}
		}
	}

	return 0;
}

bool initialize(){
	if(SDL_Init(SDL_INIT_VIDEO) != 0){
		fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
		return false;	
	}
	printf("Initialization successful!\n");

	window = SDL_CreateWindow("Snake!", 
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP | SDL_WINDOW_SHOWN);
	if(!window){
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 
			SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if(!window || !renderer){
		return false;
	}

	SDL_GetWindowSize(window, &SCREEN_W, &SCREEN_H);
	GRID_X1 = (SCREEN_W/2 - GRID_W/2);
	GRID_Y1 = (SCREEN_H/2 - GRID_H/2);
	GRID_X2 = (SCREEN_W/2 + GRID_W/2);
	GRID_Y2 = (SCREEN_H/2 + GRID_H/2);

	head = init_node(CELLS_X/4 * CELL_SIZE + GRID_X1, CELLS_Y/2 * CELL_SIZE + GRID_Y1);
	tail = head;	

	apple = (food*)malloc(sizeof(food));
	apple->x = rand() % CELLS_X * CELL_SIZE + GRID_X1;
	apple->y = rand() % CELLS_Y * CELL_SIZE + GRID_Y1;

	return true;
}

void shutdown(){
	if(renderer){
		SDL_DestroyRenderer(renderer);
	}
	if(window){	
		SDL_DestroyWindow(window);
	}	
	/* FREE MEMORY */
	node* track = head;
	while(track != NULL){
		node* temp = track;
		track = track->next;
		free(temp);
	}

	SDL_Quit();	
}

void reset(){
	node* track = head;
	while(track != NULL){
		node* temp = track;
		track = track->next;
		free(temp);
	}
	head = init_node(CELLS_X/4 * CELL_SIZE + GRID_X1, CELLS_Y/2 * CELL_SIZE + GRID_Y1);
	tail = head;
	length = 1;
}

bool update(){
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	/* DRAW COMPONENTS */	
	draw_grid();
	draw_food();
	draw_snake();

	if(!move_snake()){
		return false;
	}

	if(head->x == apple->x && head->y == apple->y){
		grow();
		apple->x = rand() % CELLS_X * CELL_SIZE + GRID_X1;
		apple->y = rand() % CELLS_Y * CELL_SIZE + GRID_Y1;
	}


	SDL_RenderPresent(renderer);

	return true;
}

node* init_node(int x, int y){
	node* newNode = (node*)malloc(sizeof(node));
	newNode->x = x;
	newNode->y = y;
	newNode->next = NULL;
	newNode->prev = NULL;
	return newNode;
}

void draw_grid(){
	SDL_SetRenderDrawColor(renderer, 28, 28, 28, 255);

	unsigned int i, j;
	for(i=GRID_X1; i <= GRID_X2; i+=CELL_SIZE){
		SDL_RenderDrawLine(renderer, i, GRID_Y1, i, GRID_Y2);
		for(j=GRID_Y1; j <= GRID_Y2; j+=CELL_SIZE){
			SDL_RenderDrawLine(renderer, GRID_X1, j, GRID_X2, j);
		}
	}
}


void grow(){
	node* newNode;
	switch(curr_dir){
		case UP:
			newNode = init_node(tail->x, tail->y - CELL_SIZE);
			break;
		case DOWN:
			newNode = init_node(tail->x, tail->y + CELL_SIZE);
			break;
		case LEFT:
			newNode = init_node(tail->x - CELL_SIZE, tail->y);
			break;
		case RIGHT:
			newNode = init_node(tail->x + CELL_SIZE, tail->y);
			break;
	}
	tail->next = newNode;
	newNode->prev = tail;
	tail = newNode;
	length++;
}

void draw_snake(){
	SDL_SetRenderDrawColor(renderer, 0, 135, 95, 255);
	node* track = head;
	while(track != NULL){
		SDL_Rect seg = {
			.x = track->x,
			.y = track->y,
			.w = CELL_SIZE - 1,
			.h = CELL_SIZE - 1
		};
		SDL_RenderFillRect(renderer, &seg);

		track = track->next;
	}
}

void draw_food(){
	SDL_Rect app = {
		.x = apple->x,
		.y = apple->y,
		.w = CELL_SIZE,
		.h = CELL_SIZE
	};

	SDL_SetRenderDrawColor(renderer, 175,0,0,255);
	SDL_RenderFillRect(renderer, &app);
}

bool move_snake(){
	node* track_t = tail;
	while(track_t->prev != NULL){
		track_t->x = track_t->prev->x;
		track_t->y = track_t->prev->y;

		track_t = track_t->prev;
	}

	switch(curr_dir){
		case UP:
			head->y -= CELL_SIZE;
			break;
		case DOWN:
			head->y += CELL_SIZE;
			break;
		case LEFT:
			head->x -= CELL_SIZE;
			break;
		case RIGHT:
			head->x += CELL_SIZE;
			break;
	}
	
	/* CHECK WALL COLLISION */
	if(head->x < GRID_X1 - CELL_SIZE || head->x > GRID_X2 || head->y < GRID_Y1 - CELL_SIZE || head->y > GRID_Y2){
		return false;
	}

	/* CHECK COLLISION W/ ITSELF */
	node* track_h = head->next;
	while(track_h != NULL){
		if(head->x == track_h->x && head->y == track_h->y){
			return false;
		}
		track_h = track_h->next;
	}

	return true;
}
