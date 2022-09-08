CC=g++
CPPFLAGS=-c -g -Wall 
LDFLAGS=-lcppunit
OBJ=obj/
SRC=src/
BIN=bin/
INCLUDE=inc/

all:test


test: $(OBJ)main.o $(OBJ)server.o 
			$(CC) -o test $(OBJ)main.o $(OBJ)server.o  $(LDFLAGS)
						mv test $(BIN)

$(OBJ)main.o: $(SRC)main.cpp
			$(CC) $(CPPFLAGS) $(SRC)main.cpp -I $(INCLUDE)
						mv main.o $(OBJ)

$(OBJ)server.o: $(SRC)server.cpp
			$(CC) $(CPPFLAGS) $(SRC)server.cpp -I $(INCLUDE)
						mv server.o $(OBJ)



valgrind: $(BIN)test
			valgrind --leak-check=full --show-reachable=yes	$(BIN)test 1>$(BIN)/valgrind.txt

clean:
			rm -f $(OBJ)*.o 
					rm -f $(BIN)test
							rm -f $(BIN)/valgrind.txt



