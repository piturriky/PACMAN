
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
			this->lastInBox = true;

			this->pacmanDegreeVisivility = 45;
		}
		~Particle(){}
		
		void SetPosition(int x, int y)
		{
			this->x = x;
			this->y = y;
		}

		void SetDegreeVisivility(int x)
		{
			this->pacmanDegreeVisivility = x;
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
			lastInBox = true;
		}

		bool LastInBox(){
			return this->lastInBox;
		}

		void OutBox(){
			this->lastInBox = false;
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

		void drawLight(int particle){

			GLenum lights;

			float colorValue = 0.8;
			int degree = 20;

			if(particle == 0){
				colorValue = 0.8;
				degree = this->pacmanDegreeVisivility;
			}

			GLint position[4];
  			GLfloat color[4];
  			GLfloat dir[3];

  			switch(current_direction){
  				case UP:
  					dir[0]=0;dir[1]=0;dir[2]=-1;
  					break;
				case DOWN:
  					dir[0]=0;dir[1]=0;dir[2]=1;
  					break;
				case RIGHT:
  					dir[0]=1;dir[1]=0;dir[2]=0;
  					break;
				case LEFT:
  					dir[0]=-1;dir[1]=0;dir[2]=0;
  					break;
  			}

  			switch (particle){
  				case 0:
  					lights = GL_LIGHT2;
  					break;
				case 1:
  					lights = GL_LIGHT3;
  					break;
				case 2:
  					lights = GL_LIGHT4;
  					break;
				case 3:
  					lights = GL_LIGHT5;
  					break;
  			}
  				

  			glLightfv(lights,GL_SPOT_DIRECTION,dir);
  			glLightf(lights,GL_SPOT_CUTOFF,degree);
  			glLightf(lights,GL_SPOT_EXPONENT,10);

			position[0]=(x+0.5)*cellWidth; position[1]=Y+radiParticle; position[2]=(y+0.5)*cellHeight; position[3]=1; 
			glLightiv(lights,GL_POSITION,position);

			color[0]=colorValue; color[1]=colorValue; color[2]=colorValue; color[3]=1;
			glLightfv(lights,GL_DIFFUSE,color);

			glLightf(lights,GL_CONSTANT_ATTENUATION,1.0);
			glLightf(lights,GL_LINEAR_ATTENUATION,0.0);
			glLightf(lights,GL_QUADRATIC_ATTENUATION,0.0);

			glEnable(lights);
		}

		void draw()
		{
			GLfloat material[4];
			//glColor3f(r,g,b);
			material[0]=r; material[1]=g; material[2]=b; material[3]=1.0; 
  			glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE,material);

			GLUquadricObj *quadric;
			quadric = gluNewQuadric();

			gluQuadricDrawStyle(quadric, GLU_FILL);

			glPushMatrix();
			glTranslatef((x+0.5)*cellWidth,Y+radiParticle,(y+0.5)*cellHeight);
			gluSphere(quadric,radiParticle,36,18);
			glPopMatrix();

			gluDeleteQuadric(quadric);
			/*glBegin(GL_QUADS);
			glVertex2i((x+GHOST)*cellWidth, (y+GHOST)*cellHeight);
			glVertex2i((x+1-GHOST)*cellWidth, (y+GHOST)*cellHeight);
			glVertex2i((x+1-GHOST)*cellWidth, (y+1-GHOST)*cellHeight);
			glVertex2i((x+GHOST)*cellWidth, (y+1-GHOST)*cellHeight);
			glEnd();*/
		}

		int getNextX(){
			if(state == QUIET) return x;
			switch(current_direction){
				case RIGHT:
					return x + 1;
				case LEFT:
					return x - 1;
			}
			return x;
		}

		int getNextY(){
			if(state == QUIET) return y;
			switch(current_direction){
				case UP:
					return y + 1;
				case DOWN:
					return y - 1;
			}
			return y;
		}
	private:
		float x, y;
		float vx, vy;
		int state, pacmanDegreeVisivility;
		long time_remaining;
		float r, g, b; //RGB color
		int new_direction, current_direction;
		bool canGoOut, lastInBox;
};