CC = g++-4.7
CFLAGS = -Wall -Werror

OPTIMIZABLE_SOURCES = main/*.cpp algo/*.cpp ds/*.cpp
OPTIMIZABLE_OBJECT = bin/opt.o
VOLATILE_SOURCES = arch/*.c
VOLATILE_OBJECT = bin/vol.o
HEADERS = includes/*.h includes/*.hpp
EXECUTABLE = bin/main

KANGAROO_BODY = $(CC) $(CFLAGS)
#KANGAROO_TAIL = $(SOURCES) -o $(EXECUTABLE)
MKDIRS = mkdir -p bin graphs output 

OH = -O3
OH_NO = -O0
#ifdef O
#CFLAGS += -O$(O)
#endif

main:
	$(MKDIRS)
	$(KANGAROO_BODY) $(OH_NO) -c $(VOLATILE_SOURCES) -o $(VOLATILE_OBJECT)
	$(KANGAROO_BODY) $(VOLATILE_OBJECT) $(OH) $(OPTIMIZABLE_SOURCES) -o $(EXECUTABLE)

dop:
	rm -f output/*

sgdb:
	$(MKDIRS)
	$(KANGAROO_BODY) $(OH_NO) -g -c $(VOLATILE_SOURCES) -o $(VOLATILE_OBJECT)
	$(KANGAROO_BODY) -g $(VOLATILE_OBJECT) $(OH_NO) $(OPTIMIZABLE_SOURCES) -o $(EXECUTABLE)
	sudo gdb $(EXECUTABLES)

gdb:
	$(MKDIRS)
	$(KANGAROO_BODY) $(OH_NO) -g -c $(VOLATILE_SOURCES) -o $(VOLATILE_OBJECT)
	$(KANGAROO_BODY) -g $(VOLATILE_OBJECT) $(OH_NO) $(OPTIMIZABLE_SOURCES) -o $(EXECUTABLE)
	gdb $(EXECUTABLE)

clean:
	rm -rf bin/*

run:
	$(MAKE)
	./$(EXECUTABLE) $(t) $(q)

hist:
	$(MAKE)
	./$(EXECUTABLE) $(t) $(q)
	python main/gen_hist.py $(r1) $(r2)

.PHONY: main dbg clean
