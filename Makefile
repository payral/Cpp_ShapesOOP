#CXX=clang++
CXX=g++

FLAGS = -pthread -O3 -std=c++14 -g 

all:
	${CXX} ${FLAGS} *.cpp
