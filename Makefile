CC = g++-4.7
CFLAGS = -Wall -Werror

SOURCES = main/*.cpp algo/*.cpp arch/*.c
HEADERS = includes/*.h includes/*.hpp
EXECUTABLE = bin/main

KANGAROO_BODY = $(CC) $(CFLAGS)
KANGAROO_TAIL = $(SOURCES) -o $(EXECUTABLE)

ifdef O
CFLAGS += -O$(O)
endif

main:
	mkdir -p bin graphs output
	$(KANGAROO_BODY) $(KANGAROO_TAIL)

dbg:
	$(KANGAROO_BODY) -g $(KANGAROO_TAIL)

clean:
	rm -rf bin/*

.PHONY: main dbg clean
