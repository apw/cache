dsTest
======

WHAT: A simple testing framework for int-typed data structures in C.

WHY: I often find myself writing similar test-code over and over again for testing hand-made data structures in C. Usually I write them typed as ints to start with then use some preprocessing or sed/awk magic to get type-abstraction. Thus, it would be nice for me to be able to reuse my test code on these int-typed data structures.

WHERE: The layout of the project is fairly simple:
./bin: the directory containing binaries; what you actually want to run
./src: the directory containing source (.c) files; where the framework code lives
./include: the directory containing include (.h) files; a good place to see the interface exposed by the framework

HOW: To use this framework:

1) put your data structure code in ./src (or anywhere accessible).

2) write your test driver in the main() function of ./src/dsTest.c

3) run make

4) run ./bin/dsTest