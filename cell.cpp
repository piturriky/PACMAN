class Cell{
	public:
		Cell(){
			type = EMPTY;
			activeWall = false;
			hasFood = true;
			printf("NEW CELL\n");
		}
		~Cell(){}
		bool IsType(int t){
            if(type == t)
                return true;
            else
                return false;
		}
		int GetType(){
			return type;
		}
		void SetType(int t){
			type = t;
		}
		bool IsActive(){
			return activeWall;
		}
		void SetActive(bool a){
			activeWall = a;
		}
		bool HasFood(){
			return hasFood;
		}
		void EatFood(){	
			this->hasFood = false;
		}
		
	private:
		int type;
		bool activeWall;
		bool hasFood;
};