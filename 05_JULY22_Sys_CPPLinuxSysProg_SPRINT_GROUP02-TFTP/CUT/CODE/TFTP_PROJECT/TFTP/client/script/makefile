CC=g++
CPPFLAGS=-c -g -Wall -std=c++11
OBJ=obj/
SRC=src/
BIN=bin/
INCLUDE=inc/

all:client

client:$(OBJ)main.o $(OBJ)client.o
		$(CC) -o client $(OBJ)main.o $(OBJ)client.o
			mv client $(BIN)
$(OBJ)main.o: $(SRC)main.cpp
		$(CC) $(CPPFLAGS) $(SRC)main.cpp -I $(INCLUDE)
			mv main.o $(OBJ)
$(OBJ)client.o : $(SRC)client.cpp
		$(CC) $(CPPFLAGS) $(SRC)client.cpp -I $(INCLUDE)
			mv client.o $(OBJ)
valgrind : $(BIN)client
		valgrind --leak-check=full -show-reachable=yes $(BIN)client 1>$(BIN)valgrind.txt

clean :
	rm -f $(OBJ)*.o
	rm -f $(BIN)client
	rm -f $(BIN)/valgrind.txt


