CXX=g++
CXX_FLAGS=-g -std=c++11
.PHONY: all clean
TARGETS=turtle

all: $(TARGETS)

netgrid: netgrid.cc trail.cc 
	$(CXX) $(CXX_FLAGS) -pthread -o $@ $^

turtle: turtle.cc
	$(CXX) $(CXX_FLAGS) -o $@ $^

clean:
	rm -rf *~ *.o $(TARGETS) *.dSYM
