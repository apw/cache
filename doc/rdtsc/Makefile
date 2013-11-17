all: cold warm hot step

cold: cold.o
	$(CC) -g $< -o $@

warm: warm.o
	$(CC) -g $< -o $@

hot: hot.o
	$(CC) -g $< -o $@

step: step.o
	$(CC) -g $< -o $@

%.o: %.c
	$(CC) -g -c $< -o $@

clean:
	$(RM) -r $(wildcard cold warm hot step *.o *.c~ *.dSYM)
