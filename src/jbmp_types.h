#ifndef JBMP_TYPES_H
#define JBMP_TYPES_H

// these structs do not use inttypes.h types, so that they can interface
// directly with pre-C99 code without recasting

// 3-channel pixel structure
typedef struct jbmp_pixel_t
{
  unsigned char b;
  unsigned char g;
  unsigned char r;
} jbmp_pixel_t;

// bitmap structure
// 'bitmap' is an array of 3-channel pixels
typedef struct jbmp_bitmap_t
{
  unsigned int width;
  unsigned int height;
  unsigned char bpp;
  unsigned char bytes_per_pixel;
  unsigned int image_size_bytes;
  jbmp_pixel_t* bitmap;
} jbmp_bitmap_t;





#endif // RASTERBUF_H
