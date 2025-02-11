#define ONEBIT_IMAGE_IMPLEMENTATION
#include "onebit.h"

#define TEST_ALL

#include <cstdio>
#include <string>

using namespace std;

bool ends_with(const std::string &s, const std::string &suffix) {
  return s.rfind(suffix) == s.length() - suffix.length();
}

// clang -g  -I. -Iminiz-3.0.2 gen1bit.cpp miniz-3.0.2/miniz.c -o gen1bit
// -lstdc++

unsigned char *readFileContents(const char *filename, int *size) {
  FILE *ff = fopen(filename, "rb");
  if (ff == nullptr) {
    size = 0;
    return nullptr;
  }
  fseek(ff, 0, SEEK_END);
  int ffsize = ftell(ff);
  fseek(ff, 0, SEEK_SET);
  unsigned char *filedata = (unsigned char *)malloc(ffsize);
  fread(filedata, 1, ffsize, ff);
  fclose(ff);
  *size = ffsize;
  return filedata;
}

int main(int argc, char *argv[]) {
  int w = 0;
  int h = 0;
  int stride = 0;
  if (argc < 2)
    return 0;

  unsigned char *data = onebit_read_file(argv[1], &w, &h);
  if (data == nullptr)
    return 0;
  onebit_write_file_png1("generated/generated.png", w, h, data);
  onebit_write_file_bmp1("generated/generated.bmp", w, h, data);

  free(data);


#if defined(TEST_ALL)

  string fname((argc > 1) ? argv[1] : "indigo.png");
  std::string png_ext("png");
  std::string bmp_ext("bmp");
 w = 0; h = 0; stride = 0;
  int size = 0;
  unsigned char *filedata = readFileContents(fname.c_str(), &size);

  if (ends_with(fname, bmp_ext)) {
    data = onebit_read_mem_bmp1(filedata, size, &w, &h, &stride);
    if (data != nullptr) {
      onebit_write_file_png1("generated/generated_read_mem_bmp.png", w, h,
                             data);
      onebit_write_file_bmp1("generated/generated_read_mem_bmp.bmp", w, h,
                             data);
      size = 0;
      unsigned char *mem_bmp1 = onebit_write_mem_bmp1(w, h, data, &size);
      writeToFile("generated/generated_write_mem_bmpopen.bmp", mem_bmp1, size);
      free(mem_bmp1);
      size = 0;
      unsigned char *mem_png1 = onebit_write_mem_png1(w, h, data, &size);
      writeToFile("generated/generated_write_mem_bmp.png", mem_png1, size);
      free(mem_png1);
    }
    free(data);
  } else if (ends_with(fname, png_ext)) {
    data = onebit_read_mem_png1(filedata, size, &w, &h, &stride);
    if (data != nullptr) {
      onebit_write_file_png1("generated/generated_read_mem_png.png", w, h,
                             data);
      onebit_write_file_bmp1("generated/generated_read_mem_png.bmp", w, h,
                             data);
      size = 0;
      unsigned char *mem_bmp1 = onebit_write_mem_bmp1(w, h, data, &size);
      writeToFile("generated/generated_write_mem_png.bmp", mem_bmp1, size);
      free(mem_bmp1);
      unsigned char *mem_png1 = onebit_write_mem_png1(w, h, data, &size);
      writeToFile("generated/generated_write_mem_png.png", mem_png1, size);
      free(mem_png1);
    }
    free(data);
  }
  free(filedata);
#endif
  return 0;
}