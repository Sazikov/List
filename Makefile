FLAGS=-Wall -Wextra -Wpedantic
SRC = main.cpp list.cpp

all:
	g++ $(FLAGS) $(SRC) -o main