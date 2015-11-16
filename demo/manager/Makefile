BINTEST    		= manager.elf
TYPEOS     		= LINUX
CC         		= gcc
ERROR_FLAGS             = -Wall -W -pedantic
DEBUG_FLAGS             = -g -O0
INC_FLAGS               = -I$(PWD)/../../api

# Compiler
GLIB_CFLAGS = $$(pkg-config --cflags glib-2.0)
PANGO_CFLAGS = $$(pkg-config --cflags pango)
GTK_CFLAGS = $$(pkg-config --cflags gtk+-2.0)
CFLAGS = $(DEBUG_FLAGS) $(INC_FLAGS) $(ERROR_FLAGS) $(GLIB_CFLAGS) $(PANGO_CFLAGS) $(GTK_CFLAGS)

# Linker
GLIB_LDFLAGS = $$(pkg-config --libs glib-2.0)
PANGO_LDFLAGS = $$(pkg-config --libs pango)
GTK_LDFLAGS = $$(pkg-config --libs gtk+-2.0)
LDFLAGS = $(DEBUG_FLAGS) $(INC_FLAGS) $(ERROR_FLAGS) $(GLIB_LDFLAGS) $(PANGO_LDFLAGS) $(GTK_LDFLAGS)


all: $(BINTEST)


$(BINTEST): manager.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

%.o:%.c 
	echo 'Building target: $@'
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o *.elf
