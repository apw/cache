CFLAGS = -g -Wall -Werror -I /opt/local/include
LDFLAGS = -g -L /opt/local/lib -lgsl -lgslcblas -lcrypto -lm
ZCAT = gzcat

ifdef O
CFLAGS += -O$(O)
LDFLAGS += -O$(O)
endif

oracle: congruent.o search.o help.o main.o
oracle: sgn.o bit.o walk.o tree.o solve.o dotty.o fdp.o
	$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c rdtsc.h tunnel.h vertex.h oracle.h
	$(CC) $< $(CFLAGS) -c -o $@

c.dat: c.dat.gz
	$(ZCAT) c.dat.gz | head -n 128 > c.dat

q.dat: q.dat.gz
	$(ZCAT) q.dat.gz | head -n 1024 > q.dat

.PHONY: clean check

clean:
	$(RM) $(wildcard oracle *.o *.[ch]~ *.ok *.orig *.dot *.pdf *.svg)

check: clean c.dat q.dat oracle
	sha256sum c.dat q.dat > check.ok
	mv c.dat c.dat.orig
	mv q.dat q.dat.orig
	./oracle -1 -c c.dat.orig > c.dat
	./oracle -6 -q q.dat.orig > q.dat
	sha256sum -c check.ok
	./oracle -r -c c.dat -q q.dat | grep -c ^1 | grep -q 600
	./oracle -c c.dat -q q.dat | grep -c ^1 | grep -q 600
