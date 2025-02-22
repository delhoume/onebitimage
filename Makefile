
CCC=clang  
CC=gcc

DEF_OPTIONS= -std=c++17
OPT_OPTIONS=$(DEF_OPTIONS) 
-DBENCH -O2
DBG_OPTIONS=$(DEF_OPTIONS) -g

OPTIONS=$(OPT_OPTIONS)


all: gen1bit.bench gen1bit.dbg


gen1bit.bench: gen1bit.bench.o miniz.o
	$(CCC) -o gen1bit.bench gen1bit.bench.o miniz.o -lstdc++

gen1bit.bench.o: gen1bit.cpp onebit.h
	$(CCC) $(OPT_OPTIONS) -I. -Iminiz-3.0.2 -c gen1bit.cpp -o gen1bit.bench.o

gen1bit.dbg: gen1bit.dbg.o miniz.o
	$(CCC) -o gen1bit.dbg gen1bit.dbg.o miniz.o -lstdc++

gen1bit.dbg.o: gen1bit.cpp onebit.h
	$(CCC) $(DBG_OPTIONS) -I. -Iminiz-3.0.2 -c gen1bit.cpp -o gen1bit.dbg.o

miniz.o: miniz-3.0.2/miniz.c
	gcc -I. -Iminiz-3.0.2 -g -c miniz-3.0.2/miniz.c

testit: gen1bit.bench
	./gen1bit.bench testimages/waveshare.bmp	
	./gen1bit.bench testimages/waveshare.bmp

	./gen1bit.bench testimages/indigo.bmp
	./gen1bit.bench testimages/indigo.png	

	./gen1bit.bench testimages/space_800_480.bmp

clean:
	rm -rf generated/*.bmp generated/*.png *.o gen1bit.bench gen1bit.dbg