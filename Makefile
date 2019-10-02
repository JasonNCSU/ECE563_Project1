CC = g++ -std=c++11
OPT = -O3
#OPT = -g
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

# List all your .cc/.cpp files here (source files, excluding header files)
SIM_SRC = cache.cpp main.cpp

# List corresponding compiled object files here (.o files)
SIM_OBJ = sim_cache.o

#################################

# default rule

all: sim_cache
	@echo "my work is done here..."

# generic rule for converting any .cpp file to any .o file
 
.cc.o:
	$(CC) $(CFLAGS)  -c $*.cc

.cpp.o:
	$(CC) $(CFLAGS)  -c $*.cpp


# rule for making sim_cache

sim_cache: $(SIM_SRC)
	$(CC) $(SIM_SRC) -o sim_cache $(CFLAGS) -lm
	@echo "-----------DONE WITH sim_cache-----------"



# type "make clean" to remove all .o files plus the sim_cache binary

clean:
	rm -f *.o sim_cache


# type "make clobber" to remove all .o files (leaves sim_cache binary)

clobber:
	rm -f *.o


