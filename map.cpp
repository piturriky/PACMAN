class Map{
	public:

	  int red, green, blue;//maze color

      Map(int w, int h){
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
	    getRandoomWallColor();
     }

     ~Map(){}

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

     Cell GetCell(int x, int y){
     	return cells[y][x];
     }

     void printMap(){
    	int i,j;
		for(i=0;i<height;i++){
	    	for(j=0;j<width;j++){
				if(cells[i][j].GetType()>=WALL) printf("%i", cells[i][j].GetType());
				else if(cells[i][j].IsType(CORRIDOR))printf("·");
				else if(cells[i][j].IsType(EMPTY))printf(" ");
			}
			printf("\n");
		}
     }

     void initialize(){
     	InsertFixedPositions();
     	FillMap();
     	DoMirrorMap();
     }

     void EatFood(int x, int y){
     	cells[y][x].EatFood();
     }

     bool HasFood(int x, int y){
     	return cells[y][x].HasFood();
     }

   private:
     int height;
     int width;
     int maxTurns;

     Cell **cells;

	 void getRandoomWallColor(){

		int randVal = rand() % 200 + 200;
		int max = 255;
		int val;

		switch(rand() % 3){
			case 0:
			{
				red = 0;
				if(randVal < max) green = rand() % randVal;
				else green = rand() % max;
				
				randVal -= green;

				if(randVal < max) blue = rand() % randVal;
				else blue = rand() % max;
				break;
			}
			case 1:
			{
				green = 0;
				if(randVal < max) red = rand() % randVal;
				else red = rand() % max;
				
				randVal -= red;

				if(randVal < max) blue = rand() % randVal;
				else blue = rand() % max;
				break;
			}
			case 2:
			{
				blue = 0;
				if(randVal < max) green = rand() % randVal;
				else green = rand() % max;
				
				randVal -= green;

				if(randVal < max) red = rand() % randVal;
				else red = rand() % max;
				break;
			}
		}
	}

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
				}else if(i == height/2 + CENTERBOX/2 && j == width/2){
					//ENTRANCE
					cells[i][j].SetType(UNREACHABLE);
				}else if((i == height/2 - CENTERBOX/2 || i == height/2 + CENTERBOX/2) && j >= width/2 - CENTERBOX/2 && j <= width/2){
					//TOP AND BOTTON WALLS
					cells[i][j].SetType(WALL);
				}else if(i == 1 || j == 1 || i == height - 2){
					// SIDES RANDOM
					if(j > 3 && j < width/2+1 && cells[i][j].IsType(EMPTY))
					{//TOP AND BOTTOM SIDEA
                        int cellType = GetRand(PROBABILITY_WALL_FIRST_LEVEL);
                        if(cellType == WALL)
                        {
                            cells[i][j].SetType(WALL);
                            EnvolveCorridors(i,j);
                        }
                        else
                            cells[i][j].SetType(cellType);
                        if(!cells[i][j-1].IsType(CORRIDOR) && cells[i][j].IsType(CORRIDOR))
                        {
                            cells[i][j+1].SetType(CORRIDOR);
                            cells[i][j+2].SetType(CORRIDOR);
                        }
                        if(j == width/2 && cells[i][j-1].IsType(WALL))
                        	cells[i][j].SetType(WALL);

					}else if(i > 3 && i < height-4 && cells[i][j].IsType(EMPTY))
					{//LEFT SIDE
                        int cellType = GetRand(PROBABILITY_WALL_FIRST_LEVEL);
                        if(cellType == WALL)
                        {
                        	if(CanBeWall(i,j))
                        	{
                            	cells[i][j].SetType(WALL);
                            	EnvolveCorridors(i,j);
                        	}
                            else
                            	cells[i][j].SetType(CORRIDOR);
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
					int cellType = GetRand(PROBABILITY_WALL_UNIQUE_COL);
                    if(cellType == WALL)
                    {
                    	if(CanBeWall(i,j))
                    	{
                        	cells[i][j].SetType(WALL);
                        	EnvolveCorridors(i,j);
                    	}
                        else
                        	cells[i][j].SetType(CORRIDOR);
                    }else
                        cells[i][j].SetType(cellType);
				}else if(j == width/2 + 1)
				{
					cells[i][j].SetType(WALL);
				}
				else if(j > width/2)
				{//MIRROR CELLS
					cells[i][j].SetType(MIRROR);
				}
			}
		}
		for(int i = height/2 - CENTERBOX/2 - 1; i < height/2 + CENTERBOX/2 + 2; i++){
			for(int j = width/2 - CENTERBOX/2 - 1; j < width/2 + 1; j++){
				if(cells[i][j].IsType(EMPTY) && (i == height/2 - CENTERBOX/2 - 1 || i == height/2 + CENTERBOX/2 + 1
												|| j == width/2 - CENTERBOX/2 - 1))
					cells[i][j].SetType(CORRIDOR);
				else if(cells[i][j].IsType(EMPTY))
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
				if(!cells[i][j].IsType(EMPTY)) continue;
				maxTurns = 1;
				cells[i][j].SetType(WALL);
				cells[i][j].SetActive(true);
				RecursiveWalls(DOWN, i+1, j, maxTurns, INITIAL_WALL_PROBABILITY);
				RecursiveWalls(RIGHT, i, j+1, maxTurns, INITIAL_WALL_PROBABILITY);
				RecursiveCorridors(i,j);
			}
		}
	}

	void RecursiveWalls(int direction, int i, int j, int maxTurns, int wallProb)
	{
		if(maxTurns < 0 || !cells[i][j].IsType(EMPTY) || j > width/2 + 1) return;
		int randNum = GetRand(wallProb);
		int mandatoryPart = MandatoryWall(i,j);
		if (CanBeWall(i,j) && (mandatoryPart > 0 || randNum == WALL))
		{
			if(mandatoryPart > 0)
				cells[i][j].SetType(WALL);
			else
				cells[i][j].SetType(WALL);
			cells[i][j].SetActive(true);
			switch(direction)
			{
				case UP:
					RecursiveWalls(UP, i-1, j, maxTurns, wallProb - wallProbDecrease);
					break;
				case DOWN:
					RecursiveWalls(DOWN, i+1, j, maxTurns, wallProb - wallProbDecrease);
					break;
				case RIGHT:
					RecursiveWalls(RIGHT, i, j+1, maxTurns, wallProb - wallProbDecrease);
					break;
				case LEFT:
					RecursiveWalls(UP, i, j-1, maxTurns, wallProb - wallProbDecrease);
					break;
			}
			if(maxTurns > 0)
			{
				randNum = rand() % 4; //50% probability to turn
				switch(randNum)
				{
					case 0:
						maxTurns--;
						if(direction == UP || direction == DOWN)
							RecursiveWalls(LEFT, i, j-1, maxTurns, wallProb - wallProbDecrease);
						else
							RecursiveWalls(UP, i-1, j, maxTurns, wallProb - wallProbDecrease);
						break;
					case 1:
						maxTurns--;
						if(direction == UP || direction == DOWN)
							RecursiveWalls(RIGHT, i, j+1, maxTurns, wallProb - wallProbDecrease);
						else
							RecursiveWalls(DOWN, i+1, j, maxTurns, wallProb - wallProbDecrease);
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
				if(cells[x][y].IsType(EMPTY))
				{
					int mandatoryPart = MandatoryWall(x,y);
					if(mandatoryPart > 0)
						if (CanBeWall(x,y))
						{
							cells[x][y].SetType(WALL);
							RecursiveCorridors(x,y);
						}
						else
						{
							CheckMandatoryPosibilities(mandatoryPart, x, y);
						}
					else
						cells[x][y].SetType(CORRIDOR);
				}
				else if(cells[x][y].IsType(WALL) && cells[x][y].IsActive())
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
								return;
							}
					break;
				case 2:
					for(int x = i-1; x<=i; x++)
						for(int y = j; y<=j+1; y++)
							if(CanBeWall(x, y))
							{
								cells[x][y].SetType(WALL);
								RecursiveCorridors(x,y);
								return;
							}
					break;
				case 3:
					for(int x = i; x<=i+1; x++)
						for(int y = j-1; y<=j; y++)
							if(CanBeWall(x, y))
							{
								cells[x][y].SetType(WALL);
								RecursiveCorridors(x,y);
								return;
							}
					break;
				case 4:
					for(int x = i; x<=i+1; x++)
						for(int y = j; y<=j+1; y++)
							if(CanBeWall(x, y))
							{
								cells[x][y].SetType(WALL);
								RecursiveCorridors(x,y);
								return;
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
				if(cells[x][y].IsType(WALL) && !cells[x][j].IsType(WALL) && !cells[i][y].IsType(WALL))
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
				if((x == i || y == j) && cells[x][y].IsType(WALL))
					wallsCount -= 1;
				if(wallsCount == 0)
				{
					cells[i][j].SetType(WALL);
					RecursiveCorridors(i,j);
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