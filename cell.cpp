class Cell{
	public:
		Cell(){
			type = EMPTY;
			activeWall = false;
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
	private:
		int type;
		bool activeWall;
};