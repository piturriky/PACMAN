int alphaBetaDesition(State state){

}

int MaxValue(State state, int alpha, int beta){
	if(state.isTerminal()) return 
}

int MinValue(State state, int alpha, int beta){

}

int Utility(State state){
	int dist = 0;
	list<pair<int,int> >::iterator i;
	for(i = state.ghosts.begin(); i!=state.ghosts.end(); i++){

	}
}

list< pair<int, int> > visiteds;

int BFS(State state){
	queue< queue < pair<int, int> > > bfsQueue;
	queue< pair<int, int> > result;

	visiteds.push_front( make_pair(i->getX(), i->getY()));
	buidar i afegir primeres cues de camins a bfsQueue comprovant si ja hem trobat el pacman;
	while(true){
		si la cua nomes te un element ja hem trobat cap on es moura el fantasma pero seguirem desenvolupant per marcar el cami per on pasem.
		desenvolupar següent element de la cua comprobant si hem trobat el pacman i si podem continuar per akell cami (visiteds);
		quan trobem el pacman o no podem continuar pq esta tot marcat, sortim del bucle
	}
	marcar posicions de result i determinar cap on anira el fantasma
	