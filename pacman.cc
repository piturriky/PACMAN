#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <GL/glut.h>

#define WIDTH 300
#define HEIGHT 300

#define CORRIDOR 0
#define WALL 1
#define EMPTY -1
#define MIRROR -2

#define CENTERBOX 7 // ALWAYS PAIR

//DIRECTIONS
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

class Cell{
	public:
		Cell(){
			type = EMPTY;
			activeWall = false;
		}
		~Cell(){}
		bool IsType(int t){
            if(type == t)
                return true;
            else
                return false;
		}
		int GetType(){
			return type;
		}
		void SetType(int t){
			type = t;
		}
		bool IsActive(){
			return activeWall;
		}
		void SetActive(bool a){
			activeWall = a;
		}
	private:
		int type;
		bool activeWall;
};

class Map{
	public:                    // begin public section
      Map(int w, int h){     // constructor
      	width = w;
      	if (w % 2 == 0){
      		width += 1;
      	}

		height = h;

		cells = new Cell* [h];

	    for(int i = 0; i < h; i++){
	    	cells[i] = new Cell[w];
	    }

	    srand(time(NULL));
     }

     ~Map(){}                  // destructor

     int GetWidth(){
		return width;
	 }

     void SetWidth(int w){
     	width = w;
		if (w % 2 == 0){
      		width += 1;
      	}
	 }

     int GetHeight(){
		return height;
	 }

     void SetHeight(int h){
		height = h;
     }

     void printMap(){
    	int i,j;
		for(i=0;i<height;i++){
	    	for(j=0;j<width;j++){
				if(cells[i][j].GetType() >= WALL) printf("%i", cells[i][j].GetType());
				else if(cells[i][j].GetType() == CORRIDOR)printf("·");
				else if(cells[i][j].GetType() == EMPTY)printf(" ");
			}
			printf("\n");
		}
     }

     void initialize(){
     	InsertFixedPositions();
     	printMap();
     	FillMap();
     	DoMirrorMap();
     }

   private:                   // begin private section
     int height;
     int width;
     int maxTurns; /////////////////////////////////////////////////////ARREGLAR

     Cell **cells;

     void InsertFixedPositions(){
		for(int i=0;i<height;i++){
	    	for(int j=0;j<width;j++){
				// MARGIN
				if(i == 0 || i == height-1){
					// FIRST ROW OR LAST ROW
					cells[i][j].SetType(WALL);
				}else if(j == 0){
					// FIRST COLUMN
					cells[i][j].SetType(WALL);
				}else if((j == width/2 - CENTERBOX/2) && i > height/2 - CENTERBOX/2 && i < height/2 + CENTERBOX/2 + 1){
					//LEFT WALL CENTER BOX
					cells[i][j].SetType(WALL);
				}else if(i == height/2 - CENTERBOX/2 && j == width/2){
					//ENTRANCE
					cells[i][j].SetType(CORRIDOR);
				}else if((i == height/2 - CENTERBOX/2 || i == height/2 + CENTERBOX/2) && j >= width/2 - CENTERBOX/2 && j <= width/2){
					//TOP AND BOTTON WALLS
					cells[i][j].SetType(WALL);
				}else if(i == 1 || j == 1 || i == height - 2){
					// SIDES RANDOM
					if(j > 3 && j < width/2+1 && cells[i][j].GetType() == EMPTY)
					{//LEFT SIDE
                        int cellType = GetRand(50);
                        if(cellType == WALL)
                        {
                            cells[i][j].SetType(2);
                            EnvolveCorridors(i,j);
                        }
                        else
                            cells[i][j].SetType(cellType);
                        if(!cells[i][j-1].IsType(CORRIDOR) && cells[i][j].IsType(CORRIDOR))
                        {
                            cells[i][j+1].SetType(CORRIDOR);
                            cells[i][j+2].SetType(CORRIDOR);
                        }

					}else if(i > 3 && i < height-4 && cells[i][j].GetType() == EMPTY)
					{//TOP AND BOTTOM SIDES
                        int cellType = GetRand(50);
                        if(cellType == WALL)
                        {
                            cells[i][j].SetType(2);
                            EnvolveCorridors(i,j);
                        }
                        else
                            cells[i][j].SetType(cellType);
                        if(!cells[i-1][j].IsType(CORRIDOR) && cells[i][j].IsType(CORRIDOR))
                        {
                            cells[i+1][j].SetType(CORRIDOR);
                            cells[i+2][j].SetType(CORRIDOR);
                        }

					}
					else
                        cells[i][j].SetType(CORRIDOR);
				}else if(j == width/2 && (i < height/2 - CENTERBOX/2 - 1 || i > height/2 + CENTERBOX/2 + 1))
				{//UNIQUE COLUMN
					int cellType = GetRand(50);
                    if(cellType == WALL)
                    {
                        cells[i][j].SetType(2);
                        EnvolveCorridors(i,j);
                    }else
                        cells[i][j].SetType(cellType);
				}else if(j > width/2)
				{//MIRROR CELLS
					cells[i][j].SetType(MIRROR);
				}
			}
		}
		for(int i = height/2 - CENTERBOX/2 - 1; i < height/2 + CENTERBOX/2 + 2; i++){
			for(int j = width/2 - CENTERBOX/2 - 1; j < width/2 + 1; j++){
				if(cells[i][j].GetType() == EMPTY)
					cells[i][j].SetType(CORRIDOR);
			}
		}
     }

     void EnvolveCorridors(int i, int j)
     {
        for(int x = i-1; x <= i+1; x++)
			for(int y = j-1; y <= j+1; y++)
                if(cells[x][y].IsType(EMPTY) && x != 1 && x != height - 2 && y != 1)
                    cells[x][y].SetType(CORRIDOR);
     }

     void FillMap(){
		for(int i=0;i<height - 2;i++){
	    	for(int j=0;j<width/2 + 1;j++){
				if(cells[i][j].GetType() != EMPTY) continue;
				maxTurns = 1;
				cells[i][j].SetType(3);
				cells[i][j].SetActive(true);
				//printMap();
				RecursiveWalls(DOWN, i+1, j, maxTurns, 80);
				RecursiveWalls(RIGHT, i, j+1, maxTurns, 80);
				RecursiveCorridors(i,j);
			}
		}
	}

	void RecursiveWalls(int direction, int i, int j, int maxTurns, int wallProb)
	{
		if(maxTurns < 0 || cells[i][j].GetType() != EMPTY || j > width/2 + 1) return;
		int randNum = GetRand(wallProb);
		int mandatoryPart = MandatoryWall(i,j);
		if (CanBeWall(i,j) && (mandatoryPart > 0 || randNum == WALL))
		{
			if(mandatoryPart > 0)
				cells[i][j].SetType(7);
			else
				cells[i][j].SetType(4);
			cells[i][j].SetActive(true);
			switch(direction)
			{
				case UP:
					RecursiveWalls(UP, i-1, j, maxTurns, wallProb - 20);
					break;
				case DOWN:
					RecursiveWalls(DOWN, i+1, j, maxTurns, wallProb - 20);
					break;
				case RIGHT:
					RecursiveWalls(RIGHT, i, j+1, maxTurns, wallProb - 20);
					break;
				case LEFT:
					RecursiveWalls(UP, i, j-1, maxTurns, wallProb - 20);
					break;
			}
			if(maxTurns > 0)
			{
				randNum = rand() % 4;
				switch(randNum)
				{
					case 0:
						maxTurns--;
						if(direction == UP || direction == DOWN)
							RecursiveWalls(LEFT, i, j-1, maxTurns, wallProb - 20);
						else
							RecursiveWalls(UP, i-1, j, maxTurns, wallProb - 20);
						break;
					case 1:
						maxTurns--;
						if(direction == UP || direction == DOWN)
							RecursiveWalls(RIGHT, i, j+1, maxTurns, wallProb - 20);
						else
							RecursiveWalls(DOWN, i+1, j, maxTurns, wallProb - 20);
						break;
				}
			}
		}else if(mandatoryPart > 0)
		{
			CheckMandatoryPosibilities(mandatoryPart, i, j);
		}
	}

	void RecursiveCorridors(int i, int j)
	{
		cells[i][j].SetActive(false);
		for(int x = i-1; x <= i+1; x++)
			for(int y = j-1; y <= j+1; y++)
			{
				if(cells[x][y].GetType() == EMPTY)
				{
					int mandatoryPart = MandatoryWall(x,y);
					if(mandatoryPart > 0)
						if (CanBeWall(x,y))
						{
							cells[x][y].SetType(8);
							RecursiveCorridors(x,y);
						}
						else
						{
							CheckMandatoryPosibilities(mandatoryPart, x, y);
						}
					else
						cells[x][y].SetType(CORRIDOR);
				}
				else if(cells[x][y].GetType() >= WALL && cells[x][y].IsActive())
					RecursiveCorridors(x,y);
				else if(cells[x][y].IsType(CORRIDOR))
				{
					CheckBadEnd(x,y);
				}
			}
	}

	void CheckMandatoryPosibilities(int mandatoryPart, int i, int j)
	{
		switch(mandatoryPart)
			{
				case 1:
					for(int x = i-1; x<=i; x++)
						for(int y = j-1; y<=j; y++)
							if(CanBeWall(x, y))
							{
								cells[x][y].SetType(9);
								RecursiveCorridors(x,y);
								break;
							}
					break;
				case 2:
					for(int x = i-1; x<=i; x++)
						for(int y = j; y<=j+1; y++)
							if(CanBeWall(x, y))
							{
								cells[x][y].SetType(9);
								RecursiveCorridors(x,y);
								break;
							}
					break;
				case 3:
					for(int x = i; x<=i+1; x++)
						for(int y = j-1; y<=j; y++)
							if(CanBeWall(x, y))
							{
								cells[x][y].SetType(9);
								RecursiveCorridors(x,y);
								break;
							}
					break;
				case 4:
					for(int x = i; x<=i+1; x++)
						for(int y = j; y<=j+1; y++)
							if(CanBeWall(x, y))
							{
								cells[x][y].SetType(9);
								RecursiveCorridors(x,y);
								break;
							}
					break;
			}
	}

	bool CanBeWall(int i, int j)
	{
		for(int x = i-1; x <= i+1; x++)
			for(int y = j-1; y <= j+1; y++)
			{
				if(x == i || y == j) continue;
				if(cells[x][y].GetType() >= WALL && cells[x][j].GetType() < WALL && cells[i][y].GetType() < WALL)
					return false;
			}
		return true;
	}

	int MandatoryWall(int i, int j)
	{
		int maxCorridors;
		int part = 0;
		for(int x = i-1; x<=i; x++)
			for(int y = j-1; y<=j; y++)
			{
				maxCorridors = 3;
				part++;
				for(int z = x; z<=x+1; z++)
					for(int k = y; k<=y+1; k++)
						if(cells[z][k].IsType(CORRIDOR))
							maxCorridors--;
				if(maxCorridors < 1)
				{
					return part;
				}
			}
		return 0;

	}

	void CheckBadEnd(int i, int j)
	{
		int wallsCount = 3;
		for(int x = i-1; x <= i+1; x++)
			for(int y = j-1; y <= j+1; y++)
			{
				if((x == i || y == j) && cells[x][y].GetType() >= WALL)
					wallsCount -= 1;
				if(wallsCount == 0)
				{
					cells[i][j].SetType(5);
					//RecursiveCorridors(i,j);
					return;
				}
			}

	}

	void DoMirrorMap()
	{
		for(int i=0;i<height;i++){
	    	for(int j=0;j<width/2;j++){
				cells[i][width - j -1] = cells[i][j];
			}
		}
	}

	int GetRand(int w_wall){
		int res = rand() % 100;
		if(res <= w_wall){
			return WALL;
		}
		return CORRIDOR;
	}

	void display(void){
		int i,j;

		glClearColor(0.0,0.0,0.0,0.0);
		glClear(GL_COLOR_BUFFER_BIT);

		for(i=0;i<WIDTH;i++){
			for(j=0;j<HEIGHT;j++){
				if(cells[i][j].GetType() >= WALL)){
					glColor3f(0.8,0.8,0.8,0.8);
					glBegin(GL_QUADS);

					glVertex21(i*WIDTH/width, j*HEIGHT/height);
					glVertex21((i+1)*WIDTH/width, j*HEIGHT/height);
					glVertex21((i+1)*WIDTH/width, (j+1)*HEIGHT/height);
					glVertex21(i*WIDTH/width, (j+1)*HEIGHT/height);

					glEnd();
				}
			}
		}
		glutSwapBuffers();
	}

	void keyboard(unsigned char c, int x, int y){

	}
};



int main(int argc, char *argv[]) // g++ -lglut -lGLU -lGL -lm pacman.cc
{
 	int w, h;

 	w = h = 0;

	 if(argc == 2)
	   {
	    w = atoi(argv[1]);
	    h = atoi(argv[2]);
	   }

	 if(!w || !h)
	   {
	    w = 31;
	    h = 28;
	   }

    Map *map = new Map(w, h);
    map->initialize();
    map->printMap();

    

    glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB); 
	glutInitWindowPosition(50,50);
	glutInitWindowSize(WIDTH,HEIGHT);
	glutCreateWindow("PAC_MAN");

	glutDisplayFunc(map->display);
	glutKeyboardFunc(map->keyboard);

	glMatrixMode(GL_PROJECTION);
	gluOrtho2D(0,WIDTH-1,0,HEIGHT-1);

	glutMainLoop();

	delete map;
	return 0;
}

