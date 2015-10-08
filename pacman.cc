#include <stdio.h>
#include <stdlib.h>

#define CORRIDOR 0
#define WALL 1
#define EMPTY -1

#define CENTERBOX 6 // ALWAYS PAIR

class Cell{
	public:
		Cell(){
			type = EMPTY;
		}
		~Cell(){}
		int GetType(){
			return type;
		}
		void SetType(int t){
			type = t;
		}
	private:
		int type;
};

class Map{
	public:                    // begin public section
      Map(int w, int h){     // constructor
		width = w;
		height = h;

		cells = new Cell* [h];

	    for(int i = 0; i < h; i++){
	    	cells[i] = new Cell[w];
	    }
     }

     ~Map(){}                  // destructor

     int GetWidth(){
		return width;
	 }

     void SetWidth(int w){
		width = w;
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
				if(cells[i][j].GetType() >= 1) printf("0");
				else if(cells[i][j].GetType() == 0)printf("·");
				else if(cells[i][j].GetType() < 0)printf(" ");
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

     Cell **cells;

     void InsertFixedPositions(){
		for(int i=0;i<height;i++){
	    	for(int j=0;j<width/2;j++){
				// MARGIN
				if(i == 0 || i == height-1){
					// FIRST ROW OR LAST ROW
					cells[i][j].SetType(WALL);		
				}else if(j == 0){
					// FIRST COLUMN
					cells[i][j].SetType(WALL);
				}else if((j == width/2 - CENTERBOX/2) && i > height/2 - CENTERBOX/2 && i < height/2 + CENTERBOX/2){
					cells[i][j].SetType(WALL);
				}else if(i == height/2 - CENTERBOX/2 && j == width/2 - 1){
					cells[i][j].SetType(CORRIDOR);
				}else if((i == height/2 - CENTERBOX/2 || i == height/2 + CENTERBOX/2 -1) && j >= width/2 - CENTERBOX/2 && j <= width/2){
					cells[i][j].SetType(WALL);
				}else if(i == height/2 - CENTERBOX/2 - 2 && j >= width/2 - CENTERBOX/2 && j <= width/2){
					cells[i][j].SetType(WALL);
					cells[i+1][j].SetType(CORRIDOR);
				// TOP CORNERS
				}else if(i == 1 && j == 1){
					cells[i][j].SetType(CORRIDOR);
					cells[i][j+1].SetType(CORRIDOR);
					cells[i+1][j].SetType(CORRIDOR);
					cells[i][j+2].SetType(CORRIDOR);
					cells[i+2][j].SetType(CORRIDOR);
					cells[i+1][j+1].SetType(WALL);
				// BOTTOM CORNERS
				}else if(i == height -2 && j == 1){
					cells[i][j].SetType(CORRIDOR);
					cells[i][j+1].SetType(CORRIDOR);
					cells[i-1][j].SetType(CORRIDOR);
					cells[i][j+2].SetType(CORRIDOR);
					cells[i-2][j].SetType(CORRIDOR);
					cells[i-1][j+1].SetType(WALL);
				}
			}
		}
		for(int i = height/2 - CENTERBOX/2 +1; i < height/2 + CENTERBOX/2 - 1; i++){
			for(int j = width/2 - CENTERBOX/2 +1; j < width/2; j++){
				cells[i][j].SetType(CORRIDOR);
			}
		}
     }

     void FillMap(){
		for(int i=0;i<height;i++){
	    	for(int j=0;j<width/2;j++){
				if(cells[i][j].GetType() != EMPTY) continue;
					
				if(i > 1 && cells[i][j-2].GetType() == WALL && cells[i][j-1].GetType() == WALL){
					cells[i][j].SetType(GetRand(70));	
				}else if(i > 1 && cells[i][j-2].GetType() == CORRIDOR && cells[i][j-1].GetType() == CORRIDOR){
					cells[i][j].SetType(GetRand(30));
				}else{
					cells[i][j].SetType(GetRand(50));
				}			
			}
		}
	}
	void DoMirrorMap(){
		for(int i=0;i<height;i++){
	    	for(int j=0;j<width/2+1;j++){
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
	    w = 40;
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