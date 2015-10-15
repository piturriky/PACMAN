
class Particle{
	public:
		Particle(){
			state = QUIET;
		}
		~Particle(){}
		
		void SetPosition(int x, int y)
		{
			this->x = x;
			this->y = y;
		}

		void InitMovement(int destinationX, int destinationY, int duration)
		{
			vx = (destinationX - x)/duration;
			vy = (destinationY - y)/duration;

			state = MOVE;
			time_remaining = duration;
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
				x = x + vx*time_remaining;
				y = y + vy*time_remaining;

				state = QUIET;
			}
		}

		void draw()
		{
			glColor3f(1,1,1);
			glBegin(GL_QUADS);
			glVertex2i(x-6, y-6);
			glVertex2i(x+6, y-6);
			glVertex2i(x+6, y+6);
			glVertex2i(x-6, y+6);
			glEnd();
		}
	private:
		float x, y;
		float vx, vy;
		int state;
		long time_remaining;
};