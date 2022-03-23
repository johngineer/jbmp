// demo.c: demo program for libjbmp
// https://github.com/johngineer/jbmp

#include <stdio.h>
#include <stdlib.h>
#include <jbmp/jbmp.h>

int draw_rectangle(jbmp_bitmap_t* b,
                   int x0, int y0, int w, int h,
                   jbmp_pixel_t fill)
{
  if (x0 < 0 || x0 >= b->width) return -1;
  if (y0 < 0 || y0 >= b->height) return -1;
  
  if (x0+w >= b->width) { w = b->width-x0; }
  if (y0+h >= b->height) { h = b->height-y0; }

  int x, y;

  for (int y = 0; y < h; y++)
  {
    for (int x = 0; x < w; x++)
    {
      jbmp_set_pixel(b, x+x0, y+y0, fill);
    }
  }
}

int bitmap_invert(jbmp_bitmap_t* b)
{
  jbmp_pixel_t p;
  uint8_t temp;
  
  for (int y = 0; y < b->height; y++)
  {
    for (int x = 0; x < b->width; x++)
    {
      p = jbmp_get_pixel(b, x, y);
      p.r = 255-p.r;
      p.g = 255-p.g;
      p.b = 255-p.b;
      jbmp_set_pixel(b, x, y, p);
    }
  }
}

char filename[] = "demo.bmp";
char filename2[] = "demo_inverse.bmp";


int main(void)
{
  // declare a couple bitmap structs
  jbmp_bitmap_t bmp1, bmp2;
  
  // intialize bitmap 'bmp1' to 256x256 pixels
  jbmp_init_bitmap(&bmp1, 256, 256, filename);
  
  // define some colors
  jbmp_pixel_t red = jbmp_rgb(0x00, 0x00, 0xFF);
  jbmp_pixel_t grn = jbmp_rgb(0x00, 0xFF, 0x00);
  jbmp_pixel_t blu = jbmp_rgb(0xFF, 0x00, 0x00);
  jbmp_pixel_t gry = jbmp_rgb(0x40, 0x40, 0x40);
  
  // draw some colored boxes
  draw_rectangle(&bmp1, 0, 0, 128, 128, red);
  draw_rectangle(&bmp1, 128, 0, 128, 128, grn);
  draw_rectangle(&bmp1, 0, 128, 128, 128, blu);
  draw_rectangle(&bmp1, 128, 128, 128, 128, gry);
  
  // write the first bitmap out to 'filename'
  jbmp_write_bmp_file(filename, &bmp1, 1);
  
  // read the bitmap in 'filename' into bitmap 'bmp2'.
  // 'bmp2' is auto-init'd based on the info read from the file.
  jbmp_read_bmp_file(filename, &bmp2, 1);
  
  // invert the contents of 'bmp_inverse'
  bitmap_invert(&bmp2);
  
  // draw some smaller rectangles on top of the image
  draw_rectangle(&bmp2, 32, 32, 64, 64, red);
  draw_rectangle(&bmp2, 160, 32, 64, 64, grn);
  draw_rectangle(&bmp2, 32, 160, 64, 64, blu);
  draw_rectangle(&bmp2, 160, 160, 64, 64, gry);
  
  // write 'bmp2' out to 'filename2'
  jbmp_write_bmp_file(filename2, &bmp2, 1);
}
