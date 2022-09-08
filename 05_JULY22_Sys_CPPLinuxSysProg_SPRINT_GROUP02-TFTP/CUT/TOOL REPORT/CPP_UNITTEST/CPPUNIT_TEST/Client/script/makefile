CC=g++
CPPFLAGS=-c -g -Wall 
LDFLAGS=-lcppunit
OBJ=obj/
SRC=src/
BIN=bin/
INCLUDE=inc/

all:test


test: $(OBJ)main.o $(OBJ)client.o 
			$(CC) -o test $(OBJ)main.o $(OBJ)client.o  $(LDFLAGS)
						mv test $(BIN)

$(OBJ)main.o: $(SRC)main.cpp
			$(CC) $(CPPFLAGS) $(SRC)main.cpp -I $(INCLUDE)
						mv main.o $(OBJ)

$(OBJ)client.o: $(SRC)client.cpp
			$(CC) $(CPPFLAGS) $(SRC)client.cpp -I $(INCLUDE)
						mv client.o $(OBJ)



valgrind: $(BIN)test
			valgrind --leak-check=full --show-reachable=yes	$(BIN)test 1>$(BIN)/valgrind.txt

clean:
			rm -f $(OBJ)*.o 
					rm -f $(BIN)test
							rm -f $(BIN)/valgrind.txt



