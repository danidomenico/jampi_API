JAMPI_PATH = ../../jampi
NAME = fib

SRC = $(wildcard *.cpp)
OBJ = $(SRC:.cpp=.o)

CXX = g++
LINK = $(CXX)
EXE = $(NAME)

PARALLEL_LIBS = -pthread
PARALLEL_CXXFLAGS = -std=c++11 -lpthread
PARALLEL_CPPFLAGS = -I$(JAMPI_PATH)

CXXFLAGS = -g 
#CXXFLAGS += -DVERBOSE
#CXXFLAGS += -O3

default: $(EXE)

$(EXE): $(OBJ)
	$(LINK) $(OBJ) $(PARALLEL_LIBS) -o $(EXE)

%.o:%.cpp
	$(CXX) $(CXXFLAGS) $(PARALLEL_CPPFLAGS) $(PARALLEL_CXXFLAGS) -c $<

clean: 
	rm -f *.o $(EXE)
	
clean-o: 
	rm -f *.o
