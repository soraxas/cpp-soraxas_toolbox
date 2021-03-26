CXX=g++
CXXFLAGS=-g -std=c++11 -Wall -pedantic
BIN=prog

SRC=test.cpp

test.out: $(SRC)
	$(CXX) $(SRC) -Wall -Iinclude -o test.out

clean:
	rm -rf test.out test.csv
