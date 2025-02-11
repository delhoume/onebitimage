
#include "miniz-3.0.2/miniz.h"
#include <cstdio>
extern "C" { 
    # include <miniz.h>
    # include <puff.h>
}

#include <stdio.h>

// clang -g -I. -Iminiz-3.0.2 minizunc.cpp miniz-3.0.2/miniz.c puff.c -o minizunc
// ./minizunc weather_good.zs weather_good.raw
int  main(int argc, char* argv[]) {
    FILE* fp = fopen(argv[1], "rb");
    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    unsigned char* compressed = (unsigned char*)malloc(size);
    fseek(fp, 2, SEEK_SET);
    fread(compressed, 1, size - 2, fp);
    fclose(fp);
    mz_ulong dsize = 100 * size;
    mz_ulong csize = size;
    unsigned char* uncompressed = (unsigned char*)malloc(100 * size);
  //  int status = mz_uncompress2(uncompressed, &dsize, compressed, &csize);
    int status = puff((unsigned char*)0,  &dsize,  compressed, &csize);
    fprintf(stdout, "status %d\n", status);
    fprintf(stdout, "size %lu\n", dsize);
    status = puff(uncompressed, &dsize,  compressed, &csize);
    fp = fopen(argv[2], "wb");
    fwrite(uncompressed, 1, dsize, fp);
    fclose(fp);
    return 1;
}
