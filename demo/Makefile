
all:
	for i in $(shell find ./* -maxdepth 0 -type d); do \
	  cd $$i; \
	  if [ $$(ls *.pro 2>/dev/null | wc -l) -eq 0 ]; then \
	    $(MAKE) -f Makefile all ; \
	  else \
	    qmake ; \
	    $(MAKE) -f Makefile all ; \
	  fi; \
	  cd ..;\
	done;

clean:
	for i in $(shell find ./* -maxdepth 0 -type d); do \
	  cd $$i; \
	  if [ $$(ls *.pro 2>/dev/null | wc -l) -eq 0 ]; then \
	    $(MAKE) -f Makefile clean ; \
	  else \
	    $(MAKE) -f Makefile distclean ; \
	  fi; \
	  cd ..;\
	done;
