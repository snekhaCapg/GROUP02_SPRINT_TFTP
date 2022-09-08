CC=g++
CPPFLAGS=-c -g -Wall -std=c++11
OBJ=obj/
SRC=src/
BIN=bin/
INCLUDE=inc/

all:server

server:$(OBJ)main.o $(OBJ)server.o
		$(CC) -o server $(OBJ)main.o $(OBJ)server.o
			mv server $(BIN)
$(OBJ)main.o: $(SRC)main.cpp
		$(CC) $(CPPFLAGS) $(SRC)main.cpp -I $(INCLUDE)
			mv main.o $(OBJ)
$(OBJ)server.o : $(SRC)server.cpp
		$(CC) $(CPPFLAGS) $(SRC)server.cpp -I $(INCLUDE)
			mv server.o $(OBJ)
valgrind : $(BIN)server
		valgrind --leak-check=full -show-reachable=yes $(BIN)server 1>$(BIN)valgrind.txt

clean :
	rm -f $(OBJ)*.o
	rm -f $(BIN)server
	rm -f $(BIN)/valgrind.txt


