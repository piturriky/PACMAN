class AlphaBeta{
	
	public:
		vector<int> alphaBetaDesition(State state){
			MinValue(state, INT_MIN, INT_MAX);
		}
	private:
		list< pair<int, int> > visiteds;
		int MaxValue(State state, int alpha, int beta){
			if(state.isTerminal()) return Utility(state);
			int maxValue = alpha;
			int ghostIndex = 0;
			vector<int> successors;
			for(int i = 0; i<state.ghosts.size(); i++)
				successors.push_back(0);

			while(ghostIndex >= 0){
				for(int i = 0; i<state.ghosts.size(); i++){
					bool validSuccessor = true;
					for(int i = 0; i<state.ghosts.size(); i++){
						if(!CanGo(state.map, state.ghosts[i].first, state.ghosts[i].second, successors[i])){
							validSuccessor = false;
							break;
						}
					}
					if (validSuccessor){
						maxValue = std::max(maxValue, MinValue(state.createSuccessorState(successors), alpha, beta));
					}
				}
			}
			return 0;
		}

		int MinValue(State state, int alpha, int beta){
			if(state.isTerminal()) return Utility(state);
			return 0;
		}

		int Utility(State state){
			int total = 0;
			list < pair<int,int> >::iterator i;
			for(int i = 0; i<state.ghosts.size(); i++)
				total += abs(state.ghosts[i].first - state.pacman.first) + abs(state.ghosts[i].second - state.pacman.second); 
			return total;
		}

		bool CanGo(Map *map, int x, int y, int direction) 
		{
			switch(direction)
			{
				case UP:
					//printf("UP %i\n", map->GetCell(x, y + 1).GetType());
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

		/*int Utility_(State state){
			int dist = 0;
			list<pair<int,int> >::iterator i;
			//afegir posicions actuals de pacmans a visiteds
			for(i = state.ghosts.begin(); i!=state.ghosts.end(); i++){

			}
		}

		list< pair<int, int> > getNextPositions(int x, int y, State state){
			list< pair<int, int> > nextPositions;
			for(int i = x-1; i<= x+1; i++)
				for(int j = y-1; j<= y+1; j++){
					if((i == x || j == y) && state.map->GetCell(x, y).IsType(CORRIDOR))
						nextPositions.push_front(make_pair(i,j));
				}
			return nextPositions;
		}

		int BFS(int x, int y, State state){
			list< list < pair<int, int> > > bfsQueue;
			list< pair<int, int> > auxList;
			list< pair<int, int> > newQueue;
			pair<int, int> auxPair;

			auxList = getNextPositions(x, y, state);
			while(!auxList.empty()){
				newQueue.clear();
				newQueue.push_front(auxList.front());
				auxList.pop_front();
				bfsQueue.push_front(newQueue);
			}
			
			while(!bfsQueue.empty()){
				auxPair = bfsQueue.back().front();
				auxList = getNextPositions(auxPair.first, auxPair.second, state);
				while(!auxList.empty()){
					auxPair = auxList.front();
					auxList.pop_front();
					if(auxPair.first == state.pacman.first && auxPair.second == state.pacman.second){
						newQueue.clear();
						newQueue = bfsQueue.back();
						newQueue.push_front(auxPair);
						goto finalize;
					}
					else if(find(visiteds.begin(), visiteds.end(), auxPair) == visiteds.end()){
						newQueue.clear();
						newQueue = bfsQueue.back();
						newQueue.push_front(auxPair);
						bfsQueue.push_front(newQueue);
					}
				}
				bfsQueue.pop_back();
			}
			finalize:
			visiteds.splice(visiteds.begin(), newQueue);
			//marcar posicions de result i determinar cap on anira el fantasma
		}*/
};
	