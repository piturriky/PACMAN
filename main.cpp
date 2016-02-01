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
#include <algorithm>
#include <sstream>
#include <fstream>
#include <vector>
#include <climits>

using namespace std;

#define WINDOW_NAME "PACMAN"

#define WIDTH 600
#define HEIGHT 600

#define ESTANDARD_CELLS_WIDTH 31
#define ESTANDARD_CELLS_HEIGHT 28

#define MIN_CELLS_WIDTH 13
#define MIN_CELLS_HEIGHT 12

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

#define INCREMENT_GHOST_TIMER 3

//TIME TO MOVE
// TODO : #define TIMETOMOVE 500
#define TIMETOMOVE 200

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

#define CALIBRATION_TIME 20

#define CONFIG_FILE "config.cfg"
#define SCORES_FILE "scores.txt"

//Mode Variables
bool DEBUG = false;
bool arduinoActive = false;
bool IAActive = false;
bool RandoomIAActive = false;
bool DirectIAActive = false;

bool modeSummary = false;
bool modeHighScore = false;
bool modeBetweenLevels = false;
bool modeStart = true;

// Game Variables
int level = 0;
int lives = 3;
int numGhosts = 0;
int points = 0, levelPoints = 0;
string playerName = "";
int dificulty = 0;

int lowerScore = 0;

string scores[5];

bool newGame = true;

string betweenLevelsNum = "3";

//VARIABLES
int wallProbDecrease = 20;
int glutWindow;
float cellWidth, cellHeight,cellDepth,radiParticle,radiFood;
struct arduinoProtocol {
	bool state;
  int pacmanDirection, pacmanVision, pacmanVelocity, pacmanAmbient;
} aProtocol;

#include "cell.cpp"
#include "map.cpp"
#include "particle.cpp"
#include "state.cpp"
#include "alphaBeta.cpp"
#include "jpeglib.h"
#include "ArduinoComm.cpp"

Map* map;
Particle* pacman;
vector<Particle*> ghosts;
ArduinoComm *aComm;
long last_t = 0;

int ghostsTimer = 0;
int nextGost = 1;

/*--- Global variables that determine the viewpoint location ---*/
int anglealpha = 0;
int anglebeta = 0;

int realTimeToMove = TIMETOMOVE;
double realAmbientLevel = 1;

void getRandoomWallColor(int *red,int *green,int *blue);
void display();
void keyboardArrows(int key, int x, int y);
void keyboard(unsigned char c,int x,int y);
void idle();
void initializeParticles();
bool CanGo(int x, int y, int direction, bool canGoOut);
int GetGhostDirection(int x, int y, int direction);
int GetGhostRandoomDirection(int x, int y, int num);
int GetGhostDirectionToExit(int x, int y);
void Eat();
void CalculateNewDirections();
void PositionObserver(float alpha,float beta,int radi);
void printCellQuad(int i, int j);
void printGroundMap();
void ReadJPEG(char *filename,unsigned char **image,int *width, int *height);
void LoadTexture(char *filename,int dim);
bool ReadConfigurations();
void insterNewLevel();
void hunted();
bool checkScores();
void finalSummary();
void highScore();
void getScores();
void saveNewScore();
void betweenLevels();
void startScreen();
string BoolToString(bool b);

void fnExit();

int main(int argc, char *argv[]){ // g++ -o pacman pacman->cc -lglut -lGLU -lGL -lm -l jpeg -L /usr/local/lib

	//atexit(fnExit);
	getScores();

	printf("******************************\n");
	printf("*** WELLCOME TO PACMAN BGC ***\n");
	printf("******************************\n\n");

	printf("*** Reading Configurations........\n");
	if(!ReadConfigurations()){
		printf("*** Reading Configurations........ ERROR\n");
		printf("*** CLOSING\n\n");
		return -1;
	}
	printf("*** Reading Configurations....... OK\n\n");

	if(arduinoActive){
		printf("*** Connecting with arduino..........\n");

		aComm = new ArduinoComm();
		if(!aComm->init()){
			printf("*** Connecting with arduino.......... ERROR\n\n");
			printf("*** CLOSING\n\n");
			return -1;
		}

		printf("*** Connecting with arduino.......... OK\n\n");
	
    	printf("*** Calibrating BGC..................\n");
		aComm->startCallibration();

	    for(int x = CALIBRATION_TIME; x >= 0; x--){

			if(x%2 == 0){
				printf("*** Calibrating...\n");
			}else{
				printf("***\n");
			}
	    	usleep(1000000);
	    }

	    aComm->startProcess();
    }else{
    	// Initialize basic params
    	realAmbientLevel = 0.5;
    	//pacman->SetDegreeVisivility(60);
    }

    //insterNewLevel();

    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
	glutInitWindowPosition(50,50);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutWindow = glutCreateWindow(WINDOW_NAME);
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

void fnExit(void){
	printf("\n*** YOU DON'T HAVE MORE LIVES!!!\n\n");
	checkScores();
}

void initializeParticles()
{
	pacman = new Particle(0.9, 0.8, 0.1);
	pacman->SetPosition(map->GetWidth()/2, map->GetHeight()/2 - CENTERBOX/2 - 1); 

	ghosts.clear();

	for(int i = 0; i< numGhosts; i++){
		ghosts.push_back(new Particle(1,0,0));
		ghosts[i]->SetPosition(map->GetWidth()/2, map->GetHeight()/2);
	}   

	ghosts[0]->GoOut();
}

void display(){
	if(modeStart) return startScreen();
	if(modeSummary) return finalSummary();
	if(modeHighScore) return highScore();
	if(modeBetweenLevels) return betweenLevels();

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

	color[0]=realAmbientLevel; color[1]=realAmbientLevel; color[2]=realAmbientLevel; color[3]=realAmbientLevel;
	glLightfv(GL_LIGHT0,GL_AMBIENT,color);

	color[0]=0; color[1]=0; color[2]=0; color[3]=1;
  	glLightfv(GL_LIGHT0,GL_DIFFUSE,color);

	glEnable(GL_LIGHT0);

	material[0]=1; material[1]=1; material[2]=1; material[3]=1.0; 
	glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,material);

	pacman->drawLight(0);
	for(int i = 0; i< numGhosts; i++){
		ghosts[i]->drawLight(i+1);
	}

	glEnable(GL_TEXTURE_2D);
	printGroundMap();

	bool hasFood = false;

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
				hasFood = true;
			}
		}
	}
	glDisable(GL_TEXTURE_2D);
	pacman->draw();
	for(int i = 0; i< numGhosts; i++){
		ghosts[i]->draw();
		if(ghosts[i]->GetX() == pacman->GetX() && ghosts[i]->GetY() == pacman->GetY()){
			hunted();
  		}
	}

	/////////////

	glMatrixMode(GL_PROJECTION);
	  glPushMatrix();
	  glLoadIdentity();
	  gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
	  glMatrixMode(GL_MODELVIEW);
	  glPushMatrix();
	  glLoadIdentity();

	  glRasterPos2i(10, HEIGHT-20);
	  string s = "LEVEL: " +to_string(level);;
	  void * font = GLUT_BITMAP_9_BY_15;
	  for (string::iterator i = s.begin(); i != s.end(); ++i)
	  {
	    char c = *i;
	    glColor3d(1.0, 0.0, 0.0);
	    glutBitmapCharacter(font, c);
	  }


	  glRasterPos2i(10, 10);
	  s = "LIVES: " +to_string(lives);;
	  for (string::iterator i = s.begin(); i != s.end(); ++i)
	  {
	    char c = *i;
	    glColor3d(1.0, 0.0, 0.0);
	    glutBitmapCharacter(font, c);
	  }


	  glRasterPos2i(10, 30);
	  s = "POINTS: " +to_string(points);;
	  for (string::iterator i = s.begin(); i != s.end(); ++i)
	  {
	    char c = *i;
	    glColor3d(1.0, 0.0, 0.0);
	    glutBitmapCharacter(font, c);
	  }

	  glMatrixMode(GL_MODELVIEW);
	  glPopMatrix();
	  glMatrixMode(GL_PROJECTION);
	  glPopMatrix();

	////////////

	glutSwapBuffers();

	if(!hasFood){
		insterNewLevel();
	}
}

void keyboardArrows(int key, int x, int y){

	if(modeHighScore){
		return;
	}

	switch(key)
	{
		case GLUT_KEY_UP:
			if(modeSummary || modeStart){
				newGame = true;
				return;
			}
			pacman->SetNewDirection(UP);
			break;
		case GLUT_KEY_DOWN:
			if(modeSummary || modeStart){
				newGame = false;
				return;
			}
			pacman->SetNewDirection(DOWN);
			break;
		case GLUT_KEY_RIGHT:
			if(modeSummary || modeStart){
				if(dificulty<2){
					dificulty++;
				}
				return;
			}
			pacman->SetNewDirection(RIGHT);
			break;
		case GLUT_KEY_LEFT:
			if(modeSummary || modeStart){
				if(dificulty>0)dificulty--;
				return;
			}
			pacman->SetNewDirection(LEFT);
			break;
	}
}

void keyboard(unsigned char c,int x,int y)
{

	if(modeSummary || modeStart){
		if(c == 13){
			if(newGame){
				lives = 10; //TODO : 3
				level = 2; //TODO : 0
				points = 0;
				numGhosts = 0; //TODO : 0
				modeHighScore = false;
				modeSummary = false;
				modeStart = false;
				if(dificulty == 0){
					IAActive = false;
					RandoomIAActive=true;
					DirectIAActive=false;
				}else if(dificulty == 1){
					IAActive = false;
					RandoomIAActive=false;
					DirectIAActive=true;
				}else{
					IAActive = true;
					RandoomIAActive=false;
					DirectIAActive=false;
				}
				insterNewLevel();
			}else{
				exit(0);
			}
		}
		return;
	}else if(modeHighScore){
		if(c == 8 && playerName.length() > 0){
			playerName = playerName.substr(0, playerName.length()-1);
		}else if((c >= '1' && c <= '9') || (c >= 'A' && c <= 'z') || c == ' '){
			if(playerName.length() > 6) return;
			playerName = playerName.append(1,c);
		}else if(c == 13){
			saveNewScore();
			modeHighScore = false;
			modeSummary = true;
		}
		return;
	}

  int i,j;
  if(c=='1') lives++;
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
		case 4:
			switch(pacman->GetCurrentDirection()){
				case UP:
					pacman->SetNewDirection(DOWN);
					break;
				case DOWN:
					pacman->SetNewDirection(UP);
					break;
				case RIGHT:
					pacman->SetNewDirection(LEFT);
					break;
				case LEFT:
					pacman->SetNewDirection(RIGHT);
					break;
			}
			break;
	}

	//Velocity
	realTimeToMove = TIMETOMOVE / aProtocol.pacmanVelocity;

	//Ambient
	switch(aProtocol.pacmanAmbient){
		case 1:
			realAmbientLevel = 0.9;
			break;
		case 2:
			realAmbientLevel = 0.5;
			break;
		case 3:
			realAmbientLevel = 0;
			break;
		default:
			realAmbientLevel = 1;
			break;
	}
	//Front visivility
	switch(aProtocol.pacmanVision){
		case 1:
			pacman->SetDegreeVisivility(60);
			break;
		case 2:
			pacman->SetDegreeVisivility(10);
			break;
	}
	

}

void idle()
{
	//if(modeSummary) return;

	if(modeBetweenLevels || modeHighScore || modeStart || modeSummary){

	}else{
		long t;	

  t=glutGet(GLUT_ELAPSED_TIME); 

  if(last_t==0)
    last_t=t;
  else
    {
      pacman->Integrate(t-last_t);
      for(int i = 0; i < numGhosts; i++){
      	ghosts[i]->Integrate(t-last_t);
      }
      last_t=t;
    }

	if(aProtocol.state){
		if(DEBUG)printf("Direction: %i, Velocity: %i, Ambient: %i, Vision: %i\n\n", 
			aProtocol.pacmanDirection, aProtocol.pacmanVelocity, aProtocol.pacmanAmbient, aProtocol.pacmanVision);
		setParamsFromArduino();
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
	  	 //cout << "PACMAN NEXT POSITION::::::::::" << pacman->getNextX() << ";" << pacman->getNextY() << "\n";
  	 }
  	 
  }

  // GHOSTS
	  for(int i = 0; i < numGhosts; i++){
	  	if(ghosts[i]->GetState() == QUIET){

	  		// Going out management
	  		// if ghost position = door -> new direction = UP and ghost is out
	  		if(map->GetCell(ghosts[i]->GetX(),ghosts[i]->GetY()).GetType()==UNREACHABLE){
	  			ghosts[i]->SetCurrentDirection(UP);
	  			ghosts[i]->Out();
	  			printf("Out: %i, %i, %i \n",ghosts[i]->GetX(),ghosts[i]->GetY(), ghosts[i]->GetNewDirection());
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
	  			//cout << "OUT!" << ghosts[i]->LastInBox() << "\n";
	  		}
	  		// ghost is out, normal behavior
	  		else{
	  			if (IAActive) CalculateNewDirections();
	  			else if(RandoomIAActive) ghosts[i]->SetNewDirection(GetGhostRandoomDirection(ghosts[i]->GetX(), ghosts[i]->GetY(),i));
	  			else if(DirectIAActive) ghosts[i]->SetNewDirection(GetGhostDirection(ghosts[i]->GetX(), ghosts[i]->GetY(),ghosts[i]->GetCurrentDirection()));
	  			else{
	  				printf("ERROR IA CONFIGURATION\n");
	  				exit(0);
	  			}
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
			  	 		ghosts[i]->InitMovement(ghosts[i]->GetX(), ghosts[i]->GetY() + 1, realTimeToMove + 100);
			  	 		break;
			  	 	case DOWN:
			  	 		ghosts[i]->InitMovement(ghosts[i]->GetX(), ghosts[i]->GetY() - 1, realTimeToMove + 100);
			  	 		break;
			  	 	case RIGHT:
			  	 		ghosts[i]->InitMovement(ghosts[i]->GetX() + 1, ghosts[i]->GetY(), realTimeToMove + 100);
			  	 		break;
			  	 	case LEFT:
			  	 		ghosts[i]->InitMovement(ghosts[i]->GetX() - 1, ghosts[i]->GetY(), realTimeToMove + 100);
			  	 		break;
			  	 }
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

int GetGhostRandoomDirection(int x, int y, int num){
	srand(time(NULL)+num);
	int i = rand() % 4;
	while(!CanGo(x,y,i,false)){
		i = rand() % 4;
	}
	return i;
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
	levelPoints++;
	if(levelPoints % ghostsTimer == 0 && nextGost < numGhosts){
		ghosts[nextGost]->GoOut();
		nextGost++;
		//cout << "NEW OUT\n";
	} 
	//cout << "OUT: " << nextGost << "\n";
}

void CalculateNewDirections(){
	pair<int, int> pacmanPair = make_pair(pacman->getNextX(),pacman->getNextY());
	if(!map->GetCell(pacmanPair.first, pacmanPair.second).IsType(CORRIDOR))
		pacmanPair = make_pair(pacman->GetX(), pacman->GetY());
	vector<pair<int, int> > ghostsList;
	AlphaBeta ab;
	for(int i = 0; i < nextGost; i++){
		if(!ghosts[i]->LastInBox() && !ghosts[i]->CanGoOut()){
			if(map->GetCell(ghosts[i]->getNextX(), ghosts[i]->getNextY()).IsType(CORRIDOR))
				ghostsList.push_back(make_pair(ghosts[i]->getNextX(),ghosts[i]->getNextY()));
			else				
				ghostsList.push_back(make_pair(ghosts[i]->GetX(),ghosts[i]->GetY()));
		}
	}
	//cout << ghostsList.size() << "NUMGHOSTS!!\n";
	State *state = new State(pacmanPair,ghostsList,map,level);
	vector<int> ghostDirections = ab.alphaBetaDesition(*state);
	for(int i = 0; i < nextGost; i++){
		if (!ghosts[i]->LastInBox() && !ghosts[i]->CanGoOut())ghosts[i]->SetNewDirection(ghostDirections[i]);
	}
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

void callback(int v){
	if(v > 0){
		betweenLevelsNum = to_string(v);
		glutTimerFunc(1/*000*/,callback,v - 1); //TODO
	}else if(v == 0){
		betweenLevelsNum = "FIGHT!!!";
		glutTimerFunc(1/*000*/,callback,v - 1);
	}else{
		modeBetweenLevels = false;
	}
	
}

void insterNewLevel(){
	// INITIALATION
    printf("*** Initializing new game................\n");
    level ++;
    levelPoints = 0;
    nextGost = 1;
	modeBetweenLevels = true;
	glutTimerFunc(0,callback,3);
    
    srand(level);
	//srand(time(NULL));
	aProtocol.state = false;

 	int w = MIN_CELLS_WIDTH + 2 * level, h = MIN_CELLS_HEIGHT + 2 * level;

   if(w > 100 && h > 100) wallProbDecrease = 5;
   else if(w > 50 && h > 50) wallProbDecrease = 10; 

    map = new Map(w, h);
    map->initialize();
    //map->printMap();

    printf("numGhosts: %i\n",numGhosts);
    numGhosts = 2;//numGhosts + (level % 2);
    printf("numGhosts: %i\n",numGhosts);

    ghostsTimer = map->GetInitialMeal()/numGhosts/INCREMENT_GHOST_TIMER;
	
	cellWidth = WIDTH/map->GetWidth();
	cellHeight = HEIGHT/map->GetHeight()*(-1);
	cellDepth = (cellWidth-cellHeight)/4;
	radiParticle = (fmin(cellWidth,-cellHeight))/2;
	radiFood =  radiParticle/4;

    initializeParticles();

    anglealpha = 45;
    anglebeta = 60;

    printf("*** Initializing new game................ OK\n\n");
    //modeBetweenLevels = false;
}

bool ReadConfigurations(){
	ifstream if_file(CONFIG_FILE);

	string line;

	while(getline(if_file,line)){
		istringstream is_line(line);

		string key;

		if(getline(is_line,key,'=')){
			string value;
			if(getline(is_line,value)){
				//printf("%s ----> %s\n", key.c_str(),value.c_str());
				if(key == "Debug_active"){
					if(value == "true"){
						DEBUG = true;
						printf("*** Config Debug Mode ---> true\n");
					}else{
						DEBUG = false;
						printf("*** Config Debug Mode ---> false\n");
					}
				}
				if(key == "Arduino_active"){
					if(value == "true"){
						arduinoActive = true;
						printf("*** Config Arduino BGC ---> true\n");
					}else{
						arduinoActive = false;
						printf("*** Config Arduino BGC ---> false\n");
					}
				}
				if(key == "IA_active"){
					if(value == "true"){
						IAActive = true;
						printf("*** Config IA ---> true\n");
					}else{
						IAActive = false;
						printf("*** Config IA ---> false\n");
					}
				}
				if(key == "Randoom_IA_active"){
					if(value == "true"){
						RandoomIAActive = true;
						printf("*** Config Randoom IA ---> true\n");
					}else{
						RandoomIAActive = false;
						printf("*** Config Randoom IA ---> false\n");
					}
				}
				if(key == "Direct_IA_active"){
					if(value == "true"){
						DirectIAActive = true;
						printf("*** Config Direct IA ---> true\n");
					}else{
						DirectIAActive = false;
						printf("*** Config Direct IA ---> false\n");
					}
				}
			}
		}
	}
	return true;
}

bool checkScores(){

	printf("\n*** HIGH SCORES\n");

	ifstream if_file(SCORES_FILE);

	string line;

	int newScores[5];
	string newScoresNames[5];

	bool write = true;

	char name [80];

	for (int i = 0; i < 5; ++i)
	{
		if(getline(if_file,line)){
			istringstream is_line(line);

			string key;

			if(getline(is_line,key,'=')){
				string value;
				if(getline(is_line,value)){
					//printf("%s ----> %s\n", key.c_str(),value.c_str());
					if(write && points >= stoi(key)){
						printf("*** NEW HIGH SCORE POSITION %i\n", i +1);
						printf("*** Enter your name: ");
						scanf ("%79s",name); 

						newScores[i] = points;
						string str(name);
						newScoresNames[i] = str;
						i++;
						write = false;
					}
					if(i < 5){
						newScores[i] = stoi(key);
						newScoresNames[i] = value;
					}
				}
			}
		}else if (write){
			printf("*** NEW HIGH SCORE POSITION %i\n", i +1);
			printf("*** Enter your name: ");
			scanf ("%79s",name); 
			newScores[i] = points;
			string str(name);
			newScoresNames[i] = str;
			write = false;
		}else{
			newScores[i] = 0;
			newScoresNames[i] = "Empty";
		}
	}
	printf("\n");

	ofstream myfile;
	myfile.open (SCORES_FILE);

	for (int i = 0; i < 5; ++i){
		printf("*** POSITION %i: %s --> %i\n", i + 1,newScoresNames[i].c_str(),newScores[i]);
		myfile << newScores[i] << "=" << newScoresNames[i] << "\n";
	}
	myfile.close();
	printf("\n");
}

void saveNewScore(){
	ifstream if_file(SCORES_FILE);

	string line;

	int newScores[5];
	string newScoresNames[5];

	bool write = true;

	char name [80];

	for (int i = 0; i < 5; ++i)
	{
		if(getline(if_file,line)){
			istringstream is_line(line);

			string key;

			if(getline(is_line,key,'=')){
				string value;
				if(getline(is_line,value)){
					//printf("%s ----> %s\n", key.c_str(),value.c_str());
					if(write && points >= stoi(key)){

						newScores[i] = points;
						newScoresNames[i] = playerName;
						scores[i] = playerName + "............. " + to_string(points);
						i++;
						write = false;
					}
					if(i < 5){
						newScores[i] = stoi(key);
						newScoresNames[i] = value;
					}
					scores[i] = value + "............. " + key;
				}
			}
		}else if (write){
			newScores[i] = points;
			newScoresNames[i] = playerName;
			write = false;
			scores[i] = playerName + "............. " + to_string(points);
		}else{
			newScores[i] = 0;
			newScoresNames[i] = "Empty";
		}
	}

	ofstream myfile;
	myfile.open (SCORES_FILE);

	for (int i = 0; i < 5; ++i){
		myfile << newScores[i] << "=" << newScoresNames[i] << "\n";
	}
	myfile.close();
}

void getScores(){
	ifstream if_file(SCORES_FILE);

	string line;

	for (int i = 0; i < 5; ++i)
	{
		if(getline(if_file,line)){
			istringstream is_line(line);
			string key;
			if(getline(is_line,key,'=')){
				string value;
				if(getline(is_line,value)){
					scores[i] = value + "............. " + key; 
					if(i == 4) lowerScore = stoi(key);
				}
			}
		}
	}
}

void hunted(){
	lives --;
	if(lives == 0) {
		//exit(0);
		if(points >= lowerScore){
			modeHighScore = true;
		}else{
			getScores();
			modeSummary = true; 
		}
		return;
	}
	initializeParticles();
}

void startScreen(){
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	/////////////

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	void * font = GLUT_BITMAP_9_BY_15;
	void * font1 = GLUT_BITMAP_TIMES_ROMAN_24;

	int y = HEIGHT/2 +80;
	int x = WIDTH/2-50;
	glRasterPos2i(x, y);
	string s = "PACMAN";
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 0.0, 0.0);
		glutBitmapCharacter(font1, c);
	}


	y = HEIGHT/2 +20;
	x = WIDTH/4;
	glRasterPos2i(x, y);
	s = "Easy";
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 0.0, 0.0);
		if(dificulty==0)glutBitmapCharacter(font1, c);
		else glutBitmapCharacter(font, c);
	}
	x = x + WIDTH/4;
	glRasterPos2i(x, y);
	s = "Medium";
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 0.0, 0.0);
		if(dificulty==1)glutBitmapCharacter(font1, c);
		else glutBitmapCharacter(font, c);
	}
	x = x+WIDTH/4;
	glRasterPos2i(x, y);
	s = "Hard";
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 0.0, 0.0);
		if(dificulty==2)glutBitmapCharacter(font1, c);
		else glutBitmapCharacter(font, c);
	}


	string newG = "";
	string exitG = "";
	if(newGame){
		newG = "---> New Game";
		exitG= "     Exit";
	}else{
		newG = "     New Game";
		exitG= "---> Exit";
	}
	x = WIDTH/2 - 80;
	y = HEIGHT/2 - 40;
	glRasterPos2i(x, y);
	s = newG;
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}
	y = y - 20;
	glRasterPos2i(x, y);
	s = exitG;
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}

	  glMatrixMode(GL_MODELVIEW);
	  glPopMatrix();
	  glMatrixMode(GL_PROJECTION);
	  glPopMatrix();

	////////////
	  glutSwapBuffers();
}

void highScore(){
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	/////////////

	glMatrixMode(GL_PROJECTION);
	  glPushMatrix();
	  glLoadIdentity();
	  gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
	  glMatrixMode(GL_MODELVIEW);
	  glPushMatrix();
	  glLoadIdentity();

	  glRasterPos2i(WIDTH/2 - 60 , HEIGHT/2 + 30);
	  string s = "NEW HIGH SCORE!!!!!";
	  void * font = GLUT_BITMAP_9_BY_15;
	  for (string::iterator i = s.begin(); i != s.end(); ++i)
	  {
	    char c = *i;
	    glColor3d(1.0, 0.0, 0.0);
	    glutBitmapCharacter(font, c);
	  }

	  glRasterPos2i(WIDTH/2 - 60 , HEIGHT/2);
	  s = "Enter your name: " + playerName;
	  for (string::iterator i = s.begin(); i != s.end(); ++i)
	  {
	    char c = *i;
	    glColor3d(1.0, 0.0, 0.0);
	    glutBitmapCharacter(font, c);
	  }

	  glMatrixMode(GL_MODELVIEW);
	  glPopMatrix();
	  glMatrixMode(GL_PROJECTION);
	  glPopMatrix();

	////////////
	  glutSwapBuffers();
}

void finalSummary(){
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	/////////////

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	string s = "";
	void * font = GLUT_BITMAP_9_BY_15;
	void * font1 = GLUT_BITMAP_TIMES_ROMAN_24;

	int x = WIDTH/2 - 80;
	int y = HEIGHT/2 + 150;

	s = "PACMAN HIGH SCORES";
	glRasterPos2i(x, y);
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}

	y = y - 40;
	x = WIDTH/2 - 130;
	for(int i = 0; i< 5; i++){
		glRasterPos2i(x, y);
		s = "Position " + to_string(i +1) + ": "+ scores[i];
		for (string::iterator i = s.begin(); i != s.end(); ++i)
		{
			char c = *i;
			glColor3d(1.0, 0.0, 0.0);
			glutBitmapCharacter(font, c);
		}
		y = y - 20;
	}

	y = y-50;
	x = WIDTH/4;
	glRasterPos2i(x, y);
	s = "Easy";
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 0.0, 0.0);
		if(dificulty==0)glutBitmapCharacter(font1, c);
		else glutBitmapCharacter(font, c);
	}
	x = x + WIDTH/4;
	glRasterPos2i(x, y);
	s = "Medium";
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 0.0, 0.0);
		if(dificulty==1)glutBitmapCharacter(font1, c);
		else glutBitmapCharacter(font, c);
	}
	x = x+WIDTH/4;
	glRasterPos2i(x, y);
	s = "Hard";
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 0.0, 0.0);
		if(dificulty==2)glutBitmapCharacter(font1, c);
		else glutBitmapCharacter(font, c);
	}

	string newG = "";
	string exitG = "";
	if(newGame){
		newG = "---> New Game";
		exitG= "     Exit";
	}else{
		newG = "     New Game";
		exitG= "---> Exit";
	}
	x = WIDTH/2 - 80;
	y = y - 40;
	glRasterPos2i(x, y);
	s = newG;
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}
	y = y - 20;
	glRasterPos2i(x, y);
	s = exitG;
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}

	  glMatrixMode(GL_MODELVIEW);
	  glPopMatrix();
	  glMatrixMode(GL_PROJECTION);
	  glPopMatrix();

	////////////
	  glutSwapBuffers();
}

void betweenLevels(){
	glClearColor(0.0,0.0,0.0,0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	/////////////

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0.0, WIDTH, 0.0, HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glRasterPos2i(WIDTH/2 -20, HEIGHT/2 + 40);
	string s = "LEVEL " + to_string(level);
	void * font = GLUT_BITMAP_9_BY_15;
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}

	glRasterPos2i(WIDTH/2, HEIGHT/2);
	s = betweenLevelsNum;
	for (string::iterator i = s.begin(); i != s.end(); ++i)
	{
		char c = *i;
		glColor3d(1.0, 0.0, 0.0);
		glutBitmapCharacter(font, c);
	}

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	////////////
	glutSwapBuffers();
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

string BoolToString(bool b)
{
  return b ? "true" : "false";
}