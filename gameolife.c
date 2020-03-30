#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
//Game of Life but in C written in 2020 By Daniel Hannon, uses OpenGL and SDL2 I guess :)
//Uses a 64 x 64 board in this variant, might eventually use something else

//CONSTANTS / MACROS
uint64_t display[2][64] = {0};
uint64_t pixel = 1; //Needed for Bitshifts as it rolls over otherwise

//PROTOTYPES
int check_neighbours(int, int);
void update();
void refresh(SDL_Window *);

int main() {
	SDL_Window * window;
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Conways Game of Life",0,0,768,768,SDL_WINDOW_OPENGL);
	assert(window);
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	int running = 1;
	int updating = 0;
	while (running) {
		SDL_Event Event;
		while(SDL_PollEvent(&Event)) {
			switch(Event.type) {
				case SDL_QUIT:
					running = !running;
					break;
				case SDL_KEYDOWN:
					switch (Event.key.keysym.sym) {
						case SDLK_SPACE:
							updating = !updating;
							if(updating) {
								SDL_SetWindowTitle(window,"Game of Life (Running)");
							}
							else {
								SDL_SetWindowTitle(window,"Game of Life (Paused)");
							}
							break;
						case SDLK_ESCAPE:
							running = 0;
							break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					if (Event.button.button = SDL_BUTTON_LEFT) {
						display[0][Event.button.y / 12] = display[0][Event.button.y / 12] ^ pixel<<(~(Event.button.x / 12) & 0x3F);
					} //Toggles pixel at area clicked (Saves Valuable space)
					break;
			}
		}
		if (updating) {
			update();
		}
		glViewport(0,0,768,768);
		glClearColor(0.f,0.f,0.f,0.f);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,64,64,0,-1.0,1.0); //Translates Canvas coordinates so that top left is 0,0 and bottom right is 64,64
		glClear(GL_COLOR_BUFFER_BIT);
		for (int y = 0; y < 64; y++) {
			uint64_t row = display[0][y];
			for (int x = 0; x < 64; x++) {
				if((row >> (~x & 0x3F))&0x1) {
					glColor3d(1,1,1);
					glBegin(GL_QUADS);
					glVertex2d(x		,y + 1	); //Bottom left
					glVertex2d(x + 1	,y + 1	); //Bottom Right
					glVertex2d(x + 1	,y		); //Top Right
					glVertex2d(x		,y		); //Top Left
					glEnd();
				}
			}
		}
		glFlush();
		SDL_GL_SwapWindow(window);
		SDL_Delay(100);
	}
	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
}

int check_neighbours(int x, int y) {
	int total_neighbours = 0;
	if (y != 0) {
		total_neighbours += ((display[0][y-1]>>(~x & 0x3F))&0x1);
		if (x != 0) {
			total_neighbours += ((display[0][y-1]>>(~(x-1) & 0x3F))&0x1);
		}
		if (x != 63) {
			total_neighbours += ((display[0][y-1]>>(~(x+1) & 0x3F))&0x1);
		}
	}
	if (y != 63) {
		total_neighbours += ((display[0][y+1]>>(~x & 0x3F))&0x1);
		if (x != 0) {
			total_neighbours += ((display[0][y+1]>>(~(x-1) & 0x3F))&0x1);
		}
		if (x != 63) {
			total_neighbours += ((display[0][y+1]>>(~(x+1) & 0x3F))&0x1);
		}
	}
	if (x != 0) {
		total_neighbours += ((display[0][y]>>(~(x-1) & 0x3F))&0x1);
	}
	if (x != 63) {
		total_neighbours += ((display[0][y]>>(~(x+1)&0x3F))&0x1);
	}
	return total_neighbours;
}

void update() {
	for (int y = 0; y < 64; y++) {
		for (int x = 0; x < 64; x++) {
			int neighbours = check_neighbours(x,y);
			if ((neighbours == 2 && (display[0][y] >> (~x & 0x3f) & 0x1)) || neighbours == 3) {
				display[1][y] = display[1][y] ^ (pixel << (~x & 0x3F));
			}
		}
	}
	for (char i = 0; i < 64; i++) {
		display[0][i] = display[1][i]; //Swap and clear
		display[1][i] = 0;
	}
}