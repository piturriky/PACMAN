CC = g++
LIBS = -lglut -lGLU -lGL -lm 
PROGRAM_NAME = pacman

$(PROGRAM_NAME): pacman.cpp
	$(CC) -o $(PROGRAM_NAME) pacman.cpp $(LIBS)  
clean:
	rm $(PROGRAM_NAME)