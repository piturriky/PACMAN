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
				cells[i][j].SetType(WALL);
				cells[i][j].SetActive(true);
				RecursiveWalls(DOWN, i+1, j, maxTurns);				
				RecursiveWalls(RIGHT, i, j+1, maxTurns);
				RecursiveCorridors(i,j);
				//printMap();
						
			}
		}
	}

	void RecursiveWalls(int direction, int i, int j, int maxTurns)
	{
		if(maxTurns <= 0 || cells[i][j].GetType() != EMPTY || j > width/2 + 1) return;
		int randNum = rand() % 2;
		if (randNum == 1 && CanBeWall(i, j))
		{
			cells[i][j].SetType(WALL);
			cells[i][j].SetActive(true);
			switch(direction)
			{
				case UP:
					RecursiveWalls(UP, i-1, j, maxTurns);
					break;
				case DOWN:
					RecursiveWalls(DOWN, i+1, j, maxTurns);
					break;
				case RIGHT:
					RecursiveWalls(RIGHT, i, j+1, maxTurns);
					break;
				case LEFT:
					RecursiveWalls(UP, i, j-1, maxTurns);
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
							RecursiveWalls(LEFT, i, j-1, maxTurns);
						else
							RecursiveWalls(UP, i-1, j, maxTurns);
						break;
					case 1:
						maxTurns--;
						if(direction == UP || direction == DOWN)
							RecursiveWalls(RIGHT, i, j+1, maxTurns);
						else
							RecursiveWalls(DOWN, i+1, j, maxTurns);
						break;
				}
			}
		}
	}

	void RecursiveCorridors(int i, int j)
	{
		for(int x = i-1; x < i+1; x++)
			for(int y = j-1; y < j+1; y++)
			{
				if(x == i && y == j) 
				{
					cells[x][y].SetActive(false);					
					continue;
				}
				if(cells[x][y].GetType() == EMPTY) 
					cells[x][y].SetType(CORRIDOR);
				else if(cells[x][y].GetType() == WALL && cells[x][y].IsActive())
					RecursiveCorridors(x,y);
			}
	}

	bool CanBeWall(int i, int j)
	{
		for(int x = i-1; x < i+1; x++)
			for(int y = j-1; y < j+1; y++)
			{
				if(x == i || y == j) continue;
				if(cells[x][y].IsType(WALL) && !cells[x][y].IsActive())
					return false;
			}
		return true;
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