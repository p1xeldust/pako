CXX=c++
INSTALL_DIR=/usr/bin

all:
	${CXX} -std=c++17 -Wall -o pako src/main.cpp -larchive

install:
	${CXX} -std=c++17 -Wall -o pako src/main.cpp -larchive
	cp pako ${INSTALL_DIR}
