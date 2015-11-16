BINTEST    		= helloworld.elf
TYPEOS     		= LINUX
CC         		= gcc
ERROR_FLAGS             = -Wall -W -pedantic
DEBUG_FLAGS             = -g -O0
INC_FLAGS               = -I$(PWD)/../../api

# Compiler
CFLAGS = $(DEBUG_FLAGS) $(INC_FLAGS) $(ERROR_FLAGS)

# Linker
LDFLAGS = $(DEBUG_FLAGS) $(INC_FLAGS) $(ERROR_FLAGS)

all: $(BINTEST)


$(BINTEST): helloworld.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o:%.c 
	echo 'Building target: $@'
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.elf
