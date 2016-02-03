class AlphaBeta{
	
	public:
		vector<int> alphaBetaDesition(State state){
			int alpha, beta;
			alpha = INT_MAX;
			beta = INT_MIN;
			MinValue(state, alpha, beta);
			return desitionSuccessors;
		}
	private:

		list< pair<int, int> > visiteds;
		vector<int> desitionSuccessors;
		int MaxValue(State state, int alpha, int beta){
			if(state.isTerminal()) return Utility(state);
			int maxValue = INT_MIN;
				
			for(int i = 0; i<=3; i++){
				if(CanGo(state.mapa, state.pacman.first, state.pacman.second, i) &&
					!isGhostPosition(state, state.pacman.first, state.pacman.second))
				{
					maxValue = std::max(maxValue, MinValue(state.createSuccessorState(i), alpha, beta));
					if (maxValue >= beta){
						return maxValue;	
					} 
					alpha = std::max(alpha, maxValue);
				}
				
			}
			return maxValue;
		}
		
		int MinValue(State state, int alpha, int beta){
			if(state.isTerminal()) return Utility(state);
			int auxMinValue, minValue = INT_MAX;
			int ghostIndex = state.ghosts.size() - 1;
			vector<int> successors;
			for(int i = 0; i<state.ghosts.size(); i++)
				successors.push_back(0);

			while(ghostIndex >= 0){
				bool validSuccessor = true;
				for(int i = 0; i<state.ghosts.size(); i++){
					if(!CanGo(state.mapa, state.ghosts[i].first, state.ghosts[i].second, successors[i])){
						validSuccessor = false;
						break;
					}
				}

				if (validSuccessor){
					auxMinValue = minValue;
					minValue = std::min(minValue, MaxValue(state.createSuccessorState(successors), alpha, beta));
					if(state.currentDepth == 0 && minValue != auxMinValue)
						desitionSuccessors = successors;
					if (minValue >= alpha){
						return minValue;
					}
					beta = std::min(beta, minValue);
				}
				
				prepareNextSuccessor:
					successors[ghostIndex] = (successors[ghostIndex] + 1) % 4;
					if(successors[ghostIndex] == 0){
						ghostIndex--;
						if(ghostIndex >= 0)
							goto prepareNextSuccessor;
					}
					else if(ghostIndex < (state.ghosts.size() - 1)){
						ghostIndex++;
					}

			}
			return minValue;
		}

		int Utility(State state){
			int total = 0;
			list < pair<int,int> >::iterator i;
			for(int i = 0; i<state.ghosts.size(); i++)
				total += BFS(state, i);
			total += state.getPacmanEats();
			return total;
		}

		bool CanGo(Map *mapa, int x, int y, int direction) 
		{
			switch(direction)
			{
				case UP:
					return mapa->GetCell(x, y + 1).IsType(CORRIDOR);
				case DOWN:
					return mapa->GetCell(x, y - 1).IsType(CORRIDOR);
				case RIGHT:
					return mapa->GetCell(x + 1, y).IsType(CORRIDOR);
				case LEFT:
					return mapa->GetCell(x - 1, y).IsType(CORRIDOR);
			}
			return false;
		}

		bool isGhostPosition(State state, int x, int y){
			for(int i = 0; i<state.ghosts.size(); i++)
				if(state.ghosts[i].first == x && state.ghosts[i].second == y)
					return true;
			return false;
		}
		int BFS(State state, int ghost){
			list< pair <int, int> > bfsQueue;
			
			bool **visited = new bool*[state.mapa->GetHeight()];
			for(int i=0; i < state.mapa->GetHeight();i++){
				visited[i] = new bool[state.mapa->GetWidth()];
				for(int j = 0; j<state.mapa->GetWidth(); j++){
					visited[i][j] = false;
				}
			}

			pair<int, int> auxPair,auxPair2;
			int treeLevel = 1;
			auxPair = state.ghosts[ghost];
			visited[auxPair.first][auxPair.second]=true;

			if(auxPair.first == state.pacman.first && auxPair.second == state.pacman.second)
				return INT_MIN;
			for(int i = 0; i <= 3; i++)
			{
				if(CanGo(state.mapa, auxPair.first, auxPair.second, i)){
					auxPair2 = getNextPosition(auxPair, i);
					bfsQueue.push_back(auxPair2);
					visited[auxPair2.first][auxPair2.second]=true;
				}
			}
			bfsQueue.push_back(make_pair(0,0));

			while(bfsQueue.size() > 0)
			{
				auxPair = bfsQueue.front();
				if(auxPair == make_pair(0,0))
				{
					if(bfsQueue.size() == 1)return treeLevel;
					treeLevel++;
					bfsQueue.push_back(make_pair(0,0));
				}else if(auxPair.first == state.pacman.first && auxPair.second == state.pacman.second){
					return treeLevel;
				}
				else
				{	
					for(int i = 0; i <= 3; i++)
					{
						auxPair2 = getNextPosition(auxPair, i);
						if(CanGo(state.mapa, auxPair.first, auxPair.second, i) &&
							!visited[auxPair2.first][auxPair2.second]){
							bfsQueue.push_back(auxPair2);
							visited[auxPair2.first][auxPair2.second]=true;
						}
							
					}
				}
				bfsQueue.pop_front();
			}
			return treeLevel;
		}

		pair<int, int>  getNextPosition(pair<int,int> position, int dir){
			switch(dir)
			  	 {			  	 	
			  	 	case UP:
			  	 		return make_pair(position.first, position.second + 1);
			  	 	case DOWN:
			  	 		return make_pair(position.first, position.second - 1);
			  	 	case RIGHT:
			  	 		return make_pair(position.first + 1, position.second);
			  	 	case LEFT:
			  	 		return make_pair(position.first - 1, position.second);
			  	 }
		}
};
	