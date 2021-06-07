CXX=g++
CXXFLAGS=-g -std=c++11 -Wall -pedantic
BIN=prog

SRC=test.cpp

all: test.out
	./test.out

test.out: $(SRC)
	$(CXX) $(SRC) -Wall -Iinclude -std=c++17 -o test.out

clean:
	rm -rf test.out test.csv
