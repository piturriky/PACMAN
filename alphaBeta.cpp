class AlphaBeta{
	
	public:
		vector<int> alphaBetaDesition(State state){
			//cout << "Width: " << state.pacman.first << "  Height: " << state.pacman.second << "\n"; 
			int alpha, beta;
			alpha = INT_MAX;
			beta = INT_MIN;
			cout << "A\n";
			MinValue(state, &alpha, &beta);
			return desitionSuccessors;
		}
	private:
		struct astarResult
		{
			int heuristicValue;
			vector< pair<int, int> > eatedPositions;			
		};

		list< pair<int, int> > visiteds;
		vector<int> desitionSuccessors;
		int MaxValue(State state, int *alpha, int *beta){
			if(state.isTerminal()) return Utility(state);
			int maxValue = INT_MIN;
			int ghostIndex = state.ghosts.size() - 1;
			vector<int> successors;
			for(int i = 0; i<state.ghosts.size(); i++)
				successors.push_back(0);
				
			for(int i = 0; i<=3; i++){
				//cout << state.pacman.first << ";" << state.pacman.second << "\n";
				if(CanGo(state.map, state.pacman.first, state.pacman.second, i) &&
					!isGhostPosition(state, state.pacman.first, state.pacman.second))
				{
					maxValue = std::max(maxValue, MinValue(state.createSuccessorState(i), alpha, beta));
					if (maxValue >= *beta) return maxValue;
					*alpha = std::max(*alpha, maxValue);
				}
				
			}
			return maxValue;
		}

		int MinValue(State state, int *alpha, int *beta){
			if(state.isTerminal()) return Utility(state);
			int auxMinValue, minValue = INT_MAX;
			int ghostIndex = state.ghosts.size() - 1;
			vector<int> successors;
			for(int i = 0; i<state.ghosts.size(); i++)
				successors.push_back(0);

			while(ghostIndex >= 0){
				bool validSuccessor = true;
				for(int i = 0; i<state.ghosts.size(); i++){
					//cout << "HEEEY!" << i << "\n";
					if(!CanGo(state.map, state.ghosts[i].first, state.ghosts[i].second, successors[i])){
						validSuccessor = false;
						break;
					}
				}

				if (validSuccessor){
					auxMinValue = minValue;
					minValue = std::min(minValue, MaxValue(state.createSuccessorState(successors), alpha, beta));
					if(state.currentDepth == 0 && minValue != auxMinValue)
						desitionSuccessors = successors;
					if (minValue >= *alpha) return minValue;
					*beta = std::min(*beta, minValue);
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
			//cout << "UTILITY:::::::::::" << "\n";
			int total = 0;
			list < pair<int,int> >::iterator i;
			for(int i = 0; i<state.ghosts.size(); i++)
				//total += abs(state.ghosts[i].first - state.pacman.first) + abs(state.ghosts[i].second - state.pacman.second);
				total += BFS(state, i);
			//cout << state.getPacmanEats() << "  ----------  EATS!\n";
			total += state.getPacmanEats();
						cout << "UTILITY:::::::::::" << total << "\n";
			return total;
		}

		bool CanGo(Map *map, int x, int y, int direction) 
		{
			//cout << "CanGo::" << x << "," << y << "\n";
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

		bool isGhostPosition(State state, int x, int y){
			for(int i = 0; i<state.ghosts.size(); i++)
				if(state.ghosts[i].first == x && state.ghosts[i].second == y)
					return true;
			return false;
		}
		int BFS(State state, int ghost){
			list< pair <int, int> > bfsQueue;
			vector< pair <int, int> > visiteds;
			pair<int, int> auxPair;
			int treeLevel = 1;
			for(int i = 0; i <= 3; i++)
			{
				auxPair = state.ghosts[ghost];
				visiteds.push_back(auxPair);
				if(auxPair.first == state.pacman.first && auxPair.second == state.pacman.second)
					return 1;
				if(CanGo(state.map, auxPair.first, auxPair.second, i)){
					bfsQueue.push_back(getNextPosition(auxPair, i));
					visiteds.push_back(getNextPosition(auxPair, i));
				}
			}
			bfsQueue.push_back(make_pair(0,0));

			//cout << "TREELEVEL!!!!!! ----- " << treeLevel << "\n";

			while(bfsQueue.size() > 0)
			{
				auxPair = bfsQueue.front();
				if(auxPair == make_pair(0,0))
				{
					if(bfsQueue.size() == 1)return treeLevel;
					treeLevel++;
					bfsQueue.push_back(make_pair(0,0));
				}else if(auxPair.first == state.pacman.first && auxPair.second == state.pacman.second){
					//cout << "PACMAN FOUND!!\n";
					return treeLevel;
				}
				else
				{	
					for(int i = 0; i <= 3; i++)
					{
						if(CanGo(state.map, auxPair.first, auxPair.second, i) &&
							!containsPair(visiteds, getNextPosition(auxPair, i))){
							//cout << "X: " << auxPair.first << "Y: " << auxPair.second << "\n";
							bfsQueue.push_back(getNextPosition(auxPair, i));
							visiteds.push_back(getNextPosition(auxPair, i));
						}
							
					}
				}
				bfsQueue.pop_front();
				//cout << "BFS QUEUE:::::::::::::::" << bfsQueue.size() << "\n";
			}
				//cout << "BFS::::" << bfsQueue.size() << " LEVEL::: " << treeLevel << "\n";
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

		bool containsPair(vector< pair <int, int> > l, pair <int, int> p)
		{
			for(int i = 0; i < l.size(); i++)
				if(l[i].first == p.first && l[i].second == p.second){
					//cout << "ENCOUNTERED!" << "\n";
					return true;
				}
			//cout << "NOT ENCOUNTERED!" << "\n";
			return false;
		}

		/*int Utility(State state){
			int total = 0;
			astarResult astarReturn;
			for(int i = 0; i<state.ghosts.size(); i++){
				astarReturn = astarFunction(*state.map, state.pacman, state.ghosts[i].first, state.ghosts[i].second, 
					astarReturn.eatedPositions, state.map->GetHeight() + (state.map->GetWidth() / 2));
			}
		}

		astarResult astarFunction(Map map, pair<int,int> pacmanPos, int ghostX, int ghostY, vector< pair<int, int> > eatedPositions,
			int depthLimit)
		{
			astarResult result;
			if(pacmanPos.first == ghostX && pacmanPos.second == ghostY)
			{
				result.heuristicValue = 1;
				result.eatedPositions = eatedPositions;
				return result;
			}else if(depthLimit == 0){
				result.heuristicValue = INT_MAX;
				result.eatedPositions = eatedPositions;
				return result;
			}
			vector< pair<int, int> > auxEatedPositions = eatedPositions;
			int nodeHeuristic = 1;
			if(map.GetCell(ghostX, ghostY).HasFood() && 
				find(eatedPositions.begin(), eatedPositions.end(), make_pair(ghostX, ghostY)) == eatedPositions.end()){
				auxEatedPositions.push_back(make_pair(ghostX, ghostY));
				nodeHeuristic += 1;
			}
				
			result.heuristicValue = INT_MAX;

			for(int i = 0; i<=3; i++){
				if(CanGo(&map, ghostX, ghostY, i)){
					astarResult auxResult;
					pair<int, int> nextPos = getNextPosition(ghostX, ghostY, i);
					auxResult = astarFunction(map, pacmanPos, nextPos.first, nextPos.second, auxEatedPositions, depthLimit - 1);
					if(auxResult.heuristicValue < result.heuristicValue){
						result.heuristicValue = auxResult.heuristicValue;
						result.eatedPositions = auxResult.eatedPositions;
					}
				}				
			}

			result.heuristicValue += nodeHeuristic;
			return result;
		}

		pair<int, int>  getNextPosition(int x, int y, int dir){
			switch(dir)
			  	 {			  	 	
			  	 	case UP:
			  	 		return make_pair(x, y + 1);
			  	 	case DOWN:
			  	 		return make_pair(x, y - 1);
			  	 	case RIGHT:
			  	 		return make_pair(x + 1, y);
			  	 	case LEFT:
			  	 		return make_pair(x - 1, y);
			  	 }
		}
/*
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
	