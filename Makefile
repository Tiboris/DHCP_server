# Author Tibor Dudlák xdudla00@stud.fit.vutbr.cz
CXX = g++
CXXFLAGS = -O3 -g -Wall -Wextra -pedantic -std=c++11

default: dserver

dserver: req_handler.o arg_parser.o dserver.o
	$(CXX) req_handler.o arg_parser.o  dserver.o -o dserver

dserver.o: dserver.cpp

req_handler.o: req_handler.cpp

arg_parser.o: arg_parser.cpp

clean:
	rm -f dserver *.o
