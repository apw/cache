CLEAN = rm -f bin/dsTest
COMP = g++ -g -Werror src/intGen.c src/dsTest.c ../../ds/rset_uint.cpp -o bin/dsTest
RUN = ./bin/dsTest

dsTest:
	$(CLEAN)
	$(COMP)

run:
	$(CLEAN)
	$(COMP)
	$(RUN)