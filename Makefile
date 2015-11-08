
all:
	cd module && $(MAKE) -f Makefile all
	cd test && $(MAKE) -f Makefile all

clean:
	cd module && $(MAKE) -f Makefile clean
	cd test && $(MAKE) -f Makefile clean

module_sign:
	cd module && $(MAKE) -f Makefile module_sign
