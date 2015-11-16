
all:
	cd module && $(MAKE) -f Makefile all
	cd bin && $(MAKE) -f Makefile all

clean:
	cd module && $(MAKE) -f Makefile clean
	cd bin && $(MAKE) -f Makefile clean

module_sign:
	cd module && $(MAKE) -f Makefile module_sign
