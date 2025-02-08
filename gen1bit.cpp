#include <cstdio>
#define ONEBIT_IMAGE_IMPLEMENTATION
#include "onebit.h"

#include <string>

bool ends_with(const std::string &s, const std::string &suffix) {
  return s.rfind(suffix) == s.length() - suffix.length();
}

// clang -g  -I. -Iminiz-3.0.2 gen1bit.cpp miniz-3.0.2/miniz.c puff.c -o gen1bit
// -lstdc++

int main(int argc, char *argv[]) {
  int w = 0;
  int h = 0;
  int stride = 0;
  unsigned char *data = nullptr;

  std::string fname((argc > 1) ? argv[1] : "indigo.png");
  std::string png_ext("png");
  std::string bmp_ext("bmp");

  if (ends_with(fname, bmp_ext)) {
    data = onebit_read_file_bmp1(fname.c_str(), &w, &h);
  } else if (ends_with(fname, png_ext)) {
    data = onebit_read_file_png1(fname.c_str(), &w, &h);
  }
  onebit_write_file_png1("generated.png", w, h, data);
  onebit_write_file_bmp1("generated.bmp", w, h, data);

  free(data);

  // try from memory
  FILE *ff = fopen(argv[1], "rb");
  fseek(ff, 0, SEEK_END);
  int ffsize = ftell(ff);
  fseek(ff, 0, SEEK_SET);
  unsigned char *filedata = (unsigned char *)malloc(ffsize);
  fread(filedata, 1, ffsize, ff);
  fclose(ff);
  data = onebit_read_memory_bmp1(filedata, &w, &h, &stride);
  if (data != nullptr) {
    onebit_write_file_png1("generated_from_mem_bmp.png", w, h, data);
    onebit_write_file_bmp1("generated_from_mem_bmp.bmp", w, h, data);
  }
  free(data);
  return 1;
}