BINTEST				= helloworld.elf
TYPEOS				= LINUX
CXX						= g++
ERROR_FLAGS		= -Wall -W -pedantic
DEBUG_FLAGS		= -g -O0
INC_FLAGS			= -I$(PWD)/../../api

# Compiler
CXXFLAGS = $(DEBUG_FLAGS) $(INC_FLAGS) $(ERROR_FLAGS)

# Linker
LDFLAGS = $(DEBUG_FLAGS) $(INC_FLAGS) $(ERROR_FLAGS)

all: $(BINTEST)


$(BINTEST): helloworld.o
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

%.o:%.c 
	echo 'Building target: $@'
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o *.elf
