CXX= g++
LINK = $(CXX)
EXE = gemm.sequential
CXXFLAGS = -O3 -std=c++11

#CXXFLAGS += -g
#CXXFLAGS += -DVERBOSE

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

OBJ_ROOT = ../jampi/gemm.o 
OBJ_ROOT_LINK = gemm.o

default: $(EXE)

$(EXE): $(OBJ) $(OBJ_ROOT)
	$(LINK) $(OBJ) $(OBJ_ROOT_LINK) -o $@

%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -rf *o $(EXE)

clean-o:
	rm -rf *o
