#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CORRIDOR 0
#define WALL 1
#define EMPTY -1

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
			return type == t;
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
	    	for(int j=0;j<width/2 + 1;j++){
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
					// SIDE CORRIDORS
					cells[i][j].SetType(CORRIDOR);
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

     void FillMap(){
		for(int i=0;i<height - 2;i++){
	    	for(int j=0;j<width/2 + 1;j++){
				if(cells[i][j].GetType() != EMPTY) continue;
				maxTurns = 1;
				cells[i][j].SetType(3);
				cells[i][j].SetActive(true);
				RecursiveWalls(DOWN, i+1, j, maxTurns, 80);				
				RecursiveWalls(RIGHT, i, j+1, maxTurns, 80);
				RecursiveCorridors(i,j);
				printMap();
						
			}
		}
	}

	void RecursiveWalls(int direction, int i, int j, int maxTurns, int wallProb)
	{
		if(maxTurns <= 0 || cells[i][j].GetType() != EMPTY || j > width/2 + 1) return;
		int randNum = GetRand(wallProb);
		int mandatoryPart = MandatoryWall(i,j);
		if (CanBeWall(i,j) && (mandatoryPart > 0 || randNum == WALL))
		{
			if(mandatoryPart > 0)
				cells[i][j].SetType(7);
			else
				cells[i][j].SetType(WALL);
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
				randNum = rand() % 3;
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
								cells[x][y].SetType(WALL);
								RecursiveCorridors(x,y);
							}

					break;
				case 2:
					for(int x = i-1; x<=i; x++)
						for(int y = j; y<=j+1; y++)
							if(CanBeWall(x, y))
							{
								cells[x][y].SetType(WALL);
								RecursiveCorridors(x,y);
							}
					break;
				case 3:
					for(int x = i; x<=i+1; x++)
						for(int y = j-1; y<=j; y++)
							if(CanBeWall(x, y))
							{
								cells[x][y].SetType(WALL);
								RecursiveCorridors(x,y);
							}
					break;
				case 4:
					for(int x = i; x<=i+1; x++)
						for(int y = j-1; y<=j; y++)
							if(CanBeWall(x, y))
							{
								cells[x][y].SetType(WALL);
								RecursiveCorridors(x,y);
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
				if(cells[x][y].GetType() >= WALL && cells[x][j].GetType() <= WALL && !cells[i][y].GetType() <= WALL)
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
					cells[i][j].SetType(WALL);
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
};

int main(int argc, char *argv[])
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
	    w = 41;
	    h = 30;
	   }

    Map *map = new Map(w, h);
    map->initialize();
    map->printMap();

    /*initializeMap(map);
    insertFixedPositions(map);
    fillMap(map);
    doMirrorMap(map);

    printMap(map);*/

    delete map;
}