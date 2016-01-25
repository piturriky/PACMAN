class State{

	public:
		pair<int, int> pacman;
		vector<pair<int, int> > ghosts;
		Map* map;

		int depth, currentDepth;

		int activeGhosts;

		State(pair<int, int> pacman, vector<pair<int, int> > ghosts, Map* map, int depth, int currentDepth = 0){
			this->pacman = pacman;
			this->ghosts = ghosts;

			this->map = map;

			this->depth = depth;
			this->currentDepth = currentDepth;
     	}

     	bool isTerminal(){
     		return currentDepth == depth;
     	}

     	State createSuccessorState(vector<int> successors){
     		vector<pair<int, int> > newGhosts;
     		for(int i = 0; i < ghosts.size(); i++){
     			newGhosts.push_back(getNextPosition(i, successors[i]));
     		}
     		State *state = new State(this->pacman, newGhosts, map, depth, currentDepth + 1);
     		return *state;
     	}

     	pair<int, int> getNextPosition(int i, int direction){
     		switch(direction)
			{
				case UP:
					return make_pair(this->ghosts[i].first, this->ghosts[i].second + 1);
				case DOWN:
					return make_pair(this->ghosts[i].first, this->ghosts[i].second - 1);
				case RIGHT:
					return make_pair(this->ghosts[i].first + 1, this->ghosts[i].second);
				case LEFT:
					return make_pair(this->ghosts[i].first - 1, this->ghosts[i].second);
			}
     	}

     	~State(){}

	private:

};