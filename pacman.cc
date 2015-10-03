#include <stdio.h>
#include <stdlib.h>

#define CORRIDOR 0
#define WALL 1
#define EMPTY -1

#define CENTERBOX 6 // ALWAYS PAIR


typedef struct MAP
{
 unsigned short int w, h;  /* Width and height of the map */
 int **tile;      /* Pointer to the array of tiles */
} MAP;



static MAP *create_map(int screen_w, int screen_h){
	
	MAP *temp;

 	temp = (MAP *) malloc(sizeof(MAP));

 	temp->w = screen_w;
    temp->h = screen_h;

    //temp->tile = (unsigned char *) malloc(sizeof(unsigned char) * temp->w * temp->h);
    temp->tile = new int* [temp->h];

    for(int i = 0; i < temp->h; i++){
    	temp->tile[i] = new int[temp->w];
    }

    return temp;
}

void initializeMap(MAP *map){
	int i,j;
	for(i=0;i<map->h;i++){
    	for(j=0;j<map->w;j++){
    		map->tile[i][j] = EMPTY;
		}
	}
}

void insertFixedPositions(MAP *map){
	int i,j;
	for(i=0;i<map->h;i++){
    	for(j=0;j<map->w/2;j++){
			// MARGIN
			if(i == 0 || i == map->h-1){
				// FIRST ROW OR LAST ROW
				map->tile[i][j] = WALL;		
			}else if(j == 0){
				// FIRST COLUMN
				map->tile[i][j] = WALL;
			}else if((j == map->w/2 - CENTERBOX/2) && i > map->h/2 - CENTERBOX/2 && i < map->h/2 + CENTERBOX/2){
				map->tile[i][j] = WALL;
			}else if(i == map->h/2 - CENTERBOX/2 && j == map->w/2 - 1){
				map->tile[i][j] = CORRIDOR;
			}else if((i == map->h/2 - CENTERBOX/2 || i == map->h/2 + CENTERBOX/2 -1) && j >= map->w/2 - CENTERBOX/2 && j <= map->w/2){
				map->tile[i][j] = WALL;
			}else if(i == map->h/2 - CENTERBOX/2 - 2 && j >= map->w/2 - CENTERBOX/2 && j <= map->w/2){
				map->tile[i][j] = WALL;
				map->tile[i+1][j] = CORRIDOR;
			// TOP CORNERS
			}else if(i == 1 && j == 1){
				map->tile[i][j] = CORRIDOR;
				map->tile[i][j+1] = CORRIDOR;
				map->tile[i+1][j] = CORRIDOR;
				map->tile[i][j+2] = CORRIDOR;
				map->tile[i+2][j] = CORRIDOR;
				map->tile[i+1][j+1] = WALL;
			// BOTTOM CORNERS
			}else if(i == map->h -2 && j == 1){
				map->tile[i][j] = CORRIDOR;
				map->tile[i][j+1] = CORRIDOR;
				map->tile[i-1][j] = CORRIDOR;
				map->tile[i][j+2] = CORRIDOR;
				map->tile[i-2][j] = CORRIDOR;
				map->tile[i-1][j+1] = WALL;
			}
		}
	}
	for(i = map->h/2 - CENTERBOX/2 +1; i < map->h/2 + CENTERBOX/2 -1; i++){
		for(j = map->w/2 - CENTERBOX/2 +1; j < map->w/2; j++){
			map->tile[i][j] = CORRIDOR;
		}
	}
}

int getRand(int w_wall){
	int res = rand() % 100;
	if(res <= w_wall){
		return WALL;
	}
	return CORRIDOR;
}

void fillMap(MAP *map){
	int i,j;

	for(i=0;i<map->h;i++){
    	for(j=0;j<map->w/2;j++){
			if(map->tile[i][j] != EMPTY) continue;
				
			if(i > 1 && map->tile[i][j-2] == WALL && map->tile[i][j-1] == WALL){
				map->tile[i][j] = getRand(70);	
			}else if(i > 1 && map->tile[i][j-2] == CORRIDOR && map->tile[i][j-1] == CORRIDOR){
				map->tile[i][j] = getRand(30);
			}else{
				map->tile[i][j] = getRand(50);
			}			
		}
	}
}

void doMirrorMap(MAP *map){
	int i,j;
	for(i=0;i<map->h;i++){
    	for(j=0;j<map->w/2+1;j++){
			map->tile[i][map->w - j -1] = map->tile[i][j];
		}
	}
}

void printMap(MAP *map){
	int i,j;
	for(i=0;i<map->h;i++){
    	for(j=0;j<map->w;j++){
			if(map->tile[i][j] >= 1) printf("0");
			else if(map->tile[i][j] == 0)printf("·");
			else if(map->tile[i][j] < 0)printf(" ");
		}
		printf("\n");	
	}
}


int main(int argc, char *argv[])
{
	
	MAP *map;
 
 	int w, h;

 	w = h = 0;

	 if(argc == 2)
	   {
	    w = atoi(argv[1]);
	    h = atoi(argv[2]);
	   }

	 if(!w || !h)
	   {
	    w = 20;
	    h = 20;
	   }

    map = create_map(w, h);
    initializeMap(map);
    insertFixedPositions(map);
    fillMap(map);
    doMirrorMap(map);

    printMap(map);

}