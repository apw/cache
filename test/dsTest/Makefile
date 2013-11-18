CLEAN = rm -f bin/dsTest
COMP = g++ -g -Werror src/intGen.c src/dsTest.c ../../ds/rset_uint.cpp ../../ds/uset_uint.cpp -o bin/dsTest
RUN = ./bin/dsTest
VALGRIND_COMMAND = valgrind --leak-check=full --show-reachable=yes
GDB_COMMAND = gdb

dsTest:
	$(CLEAN)
	$(COMP)

run:
	$(CLEAN)
	$(COMP)
	$(RUN)

valgrind:
	$(CLEAN)
	$(COMP)
	$(VALGRIND_COMMAND) $(RUN)

gdb:
	$(CLEAN)
	$(COMP)
	$(GDB_COMMAND) $(RUN)