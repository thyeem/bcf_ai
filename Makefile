CXX=g++
INC=
LIBS=
CXXFLAGS=-g $(INC) -Ofast -Wall -std=c++11 
LDFLAGS=-lncurses $(LIBS)
TARGET=bcf_ai

#----------------------------------------------------
SRC=main.cpp board.cpp game.cpp sofiai.cpp mariai.cpp draw.cpp
OBJ=$(SRC:%.cpp=%.o)
DEP=$(OBJ:%.o=%.d)

all: $(TARGET) 
$(TARGET): $(OBJ) 
	$(CXX) -o $@ $^ $(LDFLAGS)

-include $(DEP)
%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@


SRC=api_sofia.cpp board.cpp sofiai.cpp 
OBJ=$(SRC:%.cpp=%.o)
DEP=$(OBJ:%.o=%.d)
sofia: $(OBJ) 
	$(CXX) -o $@ $^ $(LDFLAGS)

-include $(DEP)
%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@


SRC=api_maria.cpp board.cpp mariai.cpp draw.cpp
OBJ=$(SRC:%.cpp=%.o)
DEP=$(OBJ:%.o=%.d)
maria: $(OBJ) 
	$(CXX) -o $@ $^ $(LDFLAGS)

-include $(DEP)
%.o: %.cpp 
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

#----------------------------------------------------
c: clean clean_bin
clean:
	rm -f *.o
	rm -f *.d

clean_bin:
	rm -f $(TARGET) sofia maria
