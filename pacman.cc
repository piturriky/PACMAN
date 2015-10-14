#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>

#define WINDOW_NAME "PAC MAN"

#define WIDTH 600
#define HEIGHT 600

#define ESTANDARD_CELLS_WIDTH 31
#define ESTANDARD_CELLS_HEIGHT 28

#define MIN_CELLS_WIDTH 21
#define MIN_CELLS_HEIGHT 20

#define CORRIDOR 0
#define WALL 1
#define EMPTY -1
#define MIRROR -2

#define CENTERBOX 7

//DIRECTIONS
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

//PROBABILITIES
#define PROBABILITY_WALL_FIRST_LEVEL 20
#define PROBABILITY_WALL_UNIQUE_COL 50

//VARIABLES
int wallProbDecrease = 20;

#include "cell.cpp"
#include "map.cpp"

Map *map;

void getRandoomWallColor(int *red,int *green,int *blue);
void display();
void keyboard(unsigned char c, int x, int y);


int main(int argc, char *argv[]){ // g++ -o pacman pacman.cc -lglut -lGLU -lGL -lm
 	int w, h;

 	w = h = 0;

	 if(argc == 3)
	   {
	    w = atoi(argv[1]);
	    h = atoi(argv[2]);

	    if(w < MIN_CELLS_WIDTH || h < MIN_CELLS_HEIGHT || w%2 == 0){
			w = h = 0;
	    }
	   }

	 if(!w || !h)
	   {
	    w = ESTANDARD_CELLS_WIDTH;
	    h = ESTANDARD_CELLS_HEIGHT;
	   }

   if(w > 100 && h > 100) wallProbDecrease = 5;
   else if(w > 50 && h > 50) wallProbDecrease = 10; 

    map = new Map(w, h);
    map->initialize();
    map->printMap();

    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); 
	glutInitWindowPosition(50,50);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutCreateWindow(WINDOW_NAME);

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0,WIDTH-1,0,HEIGHT-1);

	glutMainLoop();

	delete map;
	return 0;
}

void getRandoomWallColor(int *red,int *green,int *blue){

	int randVal = rand() % 200 + 200;
	int max = 255;
	int val;

	switch(rand() % 3){
		case 0:
		{
			*red = 0;
			if(randVal < max) *green = rand() % randVal;
			else *green = rand() % max;
			
			randVal -= *green;

			if(randVal < max) *blue = rand() % randVal;
			else *blue = rand() % max;
			break;
		}
		case 1:
		{
			*green = 0;
			if(randVal < max) *red = rand() % randVal;
			else *red = rand() % max;
			
			randVal -= *red;

			if(randVal < max) *blue = rand() % randVal;
			else *blue = rand() % max;
			break;
		}
		case 2:
		{
			*blue = 0;
			if(randVal < max) *green = rand() % randVal;
			else *green = rand() % max;
			
			randVal -= *green;

			if(randVal < max) *red = rand() % randVal;
			else *red = rand() % max;
			break;
		}
	}
}

void display(){
	int i,j;
	int red, green, blue;
	float max = 255.0f;

	getRandoomWallColor(&red,&green,&blue);

	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	for(i=0;i<map->GetWidth();i++){
		for(j=0;j<map->GetHeight();j++){
			if(map->GetCell(map->GetHeight() - 1 - j,i).IsType(WALL)){
				glColor3f(red/max,green/max,blue/max);
				glBegin(GL_QUADS);

				glVertex2i(i*WIDTH/map->GetWidth(), j*HEIGHT/map->GetHeight());
				glVertex2i((i+1)*WIDTH/map->GetWidth(), j*HEIGHT/map->GetHeight());
				glVertex2i((i+1)*WIDTH/map->GetWidth(), (j+1)*HEIGHT/map->GetHeight());
				glVertex2i(i*WIDTH/map->GetWidth(), (j+1)*HEIGHT/map->GetHeight());

				glEnd();
			}
		}
	}
	glutSwapBuffers();
}

void keyboard(unsigned char c, int x, int y){

}