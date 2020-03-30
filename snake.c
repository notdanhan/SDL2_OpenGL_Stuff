#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <assert.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

//GLOBALS (I can use globals in this as it's not made to be extended)
uint64_t pixel = 1;
unsigned char running = 1;

typedef struct coordinate coord;

typedef struct coordinate {
	unsigned char x;
	unsigned char y;
	coord * next;
} coord;

typedef struct snake {
	unsigned char size;
	short int score;
	char x_change;
	char y_change;
	coord * head;
} snake;

snake * snakesetup();
void update(snake *, coord *,SDL_Window *);
void draw(coord *, SDL_Window *, SDL_GLContext);
void gameover(snake *,SDL_Window *);

int main() {
	printf("Snake\n");
	SDL_Window * Window;
	SDL_Init(SDL_INIT_VIDEO);
	Window = SDL_CreateWindow("Snake",0,0,600,600,SDL_WINDOW_OPENGL);
	assert(Window);
	SDL_GLContext glcontext = SDL_GL_CreateContext(Window);
	unsigned char updating = 0;
	snake * player = snakesetup();
	coord * food = (coord *) malloc(sizeof(coord));
	srand(time(0));
	food->x = rand() % 64;
	food->y = rand() % 64;
	while(running) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch (event.type)
			{
			case SDL_QUIT:
				running = 0;
				break;
			
			case SDL_KEYDOWN:
				switch(event.key.keysym.sym) {
					case SDLK_ESCAPE:
						running = 0;
						break;
					case SDLK_SPACE:
						updating = !updating;
						if(updating) {
							SDL_SetWindowTitle(Window,"Snake");
						}
						else {
							SDL_SetWindowTitle(Window,"Snake (Paused)");
						}
						break;
					case SDLK_a:
						if(updating) {
							player->x_change = -1;
							player->y_change = 0;
						}
						break;
					case SDLK_d:
						if(updating) {
							player->x_change = 1;
							player->y_change = 0;
						}
						break;
					case SDLK_s:
						if(updating) {
							player->y_change = 1;
							player->x_change = 0;
						}
						break;
					case SDLK_w:
						if (updating) {
							player->y_change = -1;
							player->x_change = 0;
						}
						break;
				}
			}
		}
		if (updating) {
			update(player,food,Window);
		}
		glViewport(0,0,600,600);
		glClearColor(0.f,0.f,0.f,0.f);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,64,64,0,-1.0,1.0); //Translates Canvas coordinates so that top left is 0,0 and bottom right is 64,64
		glClear(GL_COLOR_BUFFER_BIT);
		draw(player->head,Window,glcontext);
		draw(food,Window,glcontext);
		glFlush();
		SDL_GL_SwapWindow(Window);
		SDL_Delay(100);
	}
	SDL_DestroyWindow(Window);
	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
}

snake * snakesetup() {
	snake * Snake = (snake *) malloc(sizeof(snake));
	Snake->size = 4;
	Snake->head = (coord *) malloc(sizeof(coord));
	Snake->x_change = 1;
	Snake->y_change = 0;
	Snake->head->x = 32;
	Snake->head->y = 32;
	Snake->head->next = NULL;
	Snake->score = 0;
	return Snake;
}

void update(snake * Snake,coord * food,SDL_Window * window) {
	char extended = 0;
	coord temp1;
	coord temp2;
	coord * snakebody = NULL;
	temp1.x = Snake->head->x;
	temp1.y = Snake->head->y;
	Snake->head->x += Snake->x_change;
	Snake->head->y += Snake->y_change;
	snakebody = Snake->head->next;
	while(snakebody != NULL) {
		if (Snake->head->x == snakebody->x && Snake->head->y == snakebody->y) {
			gameover(Snake,window);
			return;
		}
		snakebody = snakebody->next;
	}
	if(Snake->head->x > 63 || Snake->head->y > 63 ) {
		gameover(Snake,window);
		return;
	}

	if(Snake->head->x == food->x && Snake->head->y == food->y) {
		Snake->size++;
		Snake->score++;
		char notinsnake = 1;
		unsigned char x = rand() % 64;
		unsigned char y = rand() % 64;
		while(1) {
			notinsnake = 1;
			snakebody = Snake->head;
			if(temp1.x == food->x && temp1.y == food->y) {
				notinsnake = 0;
			}
			while(snakebody != NULL) {
				if (x == snakebody->x && y == snakebody->y) {
					notinsnake = 0;
					break;
				}
				snakebody = snakebody->next;
			}
			if(notinsnake) {
				break;
			}
			unsigned char x = rand() % 64;
			unsigned char y = rand() % 64;
		}
		food->x = x;
		food->y = y;
	}
	snakebody = Snake->head->next;
	if (snakebody == NULL) {
		extended = 1;
		Snake->head->next = (coord *) malloc(sizeof(coord));
		snakebody = Snake->head->next;
	}
	for (char i = 1; i < Snake->size; i++) {
		temp2.x = snakebody->x;
		temp2.y = snakebody->y;
		snakebody->x = temp1.x;
		snakebody->y = temp1.y;
		temp1.x = temp2.x;
		temp1.y = temp2.y;
		if(snakebody->next == NULL && i < (Snake->size - 1)) {
			if (!extended) {
				snakebody->next = (coord *) malloc(sizeof(snake));
				extended = 1;
			}
			else {
				break;
			}
		}
		snakebody = snakebody->next;
	}
	if (extended == 0) {
		free(snakebody);
	}
	return;
}

void draw(coord * current, SDL_Window * window, SDL_GLContext glcontext) {
	glColor3d(1,1,1);
	glBegin(GL_QUADS);
	glVertex2d(current->x		,current->y + 1	); //Bottom left
	glVertex2d(current->x + 1	,current->y + 1	); //Bottom Right
	glVertex2d(current->x + 1	,current->y		); //Top Right
	glVertex2d(current->x		,current->y		); //Top Left
	glEnd();
	if (current->next == NULL) {
		return;
	}
	return draw(current->next,window,glcontext);
}

void gameover(snake * player, SDL_Window * window) {
	char gameover[255];
	sprintf(gameover,"Game Over\nYour Score was %d",player->score);
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Game Over",gameover, window);
	running = 0;
}