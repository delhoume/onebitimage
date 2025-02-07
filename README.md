This repo hosts a simple and not definitive single header C library for handling
1 bit images in the PNG  format and in the Windows BMP format (as BitmapinfoHeaderv1)_

Usage:
```
#define ONEBIT_IMAGE_IMPLEMENTATION
#include "onebit.h
```

APIs:
```
int onebit_write_file_bmp1(const char *filename, int w, int h, unsigned char *data)\
int onebit_write_file_png1(const char *filename, int w, int h, unsigned char *data)
unsigned char* onebit_read_file_bmp1(const char *filename, int* w, int* h, unsigned char *data)
unsigned char* onebit_read_file_png1(const char *filename, int* w, int* h, unsigned char *data)
int onebit_bm_stride(int width) 
```

internal data format is Windows BMP 1 bit per pixel:
= height rows of *stride* bytes
= stride = width / 8 padded to 4 bytes ((width + 31) & ~31) >> 3)
+ image is bottom-top

Of particular interest might be:
- handling of BMP/PNG specifics
  - PNG is big endian
  - BMP is little-endian
  
- handling of BMP specifics
   - negative height (top-bottom) instead of default bottom-top
   - stride when width not aligned on 4 byt
 
- handling of PNG specifics
   - simple generation of only 3 chunks : IHDR IDAT IEND
  - deflate compression
     - depends on external minimal implementations
         - miniz (default)
         - puff (optional)
     - no dependency to zlib
  - non-trivial uncompressed data because of filter
  - crc checksum
 
- Due to lossless compression 1 bit PNGs are usually much lighter than 1 bit BMP.
  with ranges from 1/10 to 1


- A command-line utility is provided to transcode images from/to BMP/PNG
usage:
  > make
  > gen1bit imag1e.bmp
      -> generated.bmp -> uses read BMP / write BMP 
      -> generated.png -L> use read BMP _> write PNG

  > gen1bitv image.png
      -> same  thing, starting from on a PNG source
  
  > Developement is done on a mac m1 but code should be portable.
