CC = g++
LIBS = -lglut -lGLU -lGL -lm 
PROGRAM_NAME = pacman

$(PROGRAM_NAME): main.cpp
	$(CC) -o $(PROGRAM_NAME) main.cpp $(LIBS)  
clean:
	rm $(PROGRAM_NAME)