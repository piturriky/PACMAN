
class Particle{
	public:
		Particle(float r, float g, float b){
			this->r = r;
			this->g = g;
			this->b = b;
			this->state = QUIET;
			this->new_direction = -1;
			this->current_direction = -1;
			this->canGoOut = false;
		}
		~Particle(){}
		
		void SetPosition(int x, int y)
		{
			this->x = x;
			this->y = y;
		}

		int GetX()
		{
			return x;
		}

		int GetY()
		{
			return y;
		}

		void SetNewDirection(int direction)
		{
			this->new_direction = direction;
		}

		int GetNewDirection()
		{
			return this->new_direction;
		}

		void SetCurrentDirection(int direction)
		{
			this->current_direction = direction;
		}

		int GetCurrentDirection()
		{
			return this->current_direction;
		}


		void InitMovement(int destinationX, int destinationY, int duration)
		{
			vx = (destinationX - x)/duration;
			vy = (destinationY - y)/duration;

			state = MOVE;
			time_remaining = duration;
		}

		int GetState()
		{
			return state;
		}

		bool CanGoOut(){
			return canGoOut;
		}

		void GoOut(){
			canGoOut = true;
		}

		void Out(){
			canGoOut = false;
		}

		void Integrate(long t)
		{
			if(state == MOVE && t < time_remaining)
			{
				x = x + vx*t;
				y = y + vy*t;
				time_remaining -= t;
			}
			else if(state == MOVE && t >= time_remaining)
			{
				x = round(x + vx*time_remaining);
				y = round(y + vy*time_remaining);

				state = QUIET;
			}
		}

		void draw()
		{
			glColor3f(r,g,b);
			glBegin(GL_QUADS);
			glVertex2i((x+GHOST)*cellWidth, (y+GHOST)*cellHeight);
			glVertex2i((x+1-GHOST)*cellWidth, (y+GHOST)*cellHeight);
			glVertex2i((x+1-GHOST)*cellWidth, (y+1-GHOST)*cellHeight);
			glVertex2i((x+GHOST)*cellWidth, (y+1-GHOST)*cellHeight);
			glEnd();
		}
	private:
		float x, y;
		float vx, vy;
		int state;
		long time_remaining;
		float r, g, b; //RGB color
		int new_direction, current_direction;
		bool canGoOut;
};