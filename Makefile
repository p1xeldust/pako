CC=c++

all:
	$(CC) -std=c++17 -o pako src/main.cpp -larchive
install:
	$(CC) -std=c++17 -o pako src/main.cpp -larchive
	cp pako /usr/bin
