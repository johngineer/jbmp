// jbmp.c

/*
jbmp :: a simple library to read and write .bmp image files

2022 by j. m. de cristofaro ("johngineer")

note: this library currently only reads uncompressed, 24bpp RGB images.s

TODO: add support for 1-bit and greyscale
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jbitmap.h"
#include "jbmp.h"
#include "jdbg.h"


int init_bitmap_struct(jbitmap_t* bitmap, jbmp_header_t* header)
{
  return jb_init(bitmap, header->width, header->height);
}

int jbmp_read_file_header(FILE* f, jbmp_header_t* header)
{
  jd_tag(func_begin,1,"int jbmp_read_file_header(FILE* f, jbmp_header_t* header)");
  int fp = ftell(f);
  fread(&header->magic, 2, 1, f);
  fread(&header->size_of_bmp, 4, 1, f);
  fread(&header->resd1, 2, 1, f);
  fread(&header->resd2, 2, 1, f);
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
  jd_tag(var,1,"header->magic = '%c%c'", header->magic[0], header->magic[1]);
  jd_tag(var,1,"header->size_of_bmp = %i", header->size_of_bmp);
  jd_tag(var,1,"header->resd1 = %i", header->resd1);
  jd_tag(var,1,"header->resd2 = %i", header->resd2);
  jd_tag(var,1,"header->bitmap_offset = %i", header->bitmap_offset);
  jd_tag(var,1,"header->size_of_header = %i", header->size_of_header);
  jd_tag(var,1,"header->width = %i", header->width);
  jd_tag(var,1,"header->height = %i", header->height);
  jd_tag(var,1,"header->cplanes = %i", header->cplanes);
  jd_tag(var,1,"header->bpp = %i", header->bpp);
  jd_tag(var,1,"header->comp_method = %i", header->comp_method);

  jd_tag(func_end,1,"int jbmp_read_file_header(), return = %i", fp);
  return fp;
}

int jbmp_read_file_bitmap(FILE* f, jbmp_header_t header, jbitmap_t* bitmap)
{
  jd_tag(func_begin,1,"int jbmp_read_file_bitmap(FILE* f, jbmp_header_t header, jbitmap_t* bitmap)");

  int row_size_bytes = (((header.width*3)+3)/4) * 4;
  int row_pad_bytes = row_size_bytes - (header.width*3);
  jd_tag(var,1,"int row_size_bytes = %i", row_size_bytes);
  jd_tag(var,1,"int row_pad_bytes = %i", row_pad_bytes);

  int a = 0;
  int j, k;
  int update = header.height / 4;
  int line = 0;
  jbitmap_pixel_t p;

  // BMP files store rows from the bottom to the top, so we flip the direction
  // of the j counter
  for (j = header.height-1; j >= 0; j--)
  {
    if ((line % update)==0) jd_tag(mesg,1,"read %i lines...", line);
    for (k = 0; k < header.width; k++)
    {
      a += fread(&p.b, 1, 1, f);
      a += fread(&p.g, 1, 1, f);
      a += fread(&p.r, 1, 1, f);
      jb_put_pixel(bitmap, k, j, p);
    }
    fseek(f, row_pad_bytes, SEEK_CUR);
    line++;
  }
  jd_tag(mesg,1,"done! read %i lines total.", line);
  //a *= sizeof(jbitmap_pixel_t);

  jd_tag(func_end,1,"int jbmp_read_file_bitmap(), return = %i",a);
  return a;

}

int jbmp_read_bmp_file(const char* fname, jbitmap_t* bitmap)
{
  FILE* f;
  jbmp_header_t header;
  unsigned long fp;
  char file_exists = 0;

  jd_tag(func_begin,1,"int read_bmp_file(const char* fname, jbitmap_t* bitmap)");

  // check if the file exists
  if (f = fopen(fname, "r"))
  {
    file_exists = 1;
  }
  else
  {
    jd_tag(error,1,"file '%s' does not exist!", fname);
    return JBMP_ERR_FILE_NOT_EXIST;
  }

  // file exists, so read the header
  jd_tag(fsio_open,1,"opened '%s' for reading", fname);
  int c = jbmp_read_file_header(f, &header);
  jd_tag(fsio_in,1,"read %i bytes from '%s'",c, fname);
  jd_tag(fsio_close,1,"close '%s'", fname);
  jd_tag(mesg,1,"read BMP file header. verifying...", c);
  fclose(f);   // close the file while we check the validity

  // now that we've read the header, let's verify it's a real .BMP file.
  if (header.magic[0] != 'B' || header.magic[1] != 'M')
  {
    jd_tag(error,1,"'%s' -- bad magic number '%s'", fname, header.magic);
    return JBMP_ERR_BAD_FORMAT;
  }
  else if (header.bpp != 24) // we can only read 24bpp images.
  {
    jd_tag(error,1,"'%s' -- cannot read %ibpp bitmap", fname, header.bpp);
    return JBMP_ERR_BAD_BPP;
  }
  else { jd_tag(mesg,1,"BMP header verified."); }


  // avoid memory overflows
  unsigned int size_of_bitmap = 3 * (header.width * header.height);
  if (size_of_bitmap > JBMP_MAX_BITMAP_SIZE)
  {
    jd_tag(error,1,"'%s' -- bitmap too large (%i bytes)", fname, size_of_bitmap);
    return JBMP_ERR_BITMAP_TOO_BIG;
  }

  // now that we have the dimensions of the bitmap, we can initialize a
  // bitmap struct with those parameters
  jd_tag(mesg,1,"allocating memory for bitmap, %i bytes requested.", size_of_bitmap);
  c = init_bitmap_struct(bitmap, &header);

  // reopen the file and move the file position to the bitmap data
  // (we don't care about palette stuff -- life in truecolor, baby!)
  f = fopen(fname, "r");
  fseek(f, header.bitmap_offset, SEEK_SET);
  int a = jbmp_read_file_bitmap(f, header, bitmap);

  jd_tag(fsio_in,1,"read %i lines of %i pixels from '%s'", header.height, header.width, fname);
  jd_tag(mesg,1,"read %i bytes total.", a);

  if (a != bitmap->image_size_bytes)
  {
    jd_tag(error,1,"size/data mismatch; unable to read complete bitmap from '%s'", fname);
    return JBMP_ERR_SIZE_MISMATCH;
  }
  else
  {
    jd_tag(mesg,1,"successfully read all %i bytes from file.", a);
  }

  fp = ftell(f);
  fclose(f);
  jd_tag(fsio_close,1,"closed '%s'", fname);

  jd_tag(func_end,1,"jbmp_read_bmp_file(), returning %i", fp);

  return fp;
}

// TODO
int jbmp_write_file_header(FILE* f, jbmp_header_t* h)
{
  return fwrite(h, sizeof(jbmp_header_t), 1, f);
}

int jbmp_write_file_bitmap(FILE* f, jbitmap_t* b)
{
  return fwrite(b->bitmap, sizeof(jbmp_pixel_t), b->image_size_pixels, f);
}

int jbmp_write_bmp_file(const char* fname, jbmp_header_t* header, jbitmap_t* bitmap)
{
  FILE* f = fopen(fname, "w");
  jbmp_write_file_header(f, header);
  fseek(f, header->bitmap_offset, SEEK_SET);
  jbmp_write_file_bitmap(f, bitmap);
  fclose(f);
}
