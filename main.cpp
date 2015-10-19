/*
*	Lluís Echeverria Rovira
*	Albert Berga Gatius
*	
*
*	Constraints:
*		- Pacman maze size will be allways greater than 21 width and 20 height
*		- Pacman maze width will be allways even
*		- Pacman maze center box will be 7 height and 7 width
*		- If bad size arguments are introduced, standard size will be 31 width and 28 height
*
*	Addeds:
*		- Pacman maze will never have dead ends
*		- Wall's color will be random
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>
#include <math.h>

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
#define UNREACHABLE -3

#define CENTERBOX 7

//DIRECTIONS
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

//PROBABILITIES
#define PROBABILITY_WALL_FIRST_LEVEL 20
#define PROBABILITY_WALL_UNIQUE_COL 50
#define INITIAL_WALL_PROBABILITY 80

//FOOD PAINT CELL INCREMENT
#define FOOD_INCREMENT 0.4

//PARTICLES STATES
#define QUIET 0
#define MOVE 1

//GHOST CELL INCREMENT
#define GHOST 0.3

//NUMBER OF PARTICLES
#define NUM_PARTICLES 5

//TIME TO MOVE
#define TIMETOMOVE 500

//VARIABLES
int wallProbDecrease = 20;
float cellWidth, cellHeight;

#include "cell.cpp"
#include "map.cpp"
#include "particle.cpp"

Map *map;
Particle *pacman;
long last_t = 0;

void getRandoomWallColor(int *red,int *green,int *blue);
void display();
void keyboard(int key, int x, int y);
void idle();
void initializeParticles();
bool CanGo(int x, int y, int direction);


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
	printf("EII");


	cellWidth = WIDTH/map->GetWidth();
	cellHeight = HEIGHT/map->GetHeight();

    initializeParticles();

    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); 
	glutInitWindowPosition(50,50);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutCreateWindow(WINDOW_NAME);

	glutDisplayFunc(display);
	glutSpecialFunc(keyboard);
	glutIdleFunc(idle);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0,WIDTH-1,0,HEIGHT-1);

	glutMainLoop();

	delete map;
	return 0;
}

void initializeParticles()
{
	pacman = new Particle(0.9, 0.8, 0.1);
	pacman->SetPosition(map->GetWidth()/2, map->GetHeight()/2 - CENTERBOX/2 - 1);    
}

void display(){
	int i,j;
	float max = 255.0f;
	float cellWidth, cellHeight;

	cellWidth = WIDTH/map->GetWidth();
	cellHeight = HEIGHT/map->GetHeight();

	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	for(i=0;i<map->GetWidth();i++){
		for(j=0;j<map->GetHeight();j++){
			if(map->GetCell(i, j).IsType(WALL)){/*map->GetHeight() - 1 - */
				glColor3f(map->red/max,map->green/max,map->blue/max);
				glBegin(GL_QUADS);

				glVertex2i(i*cellWidth, j*cellHeight);
				glVertex2i((i+1)*cellWidth, j*cellHeight);
				glVertex2i((i+1)*cellWidth, (j+1)*cellHeight);
				glVertex2i(i*cellWidth, (j+1)*cellHeight);

				glEnd();
			}
			else if((i < map->GetWidth()/2 - CENTERBOX/2 || i > map->GetWidth()/2 + CENTERBOX/2
				|| j < map->GetHeight()/2 - CENTERBOX/2 || j > map->GetHeight()/2 + CENTERBOX/2 - 1) && map->GetCell(i, map->GetHeight() - 1 - j).HasFood())
			{	//OUT OF CENTER BOX
				glColor3f(1.0,1.0,1.0);
				glBegin(GL_QUADS);

				glVertex2i((i+FOOD_INCREMENT)*cellWidth, (j+FOOD_INCREMENT)*cellHeight);
				glVertex2i((i+1-FOOD_INCREMENT)*cellWidth, (j+FOOD_INCREMENT)*cellHeight);
				glVertex2i((i+1-FOOD_INCREMENT)*cellWidth, (j+1-FOOD_INCREMENT)*cellHeight);
				glVertex2i((i+FOOD_INCREMENT)*cellWidth, (j+1-FOOD_INCREMENT)*cellHeight);

				glEnd();
			}
		}
	}
	pacman->draw();

	glutSwapBuffers();
}

void keyboard(int key, int x, int y){
	switch(key)
	{
		case GLUT_KEY_UP:
			pacman->SetNewDirection(UP);
			break;
		case GLUT_KEY_DOWN:
			pacman->SetNewDirection(DOWN);
			break;
		case GLUT_KEY_RIGHT:
			pacman->SetNewDirection(RIGHT);
			break;
		case GLUT_KEY_LEFT:
			pacman->SetNewDirection(LEFT);
			break;

	}
}

void idle()
{
  long t;	


  t=glutGet(GLUT_ELAPSED_TIME); 

  if(last_t==0)
    last_t=t;
  else
    {
      pacman->Integrate(t-last_t);
      last_t=t;
    }
  if(pacman->GetState() == QUIET)
  {
  	 if(pacman->GetNewDirection() >= 0 && CanGo(pacman->GetX(), pacman->GetY(), pacman->GetNewDirection()))
  	 {
  	 	pacman->SetCurrentDirection(pacman->GetNewDirection());
  	 	pacman->SetNewDirection(-1);
  	 }
  	 if(CanGo(pacman->GetX(), pacman->GetY(), pacman->GetCurrentDirection()))
  	 {
  	 	switch(pacman->GetCurrentDirection())
	  	 {
	  	 	case UP:
	  	 		pacman->InitMovement(pacman->GetX(), pacman->GetY() + 1, TIMETOMOVE);
	  	 		break;
	  	 	case DOWN:
	  	 		pacman->InitMovement(pacman->GetX(), pacman->GetY() - 1, TIMETOMOVE);
	  	 		break;
	  	 	case RIGHT:
	  	 		pacman->InitMovement(pacman->GetX() + 1, pacman->GetY(), TIMETOMOVE);
	  	 		break;
	  	 	case LEFT:
	  	 		pacman->InitMovement(pacman->GetX() - 1, pacman->GetY(), TIMETOMOVE);
	  	 		break;
	  	 }
  	 }
  	 
  }


  glutPostRedisplay();
}

bool CanGo(int x, int y, int direction)
{
	switch(direction)
	{
		case UP:
			printf("UP %i\n", map->GetCell(x, y + 1).GetType());
			return map->GetCell(x, y + 1).IsType(CORRIDOR);
		case DOWN:
			printf("DOWN %i\n", map->GetCell(x, y - 1).GetType());
			return map->GetCell(x, y - 1).IsType(CORRIDOR);
		case RIGHT:
			printf("RIGHT %i\n", map->GetCell(x + 1, y).GetType());
			return map->GetCell(x + 1, y).IsType(CORRIDOR);
		case LEFT:
			printf("LEFT %i\n", map->GetCell(x - 1, y).GetType());
			return map->GetCell(x - 1, y).IsType(CORRIDOR);

	}
	return false;
}