all: gen1bit


# puff is optional

gen1bit: gen1bit.o miniz.o puff.o
	clang -o gen1bit gen1bit.o miniz.o puff.o -lstdc++

gen1bit.o: gen1bit.cpp
	g++ -I. -Iminiz-3.0.2 -c gen1bit.cpp

miniz.o: miniz-3.0.2/miniz.c
	gcc -I. -Iminiz-3.0.2 -c miniz-3.0.2/miniz.c

puff.o: puff.c
	gcc -I. -c puff.c
	
clean:
	rm -rf *.o gen1bit