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
*
*		- The ghosts will go out of the center box according as the pacman eats a certain amount of food
		- Basic IA
*/


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>
#include <math.h>
#include <iostream>
#include <utility>
#include <list>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <termios.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <thread>
#include <ctime>

using namespace std;

#define IA_VERSION false

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
#define UNREACHABLE 2

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

//GHOST CELL INCREMENT
#define GHOST 0.3

//PARTICLES STATES
#define QUIET 0
#define MOVE 1

//NUMBER OF PARTICLES
#define NUM_GHOST 3

#define INCREMENT_GHOST_TIMER 3

//TIME TO MOVE
#define TIMETOMOVE 500

#define MAX_F 255.0f
#define PI 3.1416

#define Y -50

#define GROUND_R 124
#define GROUND_G 139
#define GROUND_B 141

#define TEXT_BASE 0
#define TEXT_PARED 1
#define TEXT_SOSTRE 2
#define FILE_BASE "base.jpg"
#define FILE_PARED "pared.jpg"
#define FILE_SOSTRE "sostre.jpg"

#define CALIBRATION_TIME 5

//VARIABLES
int wallProbDecrease = 20;
float cellWidth, cellHeight,cellDepth,radiParticle,radiFood;
struct arduinoProtocol {
	bool state;
  int pacmanDirection, pacmanVision, pacmanVelocity, pacmanAmbient;
} aProtocol;

#include "cell.cpp"
#include "map.cpp"
#include "particle.cpp"
#include "state.cpp"
#include "jpeglib.h"
#include "ArduinoComm.cpp"

Map *map;
Particle *pacman;
Particle *ghosts[NUM_GHOST];
ArduinoComm *aComm;
long last_t = 0;

int points = 0;
int ghostsTimer = 0;
int nextGost = 1;

/*--- Global variables that determine the viewpoint location ---*/
int anglealpha = 0;
int anglebeta = 0;

int realTimeToMove = TIMETOMOVE;

void getRandoomWallColor(int *red,int *green,int *blue);
void display();
void keyboardArrows(int key, int x, int y);
void keyboard(unsigned char c,int x,int y);
void idle();
void initializeParticles();
bool CanGo(int x, int y, int direction, bool canGoOut);
int GetGhostDirection(int x, int y, int direction);
int GetGhostDirectionToExit(int x, int y);
void Eat();
void CalculateNewDirections();
void PositionObserver(float alpha,float beta,int radi);
void printCellQuad(int i, int j);
void printGroundMap();
void ReadJPEG(char *filename,unsigned char **image,int *width, int *height);
void LoadTexture(char *filename,int dim);

int main(int argc, char *argv[]){ // g++ -o pacman pacman->cc -lglut -lGLU -lGL -lm -l jpeg -L /usr/local/lib

	aComm = new ArduinoComm();
	aProtocol.state = false;

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
    //map->printMap();

    //printf("%i\n",map->GetInitialMeal());
    ghostsTimer = map->GetInitialMeal()/NUM_GHOST/INCREMENT_GHOST_TIMER;
	
	cellWidth = WIDTH/map->GetWidth();
	cellHeight = HEIGHT/map->GetHeight()*(-1);
	cellDepth = (cellWidth-cellHeight)/4;
	radiParticle = (fmin(cellWidth,-cellHeight))/2;
	radiFood =  radiParticle/4;

	//printf("%f\n", radiParticle);

    initializeParticles();

    anglealpha = 45;
    anglebeta = 60;

    aComm->startCallibration();
    /*if(!aComm->startCallibration()){
    	printf("COMMUNICATION ERROR\n");
    	return 0;
    }*/
    for(int x = CALIBRATION_TIME; x >= 0; x--){
    	switch(x){
    		case CALIBRATION_TIME:
    			printf("WELLCOME TO PACMAN BGC\n");
				break;
			case 3:
				printf("3\n");
				break;
			case 2:
				printf("2\n");
				break;
			case 1:
				printf("1\n");
				break;
			case 0:
				printf("GOOO!\n");
				break;
			default:
				printf("\n");
    	}
    	usleep(1000000);
    }

    aComm->startProcess();

    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
	glutInitWindowPosition(50,50);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutCreateWindow(WINDOW_NAME);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);

	glutDisplayFunc(display);
	glutSpecialFunc(keyboardArrows);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);

	glBindTexture(GL_TEXTURE_2D,TEXT_BASE);
	LoadTexture(FILE_BASE,512);
	glBindTexture(GL_TEXTURE_2D,TEXT_PARED);
	LoadTexture(FILE_PARED,64);
	glBindTexture(GL_TEXTURE_2D,TEXT_SOSTRE);
	LoadTexture(FILE_SOSTRE,64);
	//glMatrixMode(GL_PROJECTION);
	//gluOrtho2D(0,WIDTH-1,0,HEIGHT-1);

	glutMainLoop();

	delete map;
	return 0;
}

void initializeParticles()
{
	pacman = new Particle(0.9, 0.8, 0.1);
	pacman->SetPosition(map->GetWidth()/2, map->GetHeight()/2 - CENTERBOX/2 - 1); 

	for(int i = 0; i< NUM_GHOST; i++){
		ghosts[i] = new Particle(1,0,0);
		ghosts[i]->SetPosition(map->GetWidth()/2, map->GetHeight()/2);
	}   

	ghosts[0]->GoOut();
}

void display(){
	int i,j;

	GLint position[4];
  	GLfloat color[4];
  	GLfloat material[4];

	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	PositionObserver(anglealpha,anglebeta,450);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-WIDTH*0.6,WIDTH*0.6,-HEIGHT*0.6,HEIGHT*0.6,10,2000);

	glMatrixMode(GL_MODELVIEW);

	glPolygonMode(GL_FRONT,GL_FILL);
	glPolygonMode(GL_BACK,GL_LINE);



	//-- Ambient light
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
	position[0]=0; position[1]=0; position[2]=0; position[3]=1; 
	glLightiv(GL_LIGHT0,GL_POSITION,position);

	color[0]=0.1; color[1]=0.1; color[2]=0.1; color[3]=1;
	glLightfv(GL_LIGHT0,GL_AMBIENT,color);

	color[0]=0; color[1]=0; color[2]=0; color[3]=1;
  	glLightfv(GL_LIGHT0,GL_DIFFUSE,color);

	glEnable(GL_LIGHT0);

	material[0]=1; material[1]=1; material[2]=1; material[3]=1.0; 
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,material);

	pacman->drawLight(0);
	for(int i = 0; i< NUM_GHOST; i++){
		ghosts[i]->drawLight(i+1);
	}

	glEnable(GL_TEXTURE_2D);
	printGroundMap();

	for(i=0;i<map->GetWidth();i++){
		for(j=0;j<map->GetHeight();j++){

			if(map->GetCell(i, j).IsType(WALL)){
				material[0]=1; material[1]=1; material[2]=1; material[3]=1.0; 
				glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,material);

				printCellQuad(i,j);

			}
			else if((i < map->GetWidth()/2 - CENTERBOX/2 || i > map->GetWidth()/2 + CENTERBOX/2
				|| j < map->GetHeight()/2 - CENTERBOX/2 || j > map->GetHeight()/2 + CENTERBOX/2) && map->HasFood(i, j))
			{	//OUT OF CENTER BOX
				glDisable(GL_TEXTURE_2D);
				//glColor3f(1.0,1.0,1.0);

				material[0]=1.0; material[1]=1.0; material[2]=1.0; material[3]=1.0; 
  				glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,material);

				GLUquadricObj *quadric;
				quadric = gluNewQuadric();

				gluQuadricDrawStyle(quadric, GLU_FILL);

				glPushMatrix();
				glTranslatef((i+0.5)*cellWidth,Y+radiFood,(j+0.5)*cellHeight);
				gluSphere(quadric,radiFood,36,18);
				glPopMatrix();

				gluDeleteQuadric(quadric);
				
				glEnable(GL_TEXTURE_2D);

				//glVertex2i((i+FOOD_INCREMENT)*cellWidth, (j+FOOD_INCREMENT)*cellHeight);
				//glVertex2i((i+1-FOOD_INCREMENT)*cellWidth, (j+FOOD_INCREMENT)*cellHeight);
				//glVertex2i((i+1-FOOD_INCREMENT)*cellWidth, (j+1-FOOD_INCREMENT)*cellHeight);
				//glVertex2i((i+FOOD_INCREMENT)*cellWidth, (j+1-FOOD_INCREMENT)*cellHeight);

				//glEnd();
			}
		}
	}
	glDisable(GL_TEXTURE_2D);
	pacman->draw();
	for(int i = 0; i< NUM_GHOST; i++){
		ghosts[i]->draw();
	}

	glutSwapBuffers();
}

void keyboardArrows(int key, int x, int y){
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

void keyboard(unsigned char c,int x,int y)
{
  int i,j;

  if (c=='i' && anglebeta<=(90-4))
    anglebeta=(anglebeta+3);
  else if (c=='k' && anglebeta>=(-90+4))
    anglebeta=anglebeta-3;
  else if (c=='j')
    anglealpha=(anglealpha+3)%360;
  else if (c=='l')
    anglealpha=(anglealpha-3+360)%360;

  glutPostRedisplay();
}

void setParamsFromArduino(){
	//Direction
	switch(aProtocol.pacmanDirection)
	{
		case UP:
			pacman->SetNewDirection(UP);
			break;
		case DOWN:
			pacman->SetNewDirection(DOWN);
			break;
		case RIGHT:
			pacman->SetNewDirection(RIGHT);
			break;
		case LEFT:
			pacman->SetNewDirection(LEFT);
			break;
	}

	//Velocity
	realTimeToMove = TIMETOMOVE / aProtocol.pacmanVelocity;

	//Ambient

	//Front visivility
	pacman->SetDegreeVisivility(45/aProtocol.pacmanVision);

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
      for(int i = 0; i < NUM_GHOST; i++){
      	ghosts[i]->Integrate(t-last_t);
      }
      last_t=t;
    }

    if(aProtocol.state){
    	printf("Direction: %i, Velocity: %i, Ambient: %i, Vision: %i\n\n", 
    		aProtocol.pacmanDirection, aProtocol.pacmanVelocity, aProtocol.pacmanAmbient, aProtocol.pacmanVision);

    	aProtocol.state = false;
    }

  // PACMAN
  if(pacman->GetState() == QUIET)
  {
  	if(map->HasFood(pacman->GetX(),pacman->GetY()))Eat();

  	 if(pacman->GetNewDirection() >= 0 && CanGo(pacman->GetX(), pacman->GetY(), pacman->GetNewDirection(),pacman->CanGoOut()))
  	 {
  	 	pacman->SetCurrentDirection(pacman->GetNewDirection());
  	 	pacman->SetNewDirection(-1);
  	 }
  	 if(CanGo(pacman->GetX(), pacman->GetY(), pacman->GetCurrentDirection(),pacman->CanGoOut()))
  	 {
  	 	switch(pacman->GetCurrentDirection())
	  	 {
	  	 	case UP:
	  	 		pacman->InitMovement(pacman->GetX(), pacman->GetY() + 1, realTimeToMove);
	  	 		break;
	  	 	case DOWN:
	  	 		pacman->InitMovement(pacman->GetX(), pacman->GetY() - 1, realTimeToMove);
	  	 		break;
	  	 	case RIGHT:
	  	 		pacman->InitMovement(pacman->GetX() + 1, pacman->GetY(), realTimeToMove);
	  	 		break;
	  	 	case LEFT:
	  	 		pacman->InitMovement(pacman->GetX() - 1, pacman->GetY(), realTimeToMove);
	  	 		break;
	  	 }
  	 }
  	 
  }

  // GHOSTS
	  for(int i = 0; i < NUM_GHOST; i++){
	  	if(ghosts[i]->GetState() == QUIET){

	  		// Going out management
	  		// if ghost position = door -> new direction = UP and ghost is out
	  		if(map->GetCell(ghosts[i]->GetX(),ghosts[i]->GetY()).GetType()==UNREACHABLE){
	  			ghosts[i]->SetCurrentDirection(UP);
	  			ghosts[i]->Out();
	  			//printf("Out: %i, %i, %i \n",ghosts[i]->GetX(),ghosts[i]->GetY(), ghosts[i]->GetNewDirection());
	  		}
	  		// if ghost is in box -> can go out -> get exit way
	  		else if(ghosts[i]->CanGoOut()){
	  			//printf("Going out: %i, %i\t",ghosts[i]->GetX(),ghosts[i]->GetY());
	  			ghosts[i]->SetNewDirection(GetGhostDirectionToExit(ghosts[i]->GetX(), ghosts[i]->GetY()));
	  		}
	  		else if(ghosts[i]->LastInBox()){
	  			ghosts[i]->SetCurrentDirection(-1);
	  			ghosts[i]->SetNewDirection(-1);
	  			ghosts[i]->OutBox();
	  		}else if(IA_VERSION && ghosts[i]->GetNewDirection() < 0){
	  			CalculateNewDirections();
	  		}
	  		// ghost is out, normal behavior
	  		else{
	  			ghosts[i]->SetNewDirection(GetGhostDirection(ghosts[i]->GetX(), ghosts[i]->GetY(),ghosts[i]->GetCurrentDirection()));
	  			//printf("OUT: %i, %i, %i \n",ghosts[i]->GetX(),ghosts[i]->GetY(), ghosts[i]->GetNewDirection());
	  		}

	  		if(ghosts[i]->GetNewDirection() >= 0 && CanGo(ghosts[i]->GetX(), ghosts[i]->GetY(), ghosts[i]->GetNewDirection(),ghosts[i]->CanGoOut()))
		  	 {
		  	 	ghosts[i]->SetCurrentDirection(ghosts[i]->GetNewDirection());
		  	 	ghosts[i]->SetNewDirection(-1);
		  	 }

	  		if(CanGo(ghosts[i]->GetX(), ghosts[i]->GetY(), ghosts[i]->GetCurrentDirection(),ghosts[i]->CanGoOut()))
		  	 {
		  	 	switch(ghosts[i]->GetCurrentDirection())
			  	 {
			  	 	
			  	 	case UP:
			  	 		ghosts[i]->InitMovement(ghosts[i]->GetX(), ghosts[i]->GetY() + 1, realTimeToMove);
			  	 		break;
			  	 	case DOWN:
			  	 		ghosts[i]->InitMovement(ghosts[i]->GetX(), ghosts[i]->GetY() - 1, realTimeToMove);
			  	 		break;
			  	 	case RIGHT:
			  	 		ghosts[i]->InitMovement(ghosts[i]->GetX() + 1, ghosts[i]->GetY(), realTimeToMove);
			  	 		break;
			  	 	case LEFT:
			  	 		ghosts[i]->InitMovement(ghosts[i]->GetX() - 1, ghosts[i]->GetY(), realTimeToMove);
			  	 		break;
			  	 }
		  	 }
	  	}
	  }

	  glutPostRedisplay();
	}

bool CanGo(int x, int y, int direction, bool canGoOut) 
{
	switch(direction)
	{
		case UP:
			//printf("UP %i\n", map->GetCell(x, y + 1).GetType());
			if(canGoOut){
				return map->GetCell(x, y + 1).IsType(CORRIDOR) || map->GetCell(x, y + 1).IsType(UNREACHABLE);
			}
			return map->GetCell(x, y + 1).IsType(CORRIDOR);
		case DOWN:
			//printf("DOWN %i\n", map->GetCell(x, y - 1).GetType());
			return map->GetCell(x, y - 1).IsType(CORRIDOR);
		case RIGHT:
			//printf("RIGHT %i\n", map->GetCell(x + 1, y).GetType());
			return map->GetCell(x + 1, y).IsType(CORRIDOR);
		case LEFT:
			//printf("LEFT %i\n", map->GetCell(x - 1, y).GetType());
			return map->GetCell(x - 1, y).IsType(CORRIDOR);
	}
	return false;
}

int GetGhostDirection(int x, int y, int direction){
	switch(direction){
		case UP:
			if(pacman->GetY() < y){
				return DOWN;
			}

			if(pacman->GetX() > x){
				return RIGHT;
			}else if(pacman->GetX() < x){
				return LEFT;
			}

			return UP;

		case DOWN:
			if(pacman->GetY() > y){
				return UP;
			}

			if(pacman->GetX() > x){
				return RIGHT;
			}else if(pacman->GetX() < x){
				return LEFT;
			}

			return DOWN;

		case LEFT:
			if(pacman->GetX() > x){
				return RIGHT;
			}

			if(pacman->GetY() > y){
				return UP;
			}else if(pacman->GetY() < y){
				return DOWN;
			}

			return LEFT;

		case RIGHT:
			if(pacman->GetX() < x){
				return LEFT;
			}

			if(pacman->GetY() > y){
				return UP;
			}else if(pacman->GetY() < y){
				return DOWN;
			}

			return RIGHT;
		default:
			return LEFT;
	}
}

int GetGhostDirectionToExit(int x, int y){
	if(x>map->GetWidth()/2){
		return LEFT;
	}else if(x < map->GetWidth()/2){
		return RIGHT;
	}
	return UP;
}

void Eat(){
	map->EatFood(pacman->GetX(),pacman->GetY());
	points++;
	if(points % ghostsTimer == 0 && nextGost < NUM_GHOST){
		ghosts[nextGost]->GoOut();
		nextGost++;
	} 
	if(points % 10 == 0)printf("Points: %i\n", points);	
}

void CalculateNewDirections(){
	pair<int, int> pacmanPair = make_pair(pacman->getNextX(),pacman->getNextY());
	list<pair<int, int> > ghostsList;
	for(int i = 0; i < NUM_GHOST; i++){
		if(ghosts[i]->LastInBox())ghostsList.push_back(make_pair(ghosts[i]->getNextX(),ghosts[i]->getNextY()));
	}
	State* state = new State(pacmanPair,ghostsList,map,0);


}

void PositionObserver(float alpha,float beta,int radi)
{
  float x,y,z;
  float upx,upy,upz;
  float modul;

  radi=radi*2;

  x = (float)radi*cos(alpha*2*PI/360.0)*cos(beta*2*PI/360.0);
  y = (float)radi*sin(beta*2*PI/360.0);
  z = (float)radi*sin(alpha*2*PI/360.0)*cos(beta*2*PI/360.0);

  if (beta>0)
    {
      upx=-x;
      upz=-z;
      upy=(x*x+z*z)/y;
    }
  else if(beta==0)
    {
      upx=0;
      upy=1;
      upz=0;
    }
  else
    {
      upx=x;
      upz=z;
      upy=-(x*x+z*z)/y;
    }


  modul=sqrt(upx*upx+upy*upy+upz*upz);

  upx=upx/modul;
  upy=upy/modul;
  upz=upz/modul;

  gluLookAt(x,y,z,WIDTH/2,Y, -HEIGHT/2,upx,upy,upz);
}

void printCellQuad(int i,int j){

	//glColor3f(map->red/MAX_F,map->green/MAX_F,map->blue/MAX_F);
	glBindTexture(GL_TEXTURE_2D,TEXT_PARED);
	glBegin(GL_QUADS);
	glNormal3f(0,0,1);
	glTexCoord2f(0.0,1.0);
	glVertex3i(i*cellWidth, Y,j*cellHeight);
	glTexCoord2f(1.0,1.0);
	glVertex3i((i+1)*cellWidth,Y, j*cellHeight);
	glTexCoord2f(1.0,0.0);
	glVertex3i((i+1)*cellWidth,Y + cellDepth, j*cellHeight);
	glTexCoord2f(0.0,0.0);
	glVertex3i(i*cellWidth,Y + cellDepth, j*cellHeight);

	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(1,0,0);
	glTexCoord2f(0.0,1.0);
	glVertex3i((i+1)*cellWidth,Y, j*cellHeight);
	glTexCoord2f(1.0,1.0);
	glVertex3i((i+1)*cellWidth,Y, (j+1)*cellHeight);
	glTexCoord2f(1.0,0.0);
	glVertex3i((i+1)*cellWidth,Y +cellDepth, (j+1)*cellHeight);
	glTexCoord2f(0.0,0.0);
	glVertex3i((i+1)*cellWidth,Y + cellDepth, j*cellHeight);

	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0,0,-1);
	glTexCoord2f(0.0,1.0);
	glVertex3i((i+1)*cellWidth,Y, (j+1)*cellHeight);
	glTexCoord2f(1.0,1.0);
	glVertex3i(i*cellWidth,Y, (j+1)*cellHeight);
	glTexCoord2f(1.0,0.0);
	glVertex3i(i*cellWidth,Y +cellDepth, (j+1)*cellHeight);
	glTexCoord2f(0.0,0.0);
	glVertex3i((i+1)*cellWidth,Y + cellDepth, (j+1)*cellHeight);

	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(-1,0,0);
	glTexCoord2f(0.0,1.0);
	glVertex3i(i*cellWidth,Y, (j+1)*cellHeight);
	glTexCoord2f(1.0,1.0);
	glVertex3i(i*cellWidth,Y, j*cellHeight);
	glTexCoord2f(1.0,0.0);
	glVertex3i(i*cellWidth,Y +cellDepth, j*cellHeight);
	glTexCoord2f(0.0,0.0);
	glVertex3i(i*cellWidth,Y + cellDepth, (j+1)*cellHeight);

	glEnd();

	//glColor3f(map->red/2/MAX_F,map->green/2/MAX_F,map->blue/2/MAX_F);
	glBindTexture(GL_TEXTURE_2D,TEXT_SOSTRE);
	glBegin(GL_QUADS);
	glNormal3f(0,1,0);
	glTexCoord2f(0.0,1.0);
	glVertex3i(i*cellWidth,Y +cellDepth, j*cellHeight);
	glTexCoord2f(1.0,1.0);
	glVertex3i((i+1)*cellWidth,Y +cellDepth, j*cellHeight);
	glTexCoord2f(1.0,0.0);
	glVertex3i((i+1)*cellWidth,Y +cellDepth, (j+1)*cellHeight);
	glTexCoord2f(0.0,0.0);
	glVertex3i(i*cellWidth,Y + cellDepth, (j+1)*cellHeight);

	glEnd();
}

void printGroundMap(){
	//glColor3f(GROUND_R/MAX_F,GROUND_G/MAX_F,GROUND_B/MAX_F);
	glBindTexture(GL_TEXTURE_2D,TEXT_BASE);
	glBegin(GL_QUADS);

	glNormal3f(0,1,0);
	glTexCoord2f(0.0,2.0);
	glVertex3i(0, Y, 0);
	glTexCoord2f(2.0,2.0);
	glVertex3i(map->GetWidth()*cellWidth, Y, 0);
	glTexCoord2f(2.0,0.0);
	glVertex3i(map->GetWidth()*cellWidth,Y,map->GetHeight()*cellHeight);
	glTexCoord2f(0.0,0.0);
	glVertex3i(0,Y,map->GetHeight()*cellHeight);

	glEnd();
}

void ReadJPEG(char *filename,unsigned char **image,int *width, int *height)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE * infile;
  unsigned char **buffer;
  int i,j;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);


  if ((infile = fopen(filename, "rb")) == NULL) {
    printf("Unable to open file %s\n",filename);
    exit(1);
  }

  jpeg_stdio_src(&cinfo, infile);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_calc_output_dimensions(&cinfo);
  jpeg_start_decompress(&cinfo);

  *width = cinfo.output_width;
  *height  = cinfo.output_height;


  *image=(unsigned char*)malloc(cinfo.output_width*cinfo.output_height*cinfo.output_components);

  buffer=(unsigned char **)malloc(1*sizeof(unsigned char **));
  buffer[0]=(unsigned char *)malloc(cinfo.output_width*cinfo.output_components);


  i=0;
  while (cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines(&cinfo, buffer, 1);

    for(j=0;j<cinfo.output_width*cinfo.output_components;j++)
      {
	(*image)[i]=buffer[0][j];
	i++;
      }   

    }

  free(buffer);
  jpeg_finish_decompress(&cinfo);
} 



/*--------------------------------------------------------*/
/*--------------------------------------------------------*/
void LoadTexture(char *filename,int dim)
{
  unsigned char *buffer;
  unsigned char *buffer2;
  int width,height;
  long i,j;
  long k,h;

  ReadJPEG(filename,&buffer,&width,&height);

  buffer2=(unsigned char*)malloc(dim*dim*3);

  //-- The texture pattern is subsampled so that its dimensions become dim x dim --
  for(i=0;i<dim;i++)
    for(j=0;j<dim;j++)
      {
	k=i*height/dim;
	h=j*width/dim;
	
	buffer2[3*(i*dim+j)]=buffer[3*(k*width +h)];
	buffer2[3*(i*dim+j)+1]=buffer[3*(k*width +h)+1];
	buffer2[3*(i*dim+j)+2]=buffer[3*(k*width +h)+2];

      }

  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
  glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_REPLACE);
  glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,dim,dim,0,GL_RGB,GL_UNSIGNED_BYTE,buffer2);

  free(buffer);
  free(buffer2);
}