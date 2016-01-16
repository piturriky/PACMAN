list< pair<int, int> > visiteds;

int alphaBetaDesition(State state){

}

int MaxValue(State state, int alpha, int beta){
	if(state.isTerminal()) return 0;
}

int MinValue(State state, int alpha, int beta){

}

int Utility(State state){
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
}
	