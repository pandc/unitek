/*
 * FontsTypes.h
 *
 *  Created on: 20/mar/2016
 *      Author: Administrator
 */

#ifndef SOURCES_FONTS_FONTSTYPES_H_
#define SOURCES_FONTS_FONTSTYPES_H_



typedef struct
{
  int width; // Character width in bits.
  int height; // Character heigh in bits.
  int offset; // Offset in bytes into font bitmap.
}
FONT_CHAR_INFO;


typedef struct
{
  int height; // Character height in bits.
  char start_char; // Start character.
  char end_char; // End character.
  //int width;
  const FONT_CHAR_INFO *p_character_descriptor; // Character decriptor array.
  const unsigned char *p_character_bitmaps; // Character bitmap array.
}
FONT_INFO;

#define CAMBRIA_10 0
#define CALIBRI_10 1
#define CALIBRI_20 2
#define CALIBRI_12 3


#endif /* SOURCES_FONTS_FONTSTYPES_H_ */
