/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *    jbmp: for reading and writing RGB 24bpp .BMP image files.              *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                             FILE HANDLING                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/***** jbmp_read_file_header() ************************************************
reads the header from file 'f' and into 'header' returns the number of bytes
read.
******************************************************************************/
int jbmp_read_file_header(FILE* f, jbmp_header_t* header);

/***** jbmp_read_file_bitmap() ************************************************
reads the bitmap data from file 'f' and into 'bitmap' and returns the number of
bytes read.
******************************************************************************/
int jbmp_read_file_bitmap(FILE* f, jbmp_header_t header, jbmp_bitmap_t* bitmap);

/***** jbmp_read_bmp_file() ***************************************************
the main read function for reading BMP files; does the following:
1. opens the file, reads the header data, and confirms it is a valid BMP file.
2. initializes 'bitmap' to the required size.
3. reads the bitmap data into 'bitmap' and closes the file.
******************************************************************************/
int jbmp_read_bmp_file(const char* fname, jbmp_bitmap_t* bitmap);

/***** jbmp_write_file_header() ***********************************************
writes the header data from 'h' to file 'f'
******************************************************************************/
int jbmp_write_file_header(FILE* f, jbmp_header_t h);

/***** jbmp_write_file_header() ***********************************************
writes the bitmap data from 'b' to file 'f'
******************************************************************************/
int jbmp_write_file_bitmap(FILE* f, jbmp_bitmap_t* b);

/***** jbmp_write_file_bitmap() ***********************************************
initializes a header struct 'h' based on the dimensions of 'b'
******************************************************************************/
int jbmp_init_header(jbmp_header_t* h, jbmp_bitmap_t* b);

/***** jbmp_read_bmp_file() ***************************************************
the main write function for reading BMP files; does the following:
1. generates a header struct based on the dimensions of 'b'
2. opens/creates a file named 'fname' and writes the header data.
3. writes the bitmap data from 'bitmap' and closes the file.
******************************************************************************/
int jbmp_write_bmp_file(const char* fname, jbmp_bitmap_t* bitmap);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                           BITMAP HANDLING                                 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/***** jbmp_init_bitmap() ************************************************
reads the bitmap data from file 'f' and into 'bitmap' and returns the number of
bytes read.
******************************************************************************/
int jbmp_init_bitmap(jbmp_bitmap_t* b, int w, int h);

/***** jbmp_get_pixel ********************************************************
returns the pixel in 'b' at location given by 'x' and 'y'
******************************************************************************/
jbmp_pixel_t jbmp_get_pixel(jbmp_bitmap_t* b, int x, int y);

/***** jbmp_get_pixel ********************************************************
sets the value of the pixel in 'b' at location given by 'x' and 'y'
******************************************************************************/
int jbmp_set_pixel(jbmp_bitmap_t* b, int x, int y, jbmp_pixel_t p);

// 'v' is cast to type uint8_t
int jbmp_set_pixel_channel(jbmp_pixel_t* p, jbmp_rgb_t channel, int v);

// returns uint8_t cast to an int.
int jbmp_get_pixel_channel(jbmp_pixel_t p, jbmp_rgb_t channel);





#endif // JBMP_H
