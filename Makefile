CC = g++-4.7
CFLAGS = -Wall -Werror

SOURCES = main/*.cpp algo/*.cpp arch/*.c
HEADERS = includes/*.h includes/*.hpp
EXECUTABLE = bin/main

KANGAROO_BODY = $(CC) $(CFLAGS)
KANGAROO_TAIL = $(SOURCES) -o $(EXECUTABLE)
MKDIRS = mkdir -p bin graphs output 

OH = -O3
#ifdef O
#CFLAGS += -O$(O)
#endif

main:
	$(MKDIRS)
	$(KANGAROO_BODY) $(OH) $(KANGAROO_TAIL)

dop:
	rm -f output/*

norm:
	$(MKDIRS)
	$(KANGAROO_BODY) $(KANGAROO_TAIL)

dbg:
	$(KANGAROO_BODY) -g $(KANGAROO_TAIL)
	gdb $(EXECUTABLE)

gdb:
	$(KANGAROO_BODY) -g $(KANGAROO_TAIL)
	gdb $(EXECUTABLE)

clean:
	rm -rf bin/*

run:
	$(MKDIRS)
	$(KANGAROO_BODY) $(OH) $(KANGAROO_TAIL)
	./$(EXECUTABLE) $(t) $(q)

hist:
	$(MKDIRS)
	$(KANGAROO_BODY) $(OH) $(KANGAROO_TAIL)
	./$(EXECUTABLE) $(t) $(q)
	python main/gen_hist.py $(r1) $(r2)

.PHONY: main dbg clean
