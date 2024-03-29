CXX=g++
LIBS=
CXXFLAGS=-Ofast -Wall -std=c++14
LDFLAGS=-lncurses $(LIBS)
TARGET=bcf_ai


.PHONY: all
SRC=main.cpp board.cpp game.cpp sofiai.cpp mariai.cpp draw.cpp pattern.cpp
OBJ=$(SRC:%.cpp=%.o)
DEP=$(OBJ:%.o=%.d)
all: $(TARGET)
$(TARGET): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)
-include $(DEP)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@


.PHONY: sofia
SRC=call_sofiai.cpp board.cpp sofiai.cpp
OBJ=$(SRC:%.cpp=%.o)
DEP=$(OBJ:%.o=%.d)
sofia: $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)
-include $(DEP)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@


.PHONY: maria
SRC=call_mariai.cpp board.cpp mariai.cpp draw.cpp pattern.cpp
OBJ=$(SRC:%.cpp=%.o)
DEP=$(OBJ:%.o=%.d)
maria: $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)
-include $(DEP)
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@


.PHONY: sofimarie
sofimarie:
	make sofia
	make maria


.PHONY: clean
clean:
	rm -f *.o
	rm -f *.d
	rm -f $(TARGET) sofia maria
