CC=g++
CFLAGS= -O3 -Wall -std=c++11
LIB= abc/libabc.a -lm -ldl -lreadline -lpthread -lrt
INC= -I./src
SRC= src/pattern.cpp src/io_helper.cpp src/inputs.cpp src/abc_wrapper.cpp src/parser.cpp src/group.cpp src/util.cpp
OBJ= $(SRC:.cpp=.o)
EXE= lrg test
OTH= sim_pat.txt io_rel.txt in_pat.txt pre_opt.v inter_opt.v top.pla

.PHONY= all clean

all: $(EXE)

lrg: src/lrg.cpp $(OBJ)
	@make -C abc libabc.a
	@mkdir -p circuits/
	@echo "> Building $@..."
	@$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LIB)

test: src/test.cpp src/pattern.o src/io_helper.o src/abc_wrapper.o 
	@echo "> Building $@..."
	@$(CC) $(CFLAGS) $(INC) -o $@ $^ $(LIB)

%.o: %.cpp
	@echo "> Compiling $<..."
	@$(CC) $(CFLAGS) $(INC) -c $< -o $@

clean:
	-@rm -f $(OBJ) $(EXE) $(OTH)
