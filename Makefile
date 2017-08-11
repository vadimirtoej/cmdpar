CXX=g++
CXXFLAGS=-std=c++14
CXXFLAGS+=-Wall
CXXFLAGS+=-O3

SRCS=cmdpar.cpp

cmdpar: $(SRCS)
	$(CXX) $^ -o $@ $(CXXFLAGS)

.PHONY: clean
clean:
	rm cmdpar

