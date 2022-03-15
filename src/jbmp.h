/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*                                                                          *
*    jbmp: for reading and writing 24bpp .BMP image files.                 *
*                                                                          *
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef JBMP_H
#define JBMP_H

#include <inttypes.h>
#include "jbmp_types.h"

#define JBMP_DEBUG                      1

#define JBMP_ERR_FILE_NOT_EXIST         -1
#define JBMP_ERR_BAD_FORMAT             -2
#define JBMP_ERR_SIZE_MISMATCH          -3
#define JBMP_ERR_BAD_BPP                -4
#define JBMP_ERR_BITMAP_TOO_BIG         -5
#define JBMP_ERR_BADMEM                 -6

#define JBMP_MAGIC_NUMBER               "BM"
 

#define JBMP_MAX_BITMAP_SIZE            0x0FFFFFFF

// file ops
/***** jbmp_read_file_header() ************************************************
reads the header from file 'f' and returns the current position in the file 
******************************************************************************/
int jbmp_read_file_header(FILE* f, jbmp_header_t* header);

int jbmp_read_file_bitmap(FILE* f, jbmp_header_t header, jbmp_bitmap_t* bitmap);

int jbmp_read_bmp_file(const char* fname, jbmp_bitmap_t* bitmap);

int jbmp_write_file_header(FILE* f, jbmp_header_t h);

int jbmp_write_file_bitmap(FILE* f, jbmp_bitmap_t* b);

int jbmp_init_header(jbmp_header_t* h, jbmp_bitmap_t* b);

int jbmp_write_bmp_file(const char* fname, jbmp_bitmap_t* bitmap);

// bitmap ops
int jbmp_init_bitmap(jbmp_bitmap_t* b, int w, int h);

jbmp_pixel_t jbmp_get_pixel(jbmp_bitmap_t* b, int x, int y);
int jbmp_set_pixel(jbmp_bitmap_t* b, int x, int y, jbmp_pixel_t p);

int jbmp_set_pixel_channel(jbmp_pixel_t* p, jbmp_rgb_t channel, uint8_t v);
uint8_t jbmp_get_pixel_channel(jbmp_pixel_t p, jbmp_rgb_t channel);





#endif // JBMP_H
