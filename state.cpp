class State{

	public:
		pair<int, int> pacman;
		list<pair<int, int> > ghosts;
		Map* map;

		int depth, currentDepth;

		int activeGhosts;

		State(pair<int, int> pacman, list<pair<int, int> > ghosts, Map* map, int depth){
			this->pacman = pacman;
			this->ghosts = ghosts;

			this->map = map;

			this->depth = depth;
			currentDepth = 0;
     	}

     	bool isTerminal(){
     		return currentDepth == depth;
     	}

     	~State(){}

	private:

};