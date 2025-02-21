
CCC=clang
CC=gcc

all: gen1bit


# puff is optional

gen1bit: gen1bit.o miniz.o
	$(CCC) -o gen1bit gen1bit.o miniz.o -lstdc++

gen1bit.o: gen1bit.cpp onebit.h
	$(CCC) -I. -Iminiz-3.0.2 -c -g gen1bit.cpp

miniz.o: miniz-3.0.2/miniz.c
	gcc -I. -Iminiz-3.0.2 -g -c miniz-3.0.2/miniz.c

testit: gen1bit
	./gen1bit testimages/waveshare.bmp	
	./gen1bit testimages/waveshare.bmp

	./gen1bit testimages/indigo.bmp
	./gen1bit testimages/indigo.png	

	./gen1bit testimages/space_800_480.bmp

clean:
	rm -rf generated/*.bmp generated/*.png *.o gen1bit