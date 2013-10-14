CFLAGS = -g -Wall -Werror
LDFLAGS = -g

bench: congruent.o search.o tunnel.o bench.o
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c rdtsc.h tunnel.h
	$(CC) $< $(CFLAGS) -c -o $@

clean:
	$(RM) $(wildcard bench *.o *.c~)
