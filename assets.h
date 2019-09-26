/**
@file assets.h

Assets to be used in game. Textures are stored by columns for cache
friendliness (as rendering also happens by columns).
*/

#ifndef _SFG_RESOURCES_H
#define _SFG_RESOURCES_H

#define SFG_TEXTURE_SIZE 32

/**
  Color index which will in textures and sprites be considered transparent.
*/
#define SFG_TRANSPARENT_COLOR 175

/**
  Special index of an implicit texture that consists of only transparent pixels.
*/
#define SFG_TRANSPARENT_TEXTURE 255

static inline uint8_t SFG_getTexel(const uint8_t *texture, uint8_t x, uint8_t y)
{
  return texture[(x & 0x1f) * SFG_TEXTURE_SIZE + (y & 0x1f)];
}

const uint8_t SFG_texturesWall[][SFG_TEXTURE_SIZE * SFG_TEXTURE_SIZE] =
{
  { // 0
4,4,4,21,4,4,4,3,5,4,4,21,5,21,4,4,57,21,20,4,4,21,21,4,20,4,4,3,4,3,5,4,4,4,21,
21,21,4,4,18,5,4,4,21,21,21,4,21,57,21,20,21,4,21,21,4,20,17,6,20,5,3,5,4,4,4,
21,21,21,4,4,26,5,4,4,21,5,4,21,21,49,4,3,21,4,4,21,4,3,18,2,3,0,26,136,81,4,4,
21,5,21,4,4,2,5,21,4,4,21,4,4,21,73,21,3,4,4,4,4,4,74,21,4,4,4,5,4,4,4,4,21,21,
5,4,4,2,5,4,4,21,4,4,4,21,65,5,3,4,4,4,4,4,3,4,4,4,4,5,4,4,4,4,21,21,5,4,4,26,5,
4,21,21,4,21,4,4,65,5,3,4,4,21,4,4,3,21,4,4,4,21,4,4,4,4,4,21,21,4,4,3,5,4,21,5,
4,4,4,4,1,5,3,4,4,4,4,4,3,4,4,4,21,5,4,20,20,4,21,21,5,4,4,20,5,3,4,4,4,3,4,4,1,
5,3,4,4,4,4,4,3,21,4,4,5,5,4,20,4,4,21,21,5,4,4,2,6,5,3,5,5,4,5,6,49,5,3,4,4,21,
4,4,3,21,4,4,21,5,4,4,4,4,4,5,5,4,4,56,0,0,0,0,0,0,0,0,0,22,3,4,4,5,4,4,3,21,4,
4,5,21,4,3,4,4,4,5,21,4,4,3,5,4,4,4,5,4,4,5,2,5,3,4,4,21,4,4,3,21,4,4,5,5,21,3,
4,4,21,5,21,21,4,3,5,4,4,4,5,4,4,4,1,5,3,4,4,5,4,4,4,4,4,4,5,5,4,3,4,20,4,5,21,
4,4,3,21,4,4,4,5,4,5,4,25,5,3,4,4,5,4,4,3,4,4,4,5,21,21,3,4,4,21,5,5,21,4,3,4,4,
21,4,5,4,5,4,73,5,3,4,4,21,4,21,3,5,4,21,5,5,4,3,4,20,21,21,21,4,4,3,4,4,21,4,5,
4,21,4,57,5,3,4,4,21,20,4,3,5,4,21,5,5,4,26,4,3,4,21,21,21,4,3,4,4,21,4,5,4,21,
4,65,6,4,5,4,5,4,5,4,5,3,4,21,5,4,3,4,4,4,4,21,21,4,3,4,4,21,21,5,4,21,4,0,17,0,
16,0,16,0,10,26,4,4,4,21,5,4,3,4,4,4,4,21,4,4,3,4,4,4,5,5,4,21,4,3,4,3,4,3,4,4,
3,4,4,4,4,21,21,4,20,4,4,4,4,21,21,4,3,4,4,4,21,5,4,21,4,3,4,3,4,4,4,4,3,4,21,4,
21,21,5,4,3,4,4,4,4,5,4,4,26,21,4,4,21,5,4,21,4,3,4,4,4,4,21,4,4,4,21,3,5,5,5,4,
3,4,4,4,21,5,21,4,3,21,4,4,21,21,20,5,4,26,4,3,4,4,4,21,4,4,21,3,21,5,5,4,3,20,
4,21,4,5,21,4,3,4,20,4,5,4,20,21,4,3,4,3,21,4,4,21,4,4,4,3,21,5,5,4,3,20,4,4,4,
5,4,4,3,21,4,4,21,4,4,4,4,3,4,3,21,4,4,21,4,20,21,3,4,5,5,4,3,3,21,4,4,21,21,4,
3,21,4,4,5,4,4,4,4,3,4,3,21,4,21,5,4,4,4,3,21,5,5,4,3,20,4,4,4,21,4,21,3,21,4,4,
5,4,21,4,4,26,21,3,21,21,21,21,4,4,4,3,4,5,4,4,3,4,4,4,4,21,4,21,3,21,4,4,21,4,
5,4,4,2,21,20,5,21,21,5,4,20,4,3,4,5,21,21,3,5,5,4,5,4,4,6,82,21,4,4,21,21,5,4,
4,2,21,3,21,21,21,21,4,20,5,20,4,5,4,4,3,0,20,96,20,2,18,20,0,21,4,4,21,21,21,4,
4,2,21,3,21,21,5,21,4,3,5,3,4,21,4,21,3,3,3,3,4,3,3,3,3,21,4,4,21,21,4,4,4,2,5,
3,5,4,21,21,3,20,5,3,4,5,4,4,3,4,4,4,21,4,4,4,3,5,4,4,21,5,21,4,4,65,21,3,21,4,
5,21,4,20,5,20,4,21,4,4,3,4,4,4,21,4,4,4,26,5,4,4,21,21,4,4,21,224,5,20,4,4,21,
21,4,3,5,4,4,21,4,5,3,4,4,4,21,4,4,4,18,5,21,4,5,5,4,4,4,57,21,20,4,4,21,21,20,
3,4,4,4,4,4,5,4
  },
  { // 1
5,4,131,4,44,4,132,4,3,66,3,5,4,4,4,4,4,5,3,132,4,5,4,44,4,4,3,4,5,44,4,4,4,4,
131,4,4,4,4,4,4,66,2,4,4,4,5,5,4,4,3,4,4,4,5,5,4,5,3,4,4,44,4,132,4,4,131,4,4,4,
44,4,4,2,2,5,4,4,4,4,4,4,3,3,4,5,5,4,4,5,3,4,5,4,4,4,5,51,3,4,4,4,4,3,3,1,2,3,4,
3,50,3,137,0,1,50,3,3,44,4,4,3,131,4,5,5,4,4,59,67,2,66,2,3,3,3,3,3,3,3,3,3,3,3,
3,3,3,3,3,3,3,3,3,3,59,2,2,4,4,4,2,8,66,3,6,6,6,6,6,6,6,6,5,5,6,5,6,6,5,6,5,6,6,
5,6,6,4,50,3,3,83,2,5,5,60,3,6,5,5,5,5,4,5,5,4,5,5,5,5,5,5,4,5,5,4,5,5,5,3,138,
4,5,5,5,5,86,4,3,6,5,128,138,59,2,129,2,137,81,138,2,137,128,81,81,6,5,5,4,5,5,
4,217,4,5,5,85,4,3,2,81,5,5,0,48,2,129,59,81,128,81,81,48,81,137,48,128,6,5,5,5,
5,5,3,2,4,4,4,4,66,2,2,81,6,4,48,128,59,59,2,81,81,2,2,129,81,48,81,81,6,5,5,5,
4,5,4,2,81,66,49,2,45,4,3,3,4,4,2,81,81,81,81,129,59,129,59,2,81,0,81,128,6,5,5,
5,5,5,3,2,4,4,4,4,5,5,3,4,4,5,138,128,81,48,137,81,137,2,2,138,81,81,81,81,6,5,
5,6,20,5,4,2,4,4,4,4,4,4,4,3,6,5,81,138,128,128,81,81,81,81,130,128,81,129,81,
81,6,5,5,4,5,5,4,2,4,3,4,4,4,4,3,59,6,4,2,2,81,81,137,128,48,138,3,81,48,81,48,
48,6,5,5,5,5,5,3,2,4,4,3,4,4,4,4,2,5,5,59,139,128,0,81,2,48,81,137,0,48,48,48,
48,6,5,4,5,5,5,4,138,4,4,4,4,4,4,3,66,6,4,68,6,6,6,6,6,6,6,6,6,6,6,68,6,6,5,5,5,
4,5,26,3,4,4,4,44,4,4,4,82,5,5,6,6,76,6,60,6,76,6,61,6,68,6,61,68,6,5,5,5,5,5,3,
3,4,4,4,4,4,4,3,81,6,5,130,59,3,130,2,0,0,138,2,81,0,0,0,0,6,5,4,28,5,5,3,2,4,3,
3,3,2,2,49,2,6,4,3,59,59,59,2,129,2,81,216,0,0,0,0,0,6,5,5,5,5,5,26,2,2,4,3,3,5,
4,2,66,6,5,59,59,4,3,59,129,59,48,0,0,0,0,0,0,6,5,5,5,5,6,4,81,4,4,4,5,4,5,2,
218,6,5,51,3,59,59,4,2,216,48,0,0,0,48,0,0,6,5,5,5,5,5,26,131,4,4,4,4,44,4,5,66,
6,5,59,59,3,59,59,2,129,48,0,48,48,0,0,0,6,5,5,4,5,5,3,3,4,4,4,4,4,4,4,138,5,5,
3,59,59,3,3,81,138,128,0,48,48,48,81,0,6,5,5,4,5,5,4,2,4,4,4,4,4,5,3,131,6,5,59,
3,51,137,137,48,2,138,0,0,0,48,48,0,6,5,4,5,28,4,3,3,4,4,3,4,4,4,51,2,5,5,59,59,
59,130,0,0,81,48,0,0,0,0,0,0,6,5,5,4,5,5,2,3,4,4,4,4,4,5,3,2,6,5,5,5,5,5,5,5,5,
5,5,5,5,5,5,5,4,5,4,5,4,5,2,4,4,4,4,4,3,4,131,56,6,5,5,5,6,5,5,5,5,5,5,6,6,6,5,
5,5,5,5,5,5,6,131,3,4,4,3,3,3,50,2,3,3,3,2,51,3,51,3,3,131,2,3,2,2,3,3,3,4,3,3,
4,3,4,50,4,5,86,5,6,2,3,50,4,4,4,5,4,132,4,2,3,3,3,3,3,3,3,50,1,3,2,3,3,3,3,3,2,
2,2,3,66,44,60,66,4,4,4,4,4,4,139,3,4,4,4,5,4,4,5,3,3,5,4,5,4,45,4,4,131,4,4,5,
4,4,44,2,4,4,4,4,4,4,138,4,4,4,4,4,4,4,5,3,3,4,4,4,5,4,5,4,3,5,45,4,4,4,5,131,4,
5,132,4,4,4,131,3,5,4,4,5,5,4,4,51,4,4,4,4,5,5,5,51,5,5,4,61,4
  },
  { // 2
12,83,83,12,11,11,12,12,11,11,11,83,11,12,11,12,5,4,4,4,4,4,5,4,12,12,85,83,12,
12,83,83,12,11,12,12,11,11,83,12,12,11,12,11,11,11,11,11,4,4,5,4,5,4,5,4,12,12,
12,12,12,83,12,12,12,12,11,83,12,84,12,83,11,83,11,83,12,11,83,11,5,4,5,4,5,4,5,
4,12,12,12,12,12,12,12,84,12,12,83,83,83,83,83,83,83,83,83,83,83,83,83,84,5,4,5,
4,4,4,4,4,12,12,12,12,12,12,12,11,12,84,12,83,4,4,4,4,20,4,4,4,20,4,4,85,5,3,5,
3,5,3,5,4,12,12,12,12,83,12,83,11,83,12,83,83,20,84,20,84,61,83,83,83,83,83,83,
83,5,3,5,3,5,4,4,5,83,83,83,83,83,83,83,83,61,83,12,83,4,84,20,61,83,83,61,61,
61,61,61,61,6,4,5,3,5,3,4,5,61,61,61,61,61,61,61,61,61,83,12,83,4,20,20,61,83,
20,20,20,20,20,4,85,5,3,5,3,5,4,4,5,12,12,12,12,12,12,12,84,61,83,84,83,4,20,20,
61,83,20,20,20,84,20,84,85,6,4,6,3,5,4,5,4,12,12,12,12,12,12,12,11,61,83,13,83,
20,84,20,84,83,20,20,20,20,3,4,85,5,4,5,4,5,4,4,5,12,12,12,12,84,12,12,20,83,83,
83,83,83,83,83,83,61,84,20,84,84,20,20,85,6,4,6,3,5,4,5,4,12,12,12,12,83,83,83,
83,61,61,61,61,61,61,61,61,84,84,20,84,20,84,4,85,6,4,6,4,6,4,5,5,12,12,12,61,
83,61,61,61,20,13,12,83,84,84,84,20,84,20,20,20,20,20,84,85,5,4,6,4,5,4,5,5,12,
12,12,61,83,12,12,20,13,12,84,83,4,20,131,130,130,131,131,84,4,20,4,85,6,4,6,4,
6,5,5,5,12,13,13,61,83,12,12,12,84,84,84,83,20,84,130,20,20,20,130,84,4,84,84,
85,6,4,6,4,5,4,5,5,12,84,12,61,83,84,84,84,84,84,12,83,84,20,131,20,20,20,131,
20,84,20,20,85,6,4,6,4,6,5,5,5,12,20,12,61,83,12,84,12,84,13,84,83,4,84,131,4,
84,20,130,84,4,84,84,85,6,4,7,4,6,5,5,5,12,13,13,61,83,20,20,84,13,13,85,83,84,
84,130,4,4,84,130,84,4,84,20,85,6,4,6,4,5,5,5,6,12,84,84,61,83,12,84,13,13,84,
12,83,20,84,130,84,4,4,130,84,84,84,20,85,7,5,6,5,6,5,5,5,12,13,13,61,83,13,13,
20,13,13,85,83,4,20,131,130,131,130,131,84,4,4,4,85,6,5,5,5,5,5,5,5,12,13,13,61,
83,13,20,13,13,13,85,83,20,4,4,4,4,4,84,84,84,4,20,85,6,5,6,5,5,5,5,6,12,84,13,
61,83,84,13,20,83,83,83,83,83,83,83,83,84,4,4,84,84,84,84,85,6,5,5,6,6,5,5,6,12,
13,13,61,83,83,83,83,61,61,61,61,61,61,61,84,83,4,4,4,20,84,84,85,6,6,5,5,5,6,5,
6,12,85,85,13,61,61,61,61,61,83,85,83,4,4,4,61,83,4,4,4,84,84,3,85,6,5,5,6,5,5,
5,5,12,13,85,85,13,85,85,85,61,83,85,83,3,4,4,61,83,4,4,4,4,4,20,85,6,6,6,5,5,6,
5,6,12,20,85,13,13,85,85,13,61,83,12,83,84,4,4,61,83,4,4,84,20,3,84,85,5,7,5,6,
5,6,5,6,12,20,13,13,13,13,13,85,61,83,85,83,20,84,4,61,61,83,83,83,83,83,83,83,
5,6,5,6,5,7,5,6,83,83,83,83,83,83,83,83,61,13,13,83,84,3,3,20,61,61,61,61,61,61,
61,61,5,7,5,6,5,5,5,7,61,61,61,61,61,61,61,61,85,13,85,83,3,84,20,84,3,84,3,20,
84,3,3,85,5,6,5,7,5,6,5,6,12,85,85,85,13,85,85,85,15,85,85,84,85,85,85,85,85,85,
85,85,85,85,85,85,5,7,5,5,5,7,5,7,12,12,13,13,13,85,14,85,85,85,13,85,85,14,85,
13,85,84,14,84,84,84,85,12,5,7,5,7,5,6,5,6,12,20,85,13,13,85,85,12,13,12,85,12,
13,84,13,12,13,13,13,12,13,13,84,12,5,6,5,6,5,6,6,6,12,12,85,20,12,13,13,85
  },
  { // 3
4,5,4,4,4,4,37,4,5,4,4,4,4,4,4,3,4,37,5,37,4,3,20,4,4,20,3,19,3,36,19,3,4,4,5,4,
76,4,5,4,4,4,3,4,36,4,3,11,12,36,36,4,20,3,36,37,36,4,3,4,12,3,4,130,4,5,4,4,4,
4,4,4,4,4,4,4,4,4,19,11,4,36,36,4,36,3,36,4,36,36,3,36,4,3,3,3,4,4,5,4,3,4,36,4,
4,4,3,4,4,4,4,11,4,4,4,4,4,3,20,36,4,4,3,12,3,36,3,3,4,5,4,4,4,37,36,4,4,4,3,4,
4,4,3,11,12,36,20,3,12,3,37,37,20,36,3,4,4,4,3,3,4,5,4,4,3,5,4,4,4,4,4,4,4,4,4,
76,4,20,4,4,4,4,20,36,36,4,3,36,19,3,3,3,4,4,4,4,4,36,37,4,37,4,4,4,4,4,4,3,4,
36,37,4,3,36,4,36,37,36,3,4,4,19,4,3,4,5,4,4,35,5,4,4,4,4,3,4,4,4,4,3,19,20,4,
36,4,3,36,4,36,36,3,4,3,3,3,3,3,36,4,4,3,4,36,4,4,4,3,4,4,4,3,11,4,37,20,4,12,3,
36,36,4,36,36,3,4,3,4,130,4,5,4,4,4,4,5,5,4,4,4,4,4,4,4,11,37,37,4,36,4,3,20,36,
37,20,4,36,12,4,3,3,4,4,5,20,4,37,4,20,5,4,76,4,4,4,4,11,4,20,4,20,4,36,36,20,
37,36,3,4,3,3,4,3,5,5,4,4,4,5,5,4,4,4,4,4,4,12,4,3,4,36,5,4,36,3,4,36,36,36,12,
12,36,3,12,3,4,20,5,4,76,4,20,5,4,4,4,4,4,4,4,12,4,37,37,20,4,3,36,36,20,36,12,
4,4,4,3,2,4,5,5,4,76,5,5,4,4,77,4,4,4,4,4,11,20,20,5,4,20,4,4,37,4,36,36,3,4,4,
4,2,4,5,4,5,76,4,4,4,4,4,76,4,4,4,4,76,4,37,37,4,36,3,36,4,36,36,12,4,4,3,3,130,
4,4,5,4,4,5,4,4,4,3,76,4,4,4,4,3,36,36,4,4,4,3,4,36,36,36,3,130,4,4,3,130,4,5,4,
4,76,4,5,4,4,4,4,4,4,76,4,11,4,4,36,4,4,36,4,36,36,4,35,4,3,11,4,3,4,5,5,4,76,5,
4,20,5,4,4,4,4,4,4,76,4,20,5,20,4,36,4,4,36,36,4,4,3,4,3,3,5,5,13,5,76,4,5,5,4,
4,76,4,4,4,4,76,36,5,36,4,4,3,37,36,20,36,19,4,4,4,4,11,4,4,5,77,4,5,5,20,4,4,4,
4,77,5,4,11,4,37,5,4,20,3,20,4,36,4,3,12,4,3,4,3,4,5,5,4,76,20,5,5,4,76,4,4,4,
13,4,11,20,20,36,5,4,3,4,4,4,36,3,4,12,4,4,3,5,20,5,5,4,5,4,4,4,4,4,4,4,4,4,11,
4,4,4,4,4,4,36,4,4,4,3,12,4,12,3,3,4,5,5,77,4,20,5,4,5,77,4,4,4,4,4,11,4,37,20,
4,20,4,5,36,4,4,11,12,4,4,4,3,4,5,20,4,76,5,5,13,4,76,4,4,5,4,4,11,36,4,36,4,4,
3,37,37,4,4,3,3,4,4,36,3,4,5,5,4,4,36,4,5,4,4,4,20,4,4,4,11,4,37,20,4,4,4,4,4,4,
20,4,4,12,4,4,3,4,20,5,5,76,5,4,5,4,76,76,5,5,4,4,11,37,37,5,20,36,4,20,36,4,37,
4,3,4,4,4,130,4,5,5,4,76,5,13,4,77,4,4,20,36,20,4,11,4,20,4,4,4,3,4,20,4,4,4,4,
12,4,12,3,4,5,13,5,4,5,4,4,4,4,4,4,4,5,4,3,4,37,37,37,20,4,35,36,4,20,36,4,4,4,
4,3,4,5,5,4,4,4,5,77,4,4,3,37,4,20,4,11,20,4,20,20,5,35,4,20,36,4,4,4,3,12,3,
130,4,5,5,4,76,4,4,4,4,4,3,4,4,4,4,3,4,37,37,4,4,4,37,37,4,36,3,4,4,4,4,3,4,5,
13,5,4,4,4,4,4,4,3,4,36,4,3,3,37,20,37,37,4,4,36,4,20,4,19,4,3,19,3,3,4,4,5,4,
76,4,4,4,4,4,4,4,20,4,4,35,4,36,20,20,4,4,4,37,36,4,3,4,4,4,3,3
  },
  { // 4
43,19,43,11,43,10,43,43,43,83,65,83,83,43,83,83,2,11,11,11,11,1,11,11,11,11,73,
11,11,11,11,2,20,20,20,20,20,20,21,20,21,20,11,21,20,20,20,11,11,12,12,11,11,73,
12,12,12,12,2,11,12,12,12,11,20,20,20,20,20,43,21,20,20,21,19,20,20,21,21,11,11,
12,11,11,11,1,11,12,11,12,73,12,12,12,12,11,19,20,20,20,20,19,21,20,20,21,19,21,
20,21,21,11,11,12,11,11,12,73,11,12,12,11,1,12,12,12,12,11,43,20,20,21,20,43,21,
20,20,21,19,21,20,21,21,19,11,12,11,12,12,73,12,11,12,12,73,11,12,12,12,11,43,
20,20,20,20,43,21,20,20,21,19,21,20,20,21,19,11,12,11,11,12,2,11,11,11,12,73,11,
11,12,12,11,43,20,20,20,20,11,21,20,21,21,81,21,20,21,21,19,2,12,11,11,12,2,11,
11,11,12,2,12,11,12,12,11,43,20,20,21,20,11,21,20,20,20,81,21,20,20,21,19,11,12,
11,11,12,2,11,11,12,12,2,11,11,12,12,11,20,20,20,20,20,11,20,21,20,20,2,21,20,
20,21,43,82,12,11,11,12,2,11,11,12,12,1,11,11,12,12,11,20,20,20,20,20,11,21,21,
20,20,34,21,20,20,20,43,11,12,11,11,11,2,11,11,12,12,73,11,12,11,12,11,43,20,20,
20,20,19,20,21,20,20,19,21,20,20,21,43,11,12,11,11,12,2,12,11,11,12,73,12,12,12,
12,11,20,20,20,21,20,11,21,20,20,21,19,21,20,20,20,19,2,12,11,12,12,2,12,11,11,
12,2,12,12,12,12,11,20,20,20,20,20,11,20,20,20,20,19,20,20,20,20,19,11,11,11,11,
12,2,12,12,11,12,1,12,12,12,12,11,20,20,20,20,20,43,20,20,20,20,19,21,20,20,20,
12,11,12,11,11,11,2,12,12,11,12,1,11,12,12,12,11,20,21,20,21,21,43,21,20,20,20,
11,21,20,20,20,19,2,12,12,12,12,2,12,12,12,12,2,11,12,12,12,11,43,20,43,43,43,
19,20,43,43,43,42,20,43,43,43,43,2,2,2,2,2,1,2,11,2,2,1,2,2,2,11,2,2,11,11,11,
12,2,11,11,11,11,2,11,11,11,11,2,19,20,20,20,20,43,20,43,43,20,43,20,43,43,43,
11,2,12,11,11,12,2,11,12,11,11,2,12,12,11,12,11,20,21,20,21,21,43,21,20,20,21,
43,21,20,20,20,43,2,12,11,11,12,2,12,11,11,11,2,12,12,11,11,11,21,21,21,21,21,
19,21,21,20,21,43,20,20,43,20,43,2,11,11,11,12,2,12,11,11,11,2,12,12,11,12,11,
21,21,21,21,21,43,21,21,20,21,43,20,20,43,20,43,2,11,11,11,12,2,12,12,11,12,2,
12,12,12,12,11,20,20,21,21,21,19,21,21,20,21,43,20,20,43,20,43,2,12,11,11,12,2,
12,12,12,12,2,12,12,12,11,11,21,21,21,21,21,43,21,21,20,21,43,20,20,20,20,20,2,
12,11,11,12,2,12,12,12,12,2,12,12,12,12,11,20,20,21,21,21,19,21,20,21,21,43,20,
20,20,20,20,2,11,11,11,12,2,12,12,12,11,2,12,12,12,12,11,20,21,21,21,21,19,21,
20,21,21,43,20,20,43,20,43,2,12,11,11,12,2,12,12,11,12,2,12,12,12,12,11,20,21,
21,21,21,19,21,21,21,21,11,20,20,20,20,43,2,12,11,11,12,2,12,12,11,12,2,12,12,
12,12,11,20,21,21,21,21,19,21,21,21,21,43,20,20,12,20,43,2,12,11,11,12,2,12,12,
11,12,2,12,11,11,11,11,20,21,21,21,21,10,21,20,21,21,43,20,20,43,43,43,2,12,11,
11,12,2,12,12,12,12,2,11,11,11,11,2,20,21,21,21,21,2,21,20,20,21,43,21,20,12,20,
43,2,12,11,11,12,2,12,12,12,12,2,2,12,12,12,2,20,21,21,21,21,19,21,20,21,20,11,
21,21,20,20,43,2,12,11,12,12,2,11,11,12,12,2,11,12,12,12,11,20,21,20,20,21,2,21,
21,21,21,11,21,21,20,20,43,2,12,11,12,12,2,12,12,12,12,2,12,12,11,12,11,20,21,
21,21,21,19,21,21,20,20,11,21,21,20,20,43,11,20,43,20,19,81,20,20,43,20,19,43,
20,19,20,11,43,43,43,43,43,9,43,19,43,19,81,20,20,43,43,11
  }
};

const uint8_t SFG_backgrounds[][SFG_TEXTURE_SIZE * SFG_TEXTURE_SIZE] =
{
  { // 5
10,10,10,10,10,10,19,19,12,13,13,13,13,13,13,13,13,14,64,80,64,64,73,64,64,64,
73,136,73,73,73,73,10,10,10,10,10,19,12,12,12,19,12,12,13,13,13,14,14,14,14,64,
64,64,80,73,80,73,64,73,73,73,73,73,10,10,10,10,10,19,19,10,19,19,19,12,13,13,
14,14,14,168,168,64,168,64,64,64,64,73,73,64,73,73,73,73,10,10,10,10,10,10,10,
10,19,19,19,19,12,13,13,14,14,14,14,14,14,80,64,64,64,73,64,136,73,73,73,73,10,
10,10,10,10,10,10,19,19,12,19,12,168,168,168,168,64,64,64,80,64,64,136,64,64,73,
73,64,73,73,73,73,10,10,10,10,10,10,10,19,12,12,12,13,168,0,0,0,0,168,168,168,
64,64,64,64,64,64,64,73,73,73,73,73,10,10,10,10,10,10,19,19,12,12,13,13,57,0,0,
0,168,168,136,64,168,64,64,64,64,64,73,64,73,73,136,73,10,10,10,10,10,10,19,19,
12,12,13,13,168,168,168,168,168,64,64,168,64,64,64,64,80,73,73,73,73,73,73,73,
10,10,10,10,19,19,19,19,12,12,13,13,13,13,14,14,14,14,14,80,168,64,73,80,73,64,
73,64,73,73,73,73,10,10,10,10,10,19,19,12,12,13,13,13,13,13,14,14,14,14,14,14,
80,64,80,64,64,64,64,73,73,73,73,73,10,10,10,10,10,10,19,12,12,13,168,168,168,
168,168,168,64,64,64,80,168,64,73,80,73,64,73,64,73,73,73,73,10,10,10,10,10,10,
10,19,12,13,168,0,223,0,223,0,219,168,168,168,64,64,64,64,80,64,64,73,73,73,73,
73,59,10,10,10,10,10,10,19,12,12,168,0,0,0,0,0,168,168,168,64,168,64,64,64,73,
64,73,64,73,73,73,73,10,10,10,10,10,10,19,19,12,12,168,168,168,57,168,168,168,
64,168,168,64,80,80,73,80,73,64,73,73,73,73,73,10,10,10,10,10,19,19,12,61,13,
101,12,12,12,12,63,14,14,64,64,168,80,64,64,64,64,73,136,73,73,73,73,10,10,10,
10,10,10,19,12,12,12,101,101,12,12,12,101,14,63,64,168,64,57,64,64,64,64,64,73,
73,73,73,73,10,10,10,10,10,10,19,19,12,12,101,101,168,168,168,168,64,64,64,80,
64,80,64,64,64,64,73,64,73,73,73,73,10,10,10,10,10,10,19,19,19,100,101,101,168,
168,168,168,168,64,64,64,80,136,64,64,64,64,64,73,73,73,73,73,10,10,10,10,10,10,
19,19,19,100,100,101,168,168,168,168,64,64,64,80,168,64,64,64,64,64,73,64,73,73,
73,73,10,10,10,10,19,10,19,12,168,168,168,168,168,0,0,0,0,168,168,168,136,64,64,
64,64,64,64,73,73,73,73,73,10,10,10,10,19,19,19,12,168,223,136,223,0,0,0,0,168,
168,168,64,136,64,64,64,64,64,73,64,73,73,73,73,10,10,10,10,19,60,19,12,168,168,
168,168,168,168,168,168,168,64,57,64,80,80,80,64,64,136,64,73,73,73,73,73,59,59,
10,10,19,60,19,19,12,12,100,100,101,101,187,101,101,64,64,80,168,64,73,80,73,64,
73,136,73,73,73,73,10,10,10,10,10,19,19,19,19,12,12,100,100,101,187,168,168,64,
64,64,64,64,64,64,64,64,64,73,73,136,73,73,10,10,10,10,19,19,19,19,19,100,100,
101,100,100,187,168,64,64,64,80,168,64,64,64,136,64,73,64,73,73,73,73,59,10,10,
19,19,12,12,12,12,19,100,101,100,100,101,168,168,64,64,64,80,80,80,64,64,64,64,
73,73,73,73,73,10,10,19,19,19,12,12,12,13,13,13,101,100,100,100,101,101,14,14,
80,64,80,73,64,64,64,73,64,73,73,73,73,10,10,10,10,19,19,19,12,12,13,14,101,100,
100,101,101,71,14,14,14,14,14,80,64,64,64,64,73,73,73,73,73,10,10,10,10,10,19,
12,12,12,13,14,101,100,101,101,209,64,64,64,80,64,80,64,64,64,64,73,64,73,73,73,
73,10,10,10,10,10,19,12,19,12,14,14,101,101,14,14,14,14,14,64,168,136,64,64,64,
64,73,64,73,73,73,73,73,10,10,10,10,10,19,19,19,19,13,14,13,13,14,14,209,64,64,
64,80,64,64,73,80,73,73,73,64,73,73,73,73,10,10,10,10,10,10,19,19,19,13,14,14,
14,13,14,14,63,14,14,14,63,80,80,64,64,64,64,73,73,73,73,73
  }
};

#endif // guard
