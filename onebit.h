#ifndef INCLUDE_ONEBIT_IMAGE_H
#define INCLUDE_ONEBIT_IMAGE_H

#include <cstdio>

extern "C" {
#include <miniz.h>
}

#include <stdlib.h>

// clang -g -I. -Iminiz-3.0.2 onebit_image_demo.cpp miniz-3.0.2/miniz.c puff.c
// -o onebit_image_dem

// if STB_IMAGE_WRITE_STATIC causes problems, try defining STBIWDEF to 'inline'
// or 'static inline'
#ifndef ONEBITWDEF
#ifdef ONEBIT_IMAGE_STATIC
#define ONEBITWDEF static
#else
#ifdef __cplusplus
#define ONEBITWDEF extern "C"
#else
#define ONEBITWDEF extern
#endif
#endif
#endif

unsigned char *onebit_read_file_bmp1(const char *filename, int *w, int *h);
unsigned char* onebit_read_file_png1(const char *filename, int *w, int *h);
int onebit_write_file_bmp1(const char *filename, int w, int h, const unsigned char* data);
int onebit_write_file_png1(const char *filename, int w, int h, const unsigned char *data);

#endif // INCLUDE_ONEBIT_IMAGE_H

#define ONEBIT_IMAGE_IMPLEMENTATION

#ifdef ONEBIT_IMAGE_IMPLEMENTATION

#ifdef _WIN32
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE
#endif
#endif

#ifndef STBI_WRITE_NO_STDIO
#include <stdio.h>
#endif // STBI_WRITE_NO_STDIO
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#if defined(STBIW_MALLOC) && defined(STBIW_FREE) &&                            \
    (defined(STBIW_REALLOC) || defined(STBIW_REALLOC_SIZED))
// ok
#elif !defined(STBIW_MALLOC) && !defined(STBIW_FREE) &&                        \
    !defined(STBIW_REALLOC) && !defined(STBIW_REALLOC_SIZED)
// ok
#else
#error                                                                         \
    "Must define all or none of STBIW_MALLOC, STBIW_FREE, and STBIW_REALLOC (or STBIW_REALLOC_SIZED)."
#endif

#ifndef STBIW_MALLOC
#define STBIW_MALLOC(sz) malloc(sz)
#define STBIW_REALLOC(p, newsz) realloc(p, newsz)
#define STBIW_FREE(p) free(p)
#endif

#ifndef STBIW_REALLOC_SIZED
#define STBIW_REALLOC_SIZED(p, oldsz, newsz) STBIW_REALLOC(p, newsz)
#endif

#ifndef STBIW_MEMMOVE
#define STBIW_MEMMOVE(a, b, sz) memmove(a, b, sz)
#endif

// if system is little endian  ? ...

void writeLittle16(FILE *fp, uint16_t value) {
  fputc(value & 0xff, fp);
  fputc(value >> 8, fp);
}

uint16_t readLittle16(FILE *fp) { return fgetc(fp) + fgetc(fp) * 256; }

void write0_16(FILE *fp) {
  fputc(0, fp);
  fputc(0, fp);
}
void write0_32(FILE *fp) {
  fputc(0, fp);
  fputc(0, fp);
  fputc(0, fp);
  fputc(0, fp);
}

void writeLittle32(FILE *fp, uint32_t value) {
  unsigned char c;
  c = value & 0xff;
  fputc(c, fp);
  c = (value >> 8) & 0xff;
  fputc(c & 0xff, fp);
  c = (value >> 16) & 0xff;
  fputc(c, fp);

  c = (value >> 24) & 0xff;
  fputc(c, fp);
}

uint32_t readLittle32(FILE *fp) {
  return fgetc(fp) | ((uint32_t)fgetc(fp) << 8) | ((uint32_t)fgetc(fp) << 16) |
         ((uint32_t)fgetc(fp) << 24);
}

void writeBig16(FILE *fp, uint16_t value) {
  fputc(value >> 8, fp);
  fputc(value & 0xff, fp);
  fflush(fp);
}

uint16_t readBig16(FILE *fp) { return fgetc(fp) * 256 + fgetc(fp); }

void writeBig32(FILE *fp, uint32_t value) {
  unsigned char c;
  c = (value >> 24) & 0xff;
  fputc(c, fp);
  c = (value >> 16) & 0xff;
  fputc(c, fp);
  c = (value >> 8) & 0xff;
  fputc(c & 0xff, fp);
  c = value & 0xff;
  fputc(c, fp);
  fflush(fp);
}

void writeBig32Mem(unsigned char *data, uint32_t value) {
  data[0] = (value >> 24) & 0xff;
  data[1] = (value >> 16) & 0xff;
  data[2] = (value >> 8) & 0xff;
  data[3] = value & 0xff;
}

uint32_t readBig32(FILE *fp) {
  return ((uint32_t)fgetc(fp) << 24) | ((uint32_t)fgetc(fp) << 16) |
         ((uint32_t)fgetc(fp) << 8) | fgetc(fp);
}

void writeN(FILE *fp, const unsigned char *data, int n) { fwrite(data, 1, n, fp); }
void writeNMem(unsigned char *dataout, const unsigned char *datain, int n) {
  memcpy(dataout, datain, n);
}

void write1(FILE *fp, unsigned char value) { fputc(value, fp); }
void write1Mem(unsigned char *dataout, unsigned char value) {
  dataout[0] = value;
}

void skipN(FILE *fp, int n) {
  for (int i = 0; i < n; ++n)
    fgetc(fp);
}

inline void skip1(FILE *fp) { fgetc(fp); }

inline void skip2(FILE *fp) {
  fgetc(fp);
  fgetc(fp);
}

inline void skip4(FILE *fp) {
  fgetc(fp);
  fgetc(fp);
  fgetc(fp);
  fgetc(fp);
}

void readN(FILE *fp, unsigned char *data, int n) { fread(data, 1, n, fp); }

unsigned char read1(FILE *fp) { return fgetc(fp); }

int onebit_bmp_stride(int width) { return ((width + 31) & ~31) >> 3; }

#if 0
//  BMP V1 
struct BitmapInfoHeaderV1 {
  uint32_t biSize;
  int32_t biWidth;
  int32_t biHeight;
  uint16_t biPlanes;
  uint16_t biBitCount;
  uint32_t biCompression;
  uint32_t biSizeImage;
  int32_t biXPelsPerMeter;
  int32_t biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
};

struct BitmapFileHeader {
  unsigned char bfType[2];
  int32_t bfSize;
  int16_t bfReserved1;
  int16_t bfReserved2;
  int32_t bfOffBits;
};

struct BMPHeader {
  struct BitmapFileHeader bfh;
  struct BitmapInfoHeaderV1 bih;
};

#endif

typedef struct BGRA {
  Byte b; Byte g; Byte r; Byte a;
} BGRA;


unsigned char *onebit_read_file_bmp1(const char *filename, int *w, int *h) {
  FILE *fp = fopen(filename, "rb");
  unsigned char *data = nullptr;
  unsigned char b0 = read1(fp);
  unsigned char b1 = read1(fp);
  if ((b0 != 'B') || (b1 != 'M'))
    return data;
  skip4(fp); // biF
  skip2(fp);
  skip2(fp); // reserved
  uint32_t offset = readLittle32(fp);

  uint32_t bih_size = readLittle32(fp);
  // 108 bitmapv4header
  // if (bih_size != 40) return data;
  int32_t width = readLittle32(fp);
  int32_t height = readLittle32(fp);
  uint16_t color_planes = readLittle16(fp);
  uint16_t bits_per_pixel = readLittle16(fp);
  uint32_t compression = readLittle32(fp);
  uint32_t raw_image_size = readLittle32(fp);
  // uint32_t vres = readLittle32(fp);
  // uint32_t hres = readLittle32(fp);
  // uint32_t num_colors = readLittle32(fp);
  // if (num_colors == 0)
  //   num_colors = 1 << bits_per_pixel;
  // uint32_t important_colors = readLittle32(fp);

  // for (int c = 0; c < num_colors; ++c) {
  //   skip4(fp);
  // }

  fseek(fp, offset, SEEK_SET);
  uint32_t stride = onebit_bmp_stride(width);
  data = (unsigned char *)malloc(stride * height);
  readN(fp, data, stride * height);
  *w = width;
  *h = height;
  return data;
}

const int png_signature_length = 8;
unsigned char png_signature[png_signature_length] = {137, 80, 78, 71,
                                                     13,  10, 26, 10};
const int full_chunk_IHDR_len = 25;

unsigned char chunk_IHDR[4] = {'I', 'H', 'D', 'R'};
unsigned char chunk_IDAT[4] = {'I', 'D', 'A', 'T'};
unsigned char chunk_IEND[4] = {'I', 'E', 'N', 'D'};

const int full_chunk_IEND_len = 12;
unsigned char full_chunk_IEND[full_chunk_IEND_len] = {
    0x00, 0x00, 0x00, 0x0, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82};

inline bool match_known(unsigned char *bytes, unsigned char *known, int len) {
  return !memcmp(bytes, known, len);
}

int onebit_write_file_png1(const char *filename, int w, int h,
                           const unsigned char *data) {
  unsigned char tmp_big32[4] = {0};
  FILE *fp = fopen(filename, "wb");

  writeN(fp, png_signature, 8);
  // ihdr
  writeBig32(fp, 13);
  unsigned char ihdr[17];
  unsigned char *ptr = ihdr;

  writeNMem(ptr, chunk_IHDR, 4);
  ptr += 4;
  writeBig32Mem(ptr, w);
  ptr += 4;
  writeBig32Mem(ptr, abs(h));
  ptr += 4;
  write1Mem(ptr, 1);
  ptr++; // bits per pixel
  write1Mem(ptr, 0);
  ptr++; // colortype (greyscale)
  write1Mem(ptr, 0);
  ptr++; // compression
  write1Mem(ptr, 0);
  ptr++; // filter
  write1Mem(ptr, 0);
  ptr++; // interlace

  unsigned int ihdr_crc = mz_crc32(MZ_CRC32_INIT, ihdr, 17);
  writeN(fp, ihdr, 17);
  writeBig32(fp, ihdr_crc);

  // idat
  int stride = onebit_bmp_stride(w);

  // FILTER adds  byte at the begining !!!
  mz_ulong data_len = (stride + 1) * h;
  unsigned char *newdata = (unsigned char *)malloc(data_len);
  // invert a the same time

  for (int y = 0; y < abs(h); y++) {
    newdata[y * (stride + 1)] = 0;
    memcpy(newdata + y * (stride + 1) + 1,
           h <= 0 ? data + y * stride : data + (abs(h) - y - 1) * stride,
           stride);
  }

  mz_ulong compressed_data_len = mz_compressBound(data_len);
  unsigned char *compressed_data =
      (unsigned char *)malloc(compressed_data_len); // same as uncompress

  int status =
      compress2(compressed_data, &compressed_data_len, newdata, data_len, 7);
  int cdata_len = compressed_data_len;

  uint32_t scdata_len = cdata_len;
  free(newdata);

  writeBig32(fp, cdata_len);
  writeN(fp, chunk_IDAT, 4);
  writeN(fp, compressed_data, cdata_len);
  unsigned int idat_crc = mz_crc32(MZ_CRC32_INIT, chunk_IDAT, 4);
  idat_crc = mz_crc32(idat_crc,  compressed_data, cdata_len);

  writeBig32(fp, idat_crc);

  // iend
  writeN(fp, full_chunk_IEND, full_chunk_IEND_len);

#if defined(DEBUG_IDAT)
  FILE *rawdata = fopen("rawdata.zs", "wb");
  fwrite(rawdata, 1, cdata_len, rawdata);
  fclose(rawdata);
#endif
  free(compressed_data);
  fclose(fp);
  return 1;
}

unsigned char* onebit_read_file_png1(const char *filename, int *w, int *h) {
  FILE *fp = fopen(filename, "rb");
  unsigned char signature[8] = {0};
  unsigned char *data = nullptr;
  unsigned char *compressed_data = nullptr;
  int compressed_data_pos = 0;
  int compressed_data_size = 0;
  int width = 0;
  int height = 0;
  int stride = 0;
  readN(fp, signature, png_signature_length);
  if (!match_known(signature, png_signature, png_signature_length)) {
    return data;
  }
  // read chunks
  bool reading_chunks = true;
  do {
    unsigned char chunk[4] = {0};
    int chunk_length = readBig32(fp);
    readN(fp, chunk, 4);
    int pos = ftell(fp);
    if (match_known(chunk, chunk_IHDR, 4)) {
      width = readBig32(fp);
      height = readBig32(fp); // we will invert later

      int bits_per_pixel = read1(fp); // 1
      int color_type = read1(fp);     // 0 grayscale
      int compression = read1(fp);    // 0 zlib
      int filter_method = read1(fp);  // 0 none
      int interlacing = read1(fp);    // 0 none 1 adam7

      stride = onebit_bmp_stride(width);
      *w = width;
      *h = height;
    } else if (match_known(chunk, chunk_IDAT, 4)) {
      compressed_data_size += chunk_length;
      if (compressed_data == nullptr) {
        // TODO check if miniz allows to decode as we read instead of reading the whole compressed contents
        compressed_data = (unsigned char *)malloc(compressed_data_size);
      } else {
        compressed_data =
            (unsigned char *)realloc(compressed_data, compressed_data_size);
      }
      readN(fp, compressed_data + compressed_data_pos, chunk_length);
      compressed_data_pos += chunk_length;
    } else if (match_known(chunk, chunk_IEND, 4)) {
      reading_chunks = false;
    }
    fseek(fp, pos + chunk_length, SEEK_SET);
    unsigned int crc = readBig32(fp);
  } while (reading_chunks);
  fclose(fp);
  unsigned long srclen = compressed_data_size;
  unsigned long dstlen = (stride + 1) * height;
  // these two allocations can be reduced to the largest one,
  // the returned data would have height "wasted" bytes ate the end
  // second pass would move data to correct location "in place"
  unsigned char *tmp_data = (unsigned char *)malloc(srclen);
  int status = uncompress2(tmp_data, &dstlen, compressed_data, &srclen);
  free(compressed_data);
  data = (unsigned char *)malloc(stride * height);
  // invert
  for (int y = 0; y < height; ++y) {
    memcpy(data + ((height - y - 1) * stride),
           tmp_data + (y * (stride + 1)) + 1, stride);
  }
  free(tmp_data);
#if defined(DEBUG_IDAT)
  onebit_write_file_bmp1("pngdecoded.bmp", width, height, data);
#endif
  return data;
}

int onebit_write_file_bmp1(const char *filename, int w, int h,
                               const unsigned char* data) {
  FILE *fp = fopen(filename, "wb");
  int ncolors = 2;
  int stride = onebit_bmp_stride(w);
  unsigned int absolute_height = abs(h);
  bool is_top_down = h < 0;
  write1(fp, 'B');
  write1(fp, 'M');
  writeLittle32(fp, 54 + ncolors * 4 + stride * h); // filesize
  write0_16(fp);
  write0_16(fp);
  writeLittle32(fp, 62);

  writeLittle32(fp, 40);
  writeLittle32(fp, w);
  writeLittle32(fp, h);
  writeLittle16(fp, 1);
  writeLittle16(fp, 1);
  write0_32(fp); // compression
  writeLittle32(fp, stride * abs(h));
  write0_32(fp);        // h res
  write0_32(fp);        // v res
  writeLittle32(fp, 2); //
  writeLittle32(fp, 2); //

  // TODO: check on TRML device is 0 is black or white
  unsigned char black[4] = {0x00, 0x00, 0x00, 0x00};
  unsigned char white[4] = {0xff, 0xff, 0xff, 0x00};

  writeN(fp, black, 4);
  writeN(fp, white, 4);
  writeN(fp, data, stride * abs(h));
  fclose(fp);
  return 1;
}
#endif