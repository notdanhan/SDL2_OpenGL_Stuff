#include <stdio.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>
#include <assert.h>

#define sqrt75 0.86602340378

//Draw a Sierpinski triangle
void draw_phase(double x, double y, double side_length, double vdiff, int curr_phase, int max_phase);

int main() {
	int stage = 0;
	int running = 1;
	double side_length = 1.0;
	SDL_Window * window;
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Sierpinski Triangle",0,0,600,600,SDL_WINDOW_OPENGL);
	assert((window != NULL));
	SDL_GLContext glcontext = SDL_GL_CreateContext(window);
	while(running) {
		SDL_Event event;
		while(SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					running = 0;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_SPACE:
							stage++;
							break;
						case SDLK_BACKSPACE:
							stage--;
							break;
					}
			}
		}
		glViewport(0,0,600,600);
		glClearColor(1.f,1.f,1.f,0.f);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0,1.0,0,1.0,-1.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		//Draw Main Triangle
		glBegin(GL_TRIANGLES);
			glColor3d(0,0,0);
			glVertex2f(0,0);
			glVertex2d(1.0,0);
			glVertex2d(0.5,sqrt75);
		glEnd();
		draw_phase(0,0,side_length,sqrt75,0,stage);
		glFlush();
		SDL_GL_SwapWindow(window);
	}
	SDL_DestroyWindow(window);
	SDL_GL_DeleteContext(glcontext);
	SDL_Quit();
}

void draw_phase(double x, double y, double side_length, double vdiff, int curr_phase, int max_phase) {
	if (curr_phase == max_phase || curr_phase > max_phase) {
		return;
	}
	else {
		glBegin(GL_TRIANGLES);
			glColor3d(1,1,1);
			glVertex2f((x+side_length+x)/2,y);
			glVertex2f((x + (side_length/4)),y + (vdiff/2 ));
			glVertex2f((x + (side_length * 0.75)), y + (vdiff/2));
		glEnd();
		vdiff = vdiff/2;
		side_length = side_length/2;
		draw_phase(x,y,side_length,vdiff,curr_phase+1,max_phase); //Left
		draw_phase(x+side_length,y,side_length,vdiff,curr_phase+1,max_phase); //Right
		draw_phase(x+(side_length/2),y + vdiff,side_length,vdiff,curr_phase+1,max_phase); //Top
	}
}
