CC = g++-4.7
CFLAGS = -Wall -Werror

SOURCES = main/*.cpp algo/*.cpp arch/*.c
HEADERS = includes/*.h includes/*.hpp
EXECUTABLE = bin/main

KANGAROO_BODY = $(CC) $(CFLAGS)
KANGAROO_TAIL = $(SOURCES) -o $(EXECUTABLE)
MKDIRS = mkdir -p bin graphs output 
RUN = ./$(EXECUTABLE)

ifdef O
CFLAGS += -O$(O)
endif

main:
	$(MKDIRS)
	$(KANGAROO_BODY) $(KANGAROO_TAIL)

dbg:
	$(KANGAROO_BODY) -g $(KANGAROO_TAIL)

clean:
	rm -rf bin/*

run:
	$(MKDIRS)
	$(KANGAROO_BODY) $(KANGAROO_TAIL)
	$(RUN) $(t) $(q)

.PHONY: main dbg clean
