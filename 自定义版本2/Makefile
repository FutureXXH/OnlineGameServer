CC = g++ -std=c++17
CFLAGS = -w -g -O2

EXE = GameServer
Lib = ./lib
Files = ./*.h ./Modules/*.h ./*.cpp  ./Modules/*.cpp 

.PHONY: clean
main:
	$(CC) $(CFLAGS) -I$(Files) -o $(EXE) -lpthread