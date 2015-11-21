.PHONY : module demo clean all

all: module demo

module:
	cd module && $(MAKE) -f Makefile all
demo:
	cd demo && $(MAKE) -f Makefile all

clean:
	cd module && $(MAKE) -f Makefile clean
	cd demo && $(MAKE) -f Makefile clean

module_sign:
	cd module && $(MAKE) -f Makefile module_sign
