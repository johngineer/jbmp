// jbmp.c

/*
jbmp :: a simple library to read and write .bmp image files

2022 by j. m. de cristofaro ("johngineer")

note: this library currently only reads uncompressed, 24bpp RGB images.

TODO: add support for 1-bit and greyscale
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jbmp.h"
//#include "jdbg.h"

// FILE OPS
int jbmp_read_file_header(FILE* f, jbmp_header_t* header)
{
  int fp = ftell(f);
  fread(&header->magic[0], 1, 1, f);
  fread(&header->magic[1], 1, 1, f);
  fread(&header->size_of_bmp, 4, 1, f);
  fread(&header->resd1, 4, 1, f);
  //fread(&header->resd2, 2, 1, f);
  fread(&header->bitmap_offset, 4, 1, f);
  fread(&header->size_of_header, 4, 1, f);

  if (header->size_of_header >= 40)   // indicates newer header
  {
    fread(&header->width, 4, 1, f);
    fread(&header->height, 4, 1, f);
    fread(&header->cplanes, 2, 1, f);
    fread(&header->bpp, 2, 1, f);
    fread(&header->comp_method, 4, 1, f);
  }
  else
  {
    fread(&header->width, 2, 1, f);
    fread(&header->height, 2, 1, f);
    fread(&header->cplanes, 2, 1, f);
    fread(&header->bpp, 2, 1, f);
  }

  fp = ftell(f) - fp;
  
  #ifdef JBMP_DEBUG
  printf("BMP header:\n");
  printf("header->magic = '%c%c'\n", header->magic[0], header->magic[1]);
  //printf("header->magic = 0x%04X\n", header->magic);
  printf("header->size_of_bmp = %i\n", header->size_of_bmp);
  printf("header->resd1 = %i\n", header->resd1);
  //printf("header->resd2 = %i\n", header->resd2);
  printf("header->bitmap_offset = %i\n", header->bitmap_offset);
  printf("header->size_of_header = %i\n", header->size_of_header);
  printf("header->width = %i\n", header->width);
  printf("header->height = %i\n", header->height);
  printf("header->cplanes = %i\n", header->cplanes);
  printf("header->bpp = %i\n", header->bpp);
  printf("header->comp_method = %i\n", header->comp_method);
  printf("\n");
  #endif

  return fp;
}

int jbmp_read_file_bitmap(FILE* f, jbmp_header_t header, jbmp_bitmap_t* bitmap)
{
  int row_size_bytes = (((header.width*3)+3)/4) * 4;
  int row_pad_bytes = row_size_bytes - (header.width*3);

  int a = 0;
  int j, k;
  int update = header.height / 4;
  int line = 0;
  jbmp_pixel_t p;

  // BMP files store rows from the bottom to the top, so we flip the direction
  // of the j counter
  for (j = header.height-1; j >= 0; j--)
  {
    for (k = 0; k < header.width; k++)
    {
      a += fread(&p.b, 1, 1, f);
      a += fread(&p.g, 1, 1, f);
      a += fread(&p.r, 1, 1, f);
      jbmp_set_pixel(bitmap, k, j, p);
    }
    fseek(f, row_pad_bytes, SEEK_CUR);
    line++;
  }
  return a;
}

int jbmp_read_bmp_file(const char* fname, jbmp_bitmap_t* bitmap)
{
  FILE* f;
  jbmp_header_t header;
  unsigned long fp;
  char file_exists = 0;

  // check if the file exists
  if (f = fopen(fname, "r"))
  {
    file_exists = 1;
  }
  else
  {
    #ifdef JBMP_DEBUG
    fprintf(stderr, "ERROR: file '%s' does not exist!\n", fname);
    #endif
    return JBMP_ERR_FILE_NOT_EXIST;
  }

  // file exists, so read the header
  // jd_tag(fsio_open,1,"opened '%s' for reading", fname);
  int c = jbmp_read_file_header(f, &header);
  // jd_tag(fsio_in,1,"read %i bytes from '%s'",c, fname);
  // jd_tag(fsio_close,1,"close '%s'", fname);
  // jd_tag(mesg,1,"read BMP file header. verifying...", c);
  fclose(f);   // close the file while we check the validity

  // now that we've read the header, let's verify it's a real .BMP file.
  if (header.magic[0] != 'B' || header.magic[1] != 'M')
  //if (header.magic == JBMP_MAGIC_NUMBER)
  {
    // jd_tag(error,1,"'%s' -- bad magic number '%s'", fname, header.magic);
    return JBMP_ERR_BAD_FORMAT;
  }
  else if (header.bpp != 24) // we can only read 24bpp images.
  {
    // jd_tag(error,1,"'%s' -- cannot read %ibpp bitmap", fname, header.bpp);
    return JBMP_ERR_BAD_BPP;
  }
  // else { jd_tag(mesg,1,"BMP header verified."); }


  // avoid memory overflows
  unsigned int size_of_bitmap = 3 * (header.width * header.height);
  if (size_of_bitmap > JBMP_MAX_BITMAP_SIZE)
  {
    // jd_tag(error,1,"'%s' -- bitmap too large (%i bytes)", fname, size_of_bitmap);
    return JBMP_ERR_BITMAP_TOO_BIG;
  }

  // now that we have the dimensions of the bitmap, we can initialize a
  // bitmap struct with those parameters
  // jd_tag(mesg,1,"allocating memory for bitmap, %i bytes requested.", size_of_bitmap);
  c = jbmp_init_bitmap(bitmap, header.width, header.height);

  // reopen the file and move the file position to the bitmap data
  // (we don't care about palette stuff -- life in truecolor, baby!)
  f = fopen(fname, "r");
  fseek(f, header.bitmap_offset, SEEK_SET);
  int a = jbmp_read_file_bitmap(f, header, bitmap);

  // jd_tag(fsio_in,1,"read %i lines of %i pixels from '%s'", header.height, header.width, fname);
  // jd_tag(mesg,1,"read %i bytes total.", a);

  if (a != bitmap->size_bytes)
  {
    // jd_tag(error,1,"size/data mismatch; unable to read complete bitmap from '%s'", fname);
    return JBMP_ERR_SIZE_MISMATCH;
  }
  else
  {
    // jd_tag(mesg,1,"successfully read all %i bytes from file.", a);
  }

  fp = ftell(f);
  fclose(f);
  // jd_tag(fsio_close,1,"closed '%s'", fname);
  // 
  // jd_tag(func_end,1,"jbmp_read_bmp_file(), returning %i", fp);

  return fp;
}

int jbmp_write_file_header(FILE* f, jbmp_header_t h)
{
  // INTRO HEADER
  fwrite(h.magic, 2, 1, f);
  
  fwrite(&h.size_of_bmp, 4, 1, f);
  fwrite(&h.resd1, 4, 1, f);
  fwrite(&h.bitmap_offset, 4, 1, f);
  
  // INFO HEADER
  fwrite(&h.size_of_header, 4, 1, f);
  fwrite(&h.width, 4, 1, f);
  fwrite(&h.height, 4, 1, f);
  fwrite(&h.cplanes, 2, 1, f);
  fwrite(&h.bpp, 2, 1, f);
  fwrite(&h.comp_method, 4, 1, f);
  fwrite(&h.image_size, 4, 1, f);
  fwrite(&h.x_pixels_per_m, 4, 1, f);
  fwrite(&h.y_pixels_per_m, 4, 1, f);
  fwrite(&h.colors_used, 4, 1, f);
  fwrite(&h.important_colors, 4, 1, f);
  
  
  //return fwrite(h, sizeof(jbmp_header_t), 1, f);
}

int jbmp_write_file_bitmap(FILE* f, jbmp_bitmap_t* b)
{
  int a = 0;
  int j, k, n;
  int line = 0;
  jbmp_pixel_t p;

  int row_size_bytes = (((b->width*3)+3)/4) * 4;
  int row_pad_bytes = row_size_bytes - (b->width*3);

  // BMP files store rows from the bottom to the top, so we flip the direction
  // of the j counter
  for (j = b->height-1; j >= 0; j--)
  {
    for (k = 0; k < b->width; k++)
    {
      p = jbmp_get_pixel(b, k, j);
      a += fwrite(&p.b, 1, 1, f);
      a += fwrite(&p.g, 1, 1, f);
      a += fwrite(&p.r, 1, 1, f);
    }
    for (n = 0; n < row_pad_bytes; n++)
    {
      fputc(0, f);
      a++;
    }
    line++;
  }
  return a;
}

int jbmp_init_header(jbmp_header_t* h, jbmp_bitmap_t* b)
{
  int row_size_bytes = (((b->width*3)+3)/4) * 4;
  //h->magic = JBMP_MAGIC_NUMBER;
  h->magic[0] = 'B';
  h->magic[1] = 'M';
  h->size_of_bmp = (row_size_bytes * b->height) + sizeof(jbmp_header_t);
  h->resd1 = 0;
  h->bitmap_offset = sizeof(jbmp_header_t);
  h->size_of_header = 0x28;
  
  h->width = (uint32_t)(b->width);
  h->height = (uint32_t)(b->height);
  h->cplanes = 1;
  h->bpp = 24;
  h->comp_method = 0;
  h->image_size = (b->width*b->height);
  h->x_pixels_per_m = 11811; // 300 dpi 
  h->y_pixels_per_m = 11811; 
  h->colors_used = 0;
  h->important_colors = 0;
  
  return 1;
}

int jbmp_write_bmp_file(const char* fname, jbmp_bitmap_t* bitmap)
{
  jbmp_header_t header;
  jbmp_init_header(&header, bitmap);
  
  FILE* f = fopen(fname, "w");
  jbmp_write_file_header(f, header);
  
  fseek(f, header.bitmap_offset, SEEK_SET);
  jbmp_write_file_bitmap(f, bitmap);
  
  fclose(f);
}



// BITMAP OPS
int jbmp_init_bitmap(jbmp_bitmap_t* b, int w, int h)
{
  b->width = (uint32_t)w;
  b->height = (uint32_t)h;
  b->size = (uint32_t)(w*h);
  b->size_bytes = (b->size * sizeof(jbmp_pixel_t));
  
  b->bitmap = calloc(b->size, sizeof(jbmp_pixel_t));
  if (b->bitmap == NULL) return JBMP_ERR_BADMEM;
  else return b->size_bytes;
}

int p_offset(int w, int x, int y)
{
  return ((y*w)+x);
}

jbmp_pixel_t jbmp_get_pixel(jbmp_bitmap_t* b, int x, int y)
{
  if (x < 0) x = 0;
  else if (x >= b->width) x = b->width-1;
  if (y < 0) y = 0;
  else if (y >= b->height) y = b->height-1;
  
  return b->bitmap[p_offset(b->width, x, y)];
}

int jbmp_set_pixel(jbmp_bitmap_t* b, int x, int y, jbmp_pixel_t p)
{
  if (x < 0) x = 0;
  else if (x >= b->width) x = b->width-1;
  if (y < 0) y = 0;
  else if (y >= b->height) y = b->height-1;
  
  b->bitmap[p_offset(b->width, x, y)] = p;
  return 1;
}
