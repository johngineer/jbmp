/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                           *
 *    jbmp: for reading and writing RGB 24bpp .BMP image files.              *
 *                                                                           *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#ifndef JBMP_H
#define JBMP_H

#include <inttypes.h>
#include <stdbool.h>
#include "jbmp_types.h"

#define JBMP_DEBUG                       1

#define JBMP_ERR_BAD_FILENAME           -1
#define JBMP_ERR_BAD_MAGIC              -2
#define JBMP_ERR_BAD_FORMAT             -3 
#define JBMP_ERR_SIZE_MISMATCH          -4
#define JBMP_ERR_BITMAP_TOO_BIG         -5
#define JBMP_ERR_NOMEM                  -6

#define JBMP_MAGIC_NUMBER               "BM"
 
#define JBMP_MAX_BITMAP_SIZE            0x1FFFFFFF // in bytes, about 500Mb

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * FILE HANDLING * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* * * jbmp_read_file_header() * * * * * * * * * * * * * * * * * * * * * * * *
 
 reads the header from file 'f' and into 'header' returns the number of bytes
 read.
 
 FILE* f ------------------- the file pointer
 jbmp_header_t* header ----- pointer to the header struct that we fill with
                               data from the file.
 int verbose --------------- verbosity flag (0 = silent, >=1 = loud)
 
 returns (int) ------------- the number of bytes read.
 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int jbmp_read_file_header(FILE* f, jbmp_header_t* h, int verbose);


/* * * jbmp_read_file_bitmap() * * * * * * * * * * * * * * * * * * * * * * * *
 
 reads the bitmap data from file 'f' and into 'bitmap' and returns the number
 of bytes read.
 
 FILE* f ------------------- the file pointer
 jbmp_header_t header ------ the header struct we fill with data from the
                               file.
 jbmp_bitmap_t* bitmap ----- pointer to the bitmap struct where we put the 
                               bitmap data from the file.
 int verbose --------------- verbosity flag (0 = silent, >=1 = loud).
 
 returns (int) ------------- the number of bytes read
 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int jbmp_read_file_bitmap(FILE* f, jbmp_header_t header, jbmp_bitmap_t* bitmap,
                          int verbose);


/* * * jbmp_read_file_bitmap() * * * * * * * * * * * * * * * * * * * * * * * *

the main read function for reading BMP files; does the following:
  1. opens the file, reads the header data, and confirms it is a valid BMP 
     file.
  2. initializes 'bitmap' to the required size.
  3. reads the bitmap data into 'bitmap' and closes the file.
 
 char* fname --------------- the string containing the file name.
 jbmp_bitmap_t* bitmap ----- pointer to the bitmap struct where we put the 
                               bitmap data from the file.
 int verbose --------------- verbosity flag (0 = silent, >=1 = loud).
 
 returns (int) ------------- the number of bytes read
 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int jbmp_read_bmp_file(char* fname, jbmp_bitmap_t* bitmap, int verbose);


/* * * jbmp_write_file_header()  * * * * * * * * * * * * * * * * * * * * * * *

writes the header data from 'h' to file 'f'

 FILE* f ------------------- the file pointer
 jbmp_header_t header ------ the header struct we fill with data from the
                               file.

 returns (int):
   on failure: an error code
   on success: the number of bytes written
   
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int jbmp_write_file_header(FILE* f, jbmp_header_t h, int verbose);

/***** jbmp_write_file_header() ***********************************************
writes the bitmap data from 'b' to file 'f'
******************************************************************************/
int jbmp_write_file_bitmap(FILE* f, jbmp_bitmap_t* b, int verbose);

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
int jbmp_write_bmp_file(char* fname, jbmp_bitmap_t* bitmap, int verbose);


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * * * * * * * * * * * * * BITMAP & PIXEL HANDLING * * * * * * * * * * * * * *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 
 jbmp_pixel_t jbmp_rgb(uint8_t r, uint8_t g, uint8_t b);

/***** jbmp_init_bitmap() ************************************************
reads the bitmap data from file 'f' and into 'bitmap' and returns the number of
bytes read.
******************************************************************************/
int jbmp_init_bitmap(jbmp_bitmap_t* b, int w, int h, char* fname);

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
