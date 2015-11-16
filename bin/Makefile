BINTEST    		= nhm.elf
TYPEOS     		= LINUX
CC         		= gcc
CFLAG      		= -g -O0 -I$(PWD)/../api -D$(TYPEOS)

all: $(BINTEST)


$(BINTEST): nhm.o
	$(CC) $(CFLAG) $^ -o $@

%.o:%.c 
	echo 'Building target: $@'
	$(CC) $(CFLAG) -c $< -o $@

clean:
	rm -f *.o *.elf
