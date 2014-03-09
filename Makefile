CC = g++-4.7
CFLAGS = -Wall -Werror -std=c++11
LIBS = -lpthread

GENERAL_SOURCES = algo/*.cpp ds/*.cpp
OPTIMIZABLE_SOURCES = main/*.cpp $(GENERAL_SOURCES)
OPTIMIZABLE_OBJECT = bin/opt.o
VOLATILE_SOURCES = arch/*.c
VOLATILE_OBJECT = bin/vol.o
HEADERS = includes/*.h includes/*.hpp
MAIN = bin/main
DGEN = bin/dgen

KANGAROO_BODY = $(CC) $(CFLAGS)
#KANGAROO_TAIL = $(SOURCES) -o $(MAIN)
MKDIRS = mkdir -p bin graphs output

OH = -O3
OH_NO = -O0
#ifdef O
#CFLAGS += -O$(O)
#endif

main:
	$(MKDIRS)
	$(KANGAROO_BODY) $(OH_NO) -c $(VOLATILE_SOURCES) -o $(VOLATILE_OBJECT) $(LIBS)
	$(KANGAROO_BODY) $(VOLATILE_OBJECT) $(OH) $(OPTIMIZABLE_SOURCES) -o $(MAIN) $(LIBS)

dgen:
	mkdir -p bin
	$(KANGAROO_BODY) $(OH_NO) -c $(VOLATILE_SOURCES) -o $(VOLATILE_OBJECT) $(LIBS)
	$(KANGAROO_BODY) $(VOLATILE_OBJECT) $(OH) dgen/*.cpp $(GENERAL_SOURCES) -o $(DGEN) $(LIBS)
	./$(DGEN) ../data/ync.dat ../data/ynq.dat

dop:
	rm -f output/*

sgdb:
	$(MKDIRS)
	$(KANGAROO_BODY) $(OH_NO) -g -c $(VOLATILE_SOURCES) -o $(VOLATILE_OBJECT) $(LIBS)
	$(KANGAROO_BODY) -g $(VOLATILE_OBJECT) $(OH_NO) $(OPTIMIZABLE_SOURCES) -o $(MAIN) $(LIBS)
	sudo gdb ./$(MAINS)

gdb:
	$(MKDIRS)
	$(KANGAROO_BODY) $(OH_NO) -g -c $(VOLATILE_SOURCES) -o $(VOLATILE_OBJECT) $(LIBS)
	$(KANGAROO_BODY) -g $(VOLATILE_OBJECT) $(OH_NO) $(OPTIMIZABLE_SOURCES) -o $(MAIN) $(LIBS)
	gdb $(MAIN)

val:
	$(MAKE)
	valgrind --leak-check=full --show-leak-kinds=all ./$(MAIN) $(t) $(q)

clean:
	rm -rf bin/*

run:
	$(MAKE)
	./$(MAIN) $(t) $(q)

hist:
	$(MAKE)
	./$(MAIN) $(t) $(q)
	python main/gen_hist.py $(r1) $(r2)

.PHONY: main dbg clean dgen
