CFLAGS = -g -Wall -Werror
LDFLAGS = -g

ifdef O
CFLAGS += -O$(O)
endif

bench: congruent.o search.o bench.o
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c rdtsc.h tunnel.h
	$(CC) $< $(CFLAGS) -c -o $@

.PHONY: clean

clean:
	$(RM) $(wildcard bench *.o *.c~)
