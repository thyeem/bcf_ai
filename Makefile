BIN=bcf_ai
#==============================
#CXX=i586-mingw32msvc-g++
CXX=g++
C11=-std=c++11
LIB=-lncurses
LIBDIR= #-L/opt/local/lib
INC=    #-I/Users/thyeem/sofiai

CFLAGS=-g -Ofast -Wall $(C11) $(INC)
LFLAGS=$(LIB) $(LIBDIR)

SRC=$(wildcard *.cpp) #$(wildcard dir/*.cpp) 
OBJ=$(SRC:%.cpp=%.o) 
DEP=$(OBJ:%.o=%.d)

.PHONY: all 
all: $(BIN)
$(BIN): $(OBJ) 
	$(CXX) -o $@ $^ $(LFLAGS) 

-include $(DEP)
%.o: %.cpp
	$(CXX) $(CFLAGS) -MMD -c $< -o $@

.PHONY: f fresh 
f: fresh
fresh: all
	-rm -rf $(OBJ) $(DEP)

.PHONY: r run 
r: run
run: fresh
	@ echo
	@ echo === run =====
	@ ./$(BIN)

.PHONY: c clean
c: clean
clean:
	-rm -rf $(BIN) $(OBJ) $(DEP)

