all: gen1bit


# puff is optional

gen1bit: gen1bit.o miniz.o
	clang -o gen1bit gen1bit.o miniz.o -lstdc++

gen1bit.o: gen1bit.cpp onebit.h
	g++ -I. -Iminiz-3.0.2 -c gen1bit.cpp

miniz.o: miniz-3.0.2/miniz.c
	gcc -I. -Iminiz-3.0.2 -c miniz-3.0.2/miniz.c

clean:
	rm -rf *.o gen1bit