CXX=g++
CPPFLAGS=-std=c++11 -Wall

sudoku: main.o
	$(CXX) main.o -o out/sudoku