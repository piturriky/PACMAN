CC = g++
LIBS = -lglut -lGLU -lGL -lm -l jpeg -L /usr/local/lib -std=c++11
PROGRAM_NAME = pacman

$(PROGRAM_NAME): main.cpp
	$(CC) -o $(PROGRAM_NAME) main.cpp $(LIBS)  
clean:
	rm $(PROGRAM_NAME)