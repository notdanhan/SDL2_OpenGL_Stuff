#include <iostream> //For Printing and stuff
#include <filesystem> //For checking if a file exists
#include <fstream> //For reading/writing to files;
#include <cstdlib>
#include <ctime>

/* SDL stuff */
#include <SDL2/SDL.h>
#include <GL/glu.h>
#include <GL/gl.h>

/*Assert*/
#include <cassert>

/*
 *	Snake.cpp
 *	Author: Daniel Hannon (danielh2942)
 *	Version: 1
 *	Last Edited: 19/05/2021
 *	Brief: I rewrote My previous implementation of Snake in OOP fashion
 *	It uses C++17, SDL2, and Open GL
 *	I might return to this to add sounds
 */

#define TILE_SIZE 10
#define BOARD_X 64
#define BOARD_Y 64

typedef struct Point2D {
	int x;
	int y;
} Point2D;

typedef struct bodypart {
	int xPos;
	int yPos;
	bodypart * next;
	bodypart * prev;
} bodypart;

class Snake {
	public:
		Snake();
		~Snake();
		void start();
		void reset();
		void draw();
		void gameOver();
	private:
		SDL_Window * Window;
		SDL_GLContext glContext;
		bool running;
		bool isPaused;
		bool foodEaten;
		bool gameOverReached;
		int score;
		int highscore;
		Point2D board;
		Point2D food;
		bodypart * snake;
		bodypart * snakeEnd;
		int snakeLength;
		int direction[2];
};

Snake::Snake() {
	//Have the board set to 30 x 30 as a default;
	this->board = {BOARD_X,BOARD_Y};
	this->gameOverReached = false;
	//Move along the X axis :)
	this->direction[1] = 0;
	this->direction[0] = 1;
	this->isPaused = true;
	std::fstream myFile;
	this->running = false;
	this->score = 0;
	std::cout << "Checking for a Highscore file" << std::endl;
	std::filesystem::path f{"score.txt"};
	if(std::filesystem::exists(f)) {
		//File exists so read in highscore
		myFile.open("score.txt",std::ios::in);
		myFile >> this->highscore;
		printf("Highscore: %d\n",this->highscore);
		myFile.close();
	} else {
		//Create a file called score and open in write mode
		myFile.open("score.txt",std::ios::out);
		if(!myFile) {
			std::cout << "Failed to create a Save File!" << std::endl;
		} else {
			myFile << "0";
			myFile.close();
		}
		this->highscore = 0;
	}
	std::cout << "Creating a Snake" << std::endl;
	this->snakeLength = 1;
	this->snake = new bodypart;
	this->snake->xPos = this->board.x/2;
	this->snake->yPos = this->board.y/2;
	this->snake->next = NULL;
	this->snake->prev = NULL;
	this->snakeEnd = this->snake;
	std::cout<<"Seeding Random number"<<std::endl;
	srand(time(NULL));
	std::cout<<"Creating Window"<<std::endl;
	SDL_Init(SDL_INIT_VIDEO);
	this->Window = SDL_CreateWindow("Snake",0,0,this->board.x * TILE_SIZE, this->board.y * TILE_SIZE,SDL_WINDOW_OPENGL);
	assert(Window);
	std::cout<<"Window Exists"<<std::endl;
	this->glContext = SDL_GL_CreateContext(this->Window);
	std::cout<<"Done!"<<std::endl;
}

Snake::~Snake() {
	std::cout<<"Closing Game!"<<std::endl;
	/*Write new Highscore to the document*/
	std::fstream myFile;
	myFile.open("score.txt", std::ios::out);
	if(!myFile) {
		std::cout<<"Could not Write new High score to the file!"<<std::endl;
	} else {
		myFile<<this->highscore;
		myFile.close();
	}
	SDL_DestroyWindow(this->Window);
	SDL_GL_DeleteContext(this->glContext);
	SDL_Quit();
}

void Snake::start() {
	this->running = true;
	this->food = {rand()%this->board.x,rand()%this->board.y};
	bodypart * checker;
	while(this->running) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch(event.type) {
				case SDL_QUIT:
					this->running = false;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym) {
						case SDLK_ESCAPE:
							this->running = false;
							break;
						case SDLK_w:
						case SDLK_UP:
							if(this->direction[1]!=-1) {
								this->direction[1] = 1;
								this->direction[0] = 0;
							}
							break;
						case SDLK_s:
						case SDLK_DOWN:
							if(this->direction[1] != 1) {
								this->direction[1] = -1;
								this->direction[0] = 0;
							}
							break;
						case SDLK_a:
						case SDLK_LEFT:
							if(this->direction[0] != 1) {
								this->direction[1] = 0;
								this->direction[0] = -1;
							}
							break;
						case SDLK_d:
						case SDLK_RIGHT:
							if(this->direction[0] != -1) {
								this->direction[1] = 0;
								this->direction[0] = 1;
							}
							break;
						case SDLK_SPACE:
							//Toggle Play/Pause
							this->isPaused ^= true;
							std::cout<<"Play/Pause toggled"<<std::endl;
							break;
					}
					break;
			}
		}
		if(!isPaused) {
			this->foodEaten = false;
			/*Perform game logic here*/
			Point2D temp = {this->snake->xPos + direction[0], this->snake->yPos + direction[1]};
			if(temp.x < 0 || temp.x > this->board.x || temp.y < 0 || temp.y > this->board.y ) {
				std::cout<<"Board edge reached"<<std::endl;
				this->gameOver();
			} else if (temp.x == this->food.x && temp.y == this->food.y) {
				this->score += 10;
				this->food.x = rand() % this->board.x;
				this->food.y = rand() % this->board.y;
				this->foodEaten = true;
			}
			checker = this->snake;
			while(checker != NULL) {
				if(temp.x == checker->xPos && temp.y == checker->yPos) {
					std::cout<<"Collision"<<std::endl;
					this->gameOver();
				}
				checker = checker->next;
			}
			if(!this->gameOverReached) {
				if(this->snakeLength < 4 || this->foodEaten) {
					bodypart * tempSnake = new bodypart;
					tempSnake->next = this->snake;
					tempSnake->prev = NULL;
					this->snake->prev = tempSnake;
					this->snake = tempSnake;
					this->snakeLength++;
				} else {
					bodypart * part = this->snakeEnd;
					this->snakeEnd = part->prev;
					this->snakeEnd->next = NULL;
					part->prev = NULL;
					part->next = this->snake;
					this->snake->prev = part;
					this->snake = part;
				}
				this->snake->xPos = temp.x;
				this->snake->yPos = temp.y;
			} else {
				this->gameOverReached = false;
			}
		}
		this->draw();
		SDL_Delay(100);
	}
}

void Snake::reset() {
	bodypart * temp = this->snakeEnd->prev;
	do {
		delete temp->next;
		if(temp->prev == NULL) {
			delete temp;
			break;
		} else {
			temp = temp->prev;
		}
	} while(1);
	this->snakeLength = 1;
	this->snake = new bodypart;
	this->snake->xPos = this->board.x/2;
	this->snake->yPos = this->board.y/2;
	this->snake->next = NULL;
	this->snake->prev = NULL;
	this->snakeEnd = this->snake;
	this->score = 0;
	this->direction[0] = 1;
	this->direction[1] = 0;
}

void Snake::draw() {
	glViewport(0,0,this->board.x * TILE_SIZE, this->board.y * TILE_SIZE);
	//R G B Alpha
	glClearColor(0.f,0.f,0.f,0.f);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,this->board.x,0,this->board.y,-1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3d(1,1,1); // White
	bodypart * temp = this->snake;
	while(temp!=NULL) {
		//This has to be done in clockwise or it doesn't work
		glBegin(GL_QUADS);
		glVertex2d(temp->xPos + 1, temp->yPos);
		glVertex2d(temp->xPos, temp->yPos);
		glVertex2d(temp->xPos, temp->yPos + 1);
		glVertex2d(temp->xPos+1,temp->yPos+1);
		glEnd();
		temp = temp->next;
	}
	glColor3d(1,0,0); //Red
	glBegin(GL_QUADS);
	glVertex2d(this->food.x+1,this->food.y);
	glVertex2d(this->food.x,this->food.y);
	glVertex2d(this->food.x,this->food.y+1);
	glVertex2d(this->food.x+1,this->food.y+1);
	glEnd();
	glFlush();
	SDL_GL_SwapWindow(this->Window);
}

void Snake::gameOver() {
	char gameOverString[255];
	if(this->score > this->highscore) {
		sprintf(gameOverString, "Game Over!\nNew High Score: %d", this->score);
		this->highscore = this->score;
	} else {
		sprintf(gameOverString, "Game Over!\n Your Score was %d\nThe Highscore is %d", this->score, this->highscore);
	}
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION,"Game Over",gameOverString,this->Window);
	this->isPaused = true;
	this->gameOverReached = true;
	this->reset();
}

int main() {
	Snake myGame = Snake();
	myGame.start();
	return 0;
}
