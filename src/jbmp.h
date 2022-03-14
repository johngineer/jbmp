#ifndef JBMP_H
#define JBMP_H

#include <inttypes.h>
#include "jbitmap.h"
#include "jbmp_types.h"

#define JBMP_ERR_FILE_NOT_EXIST         -1
#define JBMP_ERR_BAD_FORMAT             -2
#define JBMP_ERR_SIZE_MISMATCH          -3
#define JBMP_ERR_BAD_BPP                -4
#define JBMP_ERR_BITMAP_TOO_BIG         -5

#define JBMP_MAGIC_NUMBER               "BM"

#define JBMP_MAX_BITMAP_SIZE            0x0FFFFFFF

// the header we read in to verify and learn more about the .BMP file
// this struct uses inttypes.h types because specific bit-width is required.
typedef struct jbmp_header_t
{
  char magic[2];
  uint32_t size_of_bmp;
  uint16_t resd1;
  uint16_t resd2;
  uint32_t bitmap_offset;
  uint32_t size_of_header;
  uint32_t width;
  uint32_t height;
  uint16_t cplanes; // must == 1
  uint16_t bpp;
  uint32_t comp_method;

  // we're not really interested in these values but we have them
  // in here to make the header struct the correct size;
  uint32_t image_size;
  uint32_t x_pixels_per_m;
  uint32_t y_pixels_per_m;
  uint32_t colors_used;
  uint32_t important_colors;
} jbmp_header_t;



int init_bitmap_struct(jbitmap_t* bitmap, jbmp_header_t* header);

int jbmp_read_file_header(FILE* f, jbmp_header_t* header);

int jbmp_read_file_bitmap(FILE* f, jbmp_header_t header, jbitmap_t* bitmap);

int jbmp_read_bmp_file(const char* fname, jbitmap_t* bitmap);


// TODO: write functions
int jbmp_write_file_header(FILE* f, jbmp_header_t* h);

int jbmp_write_file_bitmap(FILE* f, jbitmap_t* b);

int jbmp_write_bmp_file(const char* fname, jbmp_header_t* header, jbitmap_t* bitmap);

#endif // JBMP_H
