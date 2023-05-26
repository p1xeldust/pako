CXX=c++

all:
	${CXX} -std=c++17 -Wall -o pako src/pako.cpp -lstdc++fs

