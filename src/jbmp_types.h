#ifndef JBMP_TYPES_H
#define JBMP_TYPES_H

#include <inttypes.h>

// these structs do not use inttypes.h types, so that they can interface
// directly with pre-C99 code without recasting

typedef enum jbmp_rgb_t
{
  blue,
  green,
  red 
} jbmp_rgb_t;

// 3-channel pixel structure
typedef struct jbmp_pixel_t
{
  uint8_t b;
  uint8_t g;
  uint8_t r;
  
} jbmp_pixel_t;

// bitmap structure
// 'bitmap' is an array of 3-channel pixels
typedef struct jbmp_bitmap_t
{
  int width;
  int height;
  int size;        // in pixels
  int size_bytes;  //
  jbmp_pixel_t* bitmap;
  char* filename;

} jbmp_bitmap_t;

// the header we read in to verify and learn more about the .BMP file
// this struct uses inttypes.h types because specific bit-width is required.
typedef struct jbmp_header_t
{
  uint8_t magic[2];
  uint32_t size_of_bmp;
  uint32_t resd1;
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

#endif // RASTERBUF_H
