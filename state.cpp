class State{

	public:
		pair<int, int> pacman;
		vector<pair<int, int> > ghosts;
		Map* mapa;

		int depth, currentDepth;

		int activeGhosts;

		State(pair<int, int> pacman, vector<pair<int, int> > ghosts, Map* mapa, int depth, int currentDepth = 0, int pacmanEats = 0){
			this->pacman = pacman;
			this->ghosts = ghosts;

			this->mapa = mapa;

			this->depth = depth;
			this->currentDepth = currentDepth;
			this->pacmanEats = pacmanEats + 1 * mapa->GetCell(pacman.first, pacman.second).HasFood();
     	}

     	bool isTerminal(){
     		return currentDepth == depth;
     	}

     	State createSuccessorState(vector<int> successors){
     		vector<pair<int, int> > newGhosts;
     		for(int i = 0; i < ghosts.size(); i++){
     			newGhosts.push_back(getNextPosition(this->ghosts[i], successors[i]));
     		}
     		State *state = new State(this->pacman, newGhosts, mapa, depth, currentDepth + 1);
     		return *state;
     	}

     	State createSuccessorState(int direction){
     		State *state = new State(getNextPosition(this->pacman, direction), ghosts, mapa, depth, currentDepth + 1, this->pacmanEats);
     		return *state;
     	}

     	pair<int, int> getNextPosition(pair<int, int> position, int direction){
     		switch(direction)
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

     	int getPacmanEats(){
     		return this->pacmanEats;
     	}


     	~State(){}

	private:
		int pacmanEats;

};