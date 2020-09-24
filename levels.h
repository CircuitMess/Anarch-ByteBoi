/**
  @file levels.h

  This file contains game levels and related code.

  by Miloslav Ciz (drummyfish), 2019

  Released under CC0 1.0 (https://creativecommons.org/publicdomain/zero/1.0/)
  plus a waiver of all other intellectual property. The goal of this work is
  be and remain completely in the public domain forever, available for any use
  whatsoever.
*/

#ifndef _SFG_LEVELS_H
#define _SFG_LEVELS_H

#define SFG_MAP_SIZE 64
#define SFG_TILE_DICTIONARY_SIZE 64

typedef uint16_t SFG_TileDefinition;
/**<
  Defines a single game map tile. The format is following:

    MSB aaabbbbb cccddddd LSB

   aaa:   ceiling texture index (from texture available on the map), 111
          means completely transparent texture
   bbbbb: ceiling height (1111 meaning no ceiling) ABOVE the floor
   ccc:   floor texture index, 111 means completely transparent texture
   ddddd: floor height
*/

#define SFG_TILE_CEILING_MAX_HEIGHT 31
#define SFG_TILE_TEXTURE_TRANSPARENT 7

typedef SFG_TileDefinition SFG_TileDictionary[SFG_TILE_DICTIONARY_SIZE];

/// helper macros for SFG_TileDefinition
#define SFG_TD(floorH, ceilH, floorT, ceilT)\
  ((floorH & 0x001f) |\
   ((floorT & 0x0007) << 5) |\
   ((ceilH & 0x001f) << 8) |\
   ((ceilT & 0x0007) << 13))

#define SFG_TILE_FLOOR_HEIGHT(tile) (tile & 0x1f)
#define SFG_TILE_FLOOR_TEXTURE(tile) ((tile & 0xe0) >> 5)
#define SFG_TILE_CEILING_HEIGHT(tile) ((tile & 0x1f00) >> 8)
#define SFG_TILE_CEILING_TEXTURE(tile) ((tile & 0xe000) >> 13)

#define SFG_OUTSIDE_TILE SFG_TD(63,0,7,7)

/**
  Game map represented as a 2D array. Array item has this format:

    MSB aabbbbbb LSB

    aa:     type of square, possible values:
      00:   normal
      01:   moving floor (elevator), moves from floor height to ceililing height
            (there is no ceiling above)
      10:   moving ceiling, moves from ceiling height to floor height
      11:   door
    bbbbbb: index into tile dictionary
*/
typedef uint8_t SFG_MapArray[SFG_MAP_SIZE * SFG_MAP_SIZE];

#define SFG_TILE_PROPERTY_MASK 0xc0
#define SFG_TILE_PROPERTY_NORMAL 0x00
#define SFG_TILE_PROPERTY_ELEVATOR 0x40
#define SFG_TILE_PROPERTY_SQUEEZER 0x80
#define SFG_TILE_PROPERTY_DOOR 0xc0

/**
  Serves to place elements (items, enemies etc.) into the game level.
*/
typedef struct
{
  uint8_t type;
  uint8_t coords[2];
} SFG_LevelElement;

#define SFG_MAX_LEVEL_ELEMENTS 128

/*
  Definitions of level element type. These values must leave the highest bit
  unused because that will be used by the game engine, so the values must be
  lower than 128.
*/
#define SFG_LEVEL_ELEMENT_NONE 0
#define SFG_LEVEL_ELEMENT_BARREL 0x01
#define SFG_LEVEL_ELEMENT_HEALTH 0x02
#define SFG_LEVEL_ELEMENT_BULLETS 0x03
#define SFG_LEVEL_ELEMENT_ROCKETS 0x04
#define SFG_LEVEL_ELEMENT_PLASMA 0x05
#define SFG_LEVEL_ELEMENT_TREE 0x06
#define SFG_LEVEL_ELEMENT_FINISH 0x07
#define SFG_LEVEL_ELEMENT_TELEPORT 0x08
#define SFG_LEVEL_ELEMENT_TERMINAL 0x09
#define SFG_LEVEL_ELEMENT_COLUMN 0x0a
#define SFG_LEVEL_ELEMENT_RUIN 0x0b
#define SFG_LEVEL_ELEMENT_LAMP 0x0c
#define SFG_LEVEL_ELEMENT_CARD0 0x0d ///< Access card, unlocks doors with lock.
#define SFG_LEVEL_ELEMENT_CARD1 0x0e
#define SFG_LEVEL_ELEMENT_CARD2 0x0f
#define SFG_LEVEL_ELEMENT_LOCK0 0x10 /**< Special level element that must be
                                     placed on a tile with door. This door is
                                     then unlocked by taking the corresponding
                                     access card. */
#define SFG_LEVEL_ELEMENT_LOCK1 0x11
#define SFG_LEVEL_ELEMENT_LOCK2 0x12
#define SFG_LEVEL_ELEMENT_BLOCKER 0x13 ///< Invisible wall.

#define SFG_LEVEL_ELEMENT_MONSTER_SPIDER 0x20
#define SFG_LEVEL_ELEMENT_MONSTER_DESTROYER 0x21
#define SFG_LEVEL_ELEMENT_MONSTER_WARRIOR 0x22
#define SFG_LEVEL_ELEMENT_MONSTER_PLASMABOT 0x23
#define SFG_LEVEL_ELEMENT_MONSTER_ENDER 0x24
#define SFG_LEVEL_ELEMENT_MONSTER_TURRET 0x25
#define SFG_LEVEL_ELEMENT_MONSTER_EXPLODER 0x26

#define SFG_MONSTERS_TOTAL 7

#define SFG_MONSTER_TYPE_TO_INDEX(monsterType) \
  ((monsterType) - SFG_LEVEL_ELEMENT_MONSTER_SPIDER)

#define SFG_MONSTER_INDEX_TO_TYPE(monsterIndex) \
  ((monsterIndex) + SFG_LEVEL_ELEMENT_MONSTER_SPIDER)

#define SFG_LEVEL_ELEMENT_TYPE_IS_MOSTER(t) \
  ((t) >= SFG_LEVEL_ELEMENT_MONSTER_SPIDER)

typedef struct
{
  SFG_MapArray mapArray;
  SFG_TileDictionary tileDictionary;
  uint8_t textureIndices[7]; /**< Says which textures are used on the map. There
                             can be at most 7 because of 3bit indexing (one
                             value is reserved for special transparent
                             texture). */
  uint8_t doorTextureIndex; /**< Index (global, NOT from textureIndices) of a
                             texture used for door. */
  uint8_t floorColor;
  uint8_t ceilingColor;
  uint8_t playerStart[3];   /**< Player starting location: square X, square Y,
                                 direction (fourths of RCL_Unit). */
  uint8_t backgroundImage;  /** Index of level background image. */
  SFG_LevelElement elements[SFG_MAX_LEVEL_ELEMENTS];
} SFG_Level;

static inline SFG_TileDefinition SFG_getMapTile
(
  const SFG_Level *level,
  int16_t x,
  int16_t y,
  uint8_t *properties
)
{
  if (x < 0 || x >= SFG_MAP_SIZE || y < 0 || y >= SFG_MAP_SIZE)
  {
    *properties = SFG_TILE_PROPERTY_NORMAL;
    return SFG_OUTSIDE_TILE;
  }

  uint8_t tile = level->mapArray[y * SFG_MAP_SIZE + x];

  *properties = tile & 0xc0;
  return level->tileDictionary[tile & 0x3f];
}

#define SFG_NUMBER_OF_LEVELS 10

/*
  NOTE: Initially the levels were stored sequentially in one big array, but that
  caused some issues with Arduino's PROGMEM, so now we store each level in a
  separate variable and eventually create an array of pointers to these.
*/

SFG_PROGRAM_MEMORY SFG_Level SFG_level0 =
  {          // level 0
    {        // mapArray
    #define o 0
    #define AA (37 | SFG_TILE_PROPERTY_ELEVATOR)
    #define BB (24 | SFG_TILE_PROPERTY_ELEVATOR)
    #define CC (16 | SFG_TILE_PROPERTY_DOOR)
    #define DD (18 | SFG_TILE_PROPERTY_ELEVATOR)
    #define EE (32 | SFG_TILE_PROPERTY_DOOR)
    // 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 
/*0 */ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,23,23,23,23,23,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,23,23,23,23,23,27,23,23,23,23,23,23,23,23,23,27,23,23,23,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o , /*0 */ 
/*1 */ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,23,23,23,23,23,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,23,23,27,27,23,1 ,29,29,29,27,27,27,29,29,29,1 ,23,27,27,29,29,29,29,29,29,29,29,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o , /*1 */ 
/*2 */ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,23,23,23,23,23,1 ,1 ,1 ,1 ,o ,o ,o ,o ,23,23,29,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,29,30,30,30,29,30,30,30,29,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o , /*2 */ 
/*3 */ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,23,23,23,23,23,1 ,1 ,1 ,1 ,o ,o ,o ,o ,o ,23,29,1 ,1 ,1 ,6 ,6 ,1 ,1 ,1 ,1 ,1 ,6 ,6 ,1 ,1 ,1 ,28,30,30,30,30,30,30,30,29,o ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,o ,o ,o ,o ,o ,o ,o ,o ,o , /*3 */ 
/*4 */ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,23,23,23,23,23,11,11,11,o ,o ,38,38,38,o ,23,29,1 ,1 ,o ,26,o ,o ,o ,o ,o ,o ,o ,26,o ,1 ,1 ,29,30,30,30,29,30,30,30,29,o ,5 ,15,15,15,15,15,5 ,o ,o ,o ,o ,o ,o ,o ,o ,o , /*4 */ 
/*5 */ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,23,23,23,23,23,11,11,11,o ,o ,38,38,38,o ,23,23,1 ,1 ,o ,26,o ,o ,o ,o ,o ,o ,o ,26,o ,1 ,1 ,23,29,29,29,29,30,30,30,29,o ,5 ,15,15,15,15,15,5 ,o ,o ,o ,o ,o ,o ,o ,o ,o , /*5 */ 
/*6 */ o ,o ,o ,o ,o ,o ,o ,23,23,23,23,23,11,11,11,o ,o ,38,38,38,o ,23,23,29,1 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,1 ,29,23,o ,o ,o ,29,30,30,30,29,o ,5 ,15,15,15,15,15,5 ,o ,o ,o ,o ,o ,o ,o ,o ,o , /*6 */ 
/*7 */ o ,o ,o ,o ,o ,o ,o ,23,23,23,23,23,11,11,11,o ,o ,38,38,38,o ,23,23,23,6 ,8 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,8 ,6 ,23,23,o ,o ,o ,29,29,29,29,29,o ,5 ,15,15,15,15,15,5 ,o ,o ,o ,o ,o ,o ,o ,o ,o , /*7 */ 
/*8 */ o ,o ,o ,o ,3 ,3 ,3 ,23,23,23,23,23,o ,o ,o ,o ,o ,o ,38,o ,o ,23,23,29,6 ,8 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,8 ,6 ,23,23,o ,o ,o ,o ,o ,o ,o ,o ,o ,5 ,15,15,15,15,15,5 ,o ,o ,o ,o ,o ,o ,o ,o ,o , /*8 */ 
/*9 */ o ,o ,o ,o ,3 ,o ,o ,o ,o ,3 ,o ,o ,o ,o ,o ,o ,o ,o ,38,o ,23,23,23,1 ,1 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,1 ,29,23,o ,o ,o ,o ,o ,o ,o ,o ,o ,5 ,15,15,15,15,15,5 ,o ,o ,o ,o ,o ,o ,o ,o ,o , /*9 */ 
/*10*/ o ,o ,o ,o ,3 ,o ,o ,o ,o ,3 ,o ,o ,o ,o ,o ,o ,o ,o ,38,38,38,38,38,AA,1 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,1 ,1 ,23,o ,o ,o ,o ,o ,5 ,5 ,5 ,5 ,5 ,15,15,15,5 ,5 ,5 ,o ,o ,o ,o ,o ,o ,o ,o ,o , /*10*/ 
/*11*/ o ,o ,o ,o ,3 ,o ,o ,o ,o ,3 ,3 ,3 ,3 ,o ,o ,o ,3 ,3 ,3 ,3 ,23,23,27,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,1 ,1 ,27,o ,o ,o ,o ,o ,5 ,15,15,15,15,15,15,15,5 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o , /*11*/ 
/*12*/ o ,o ,o ,o ,3 ,o ,o ,o ,o ,o ,o ,31,31,o ,o ,o ,31,31,o ,o ,23,23,27,27,23,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,6 ,6 ,27,o ,o ,o ,o ,o ,5 ,15,15,15,15,15,15,15,5 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o , /*12*/ 
/*13*/ 22,22,22,22,22,39,39,39,22,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,23,23,23,23,23,26,26,o ,o ,o ,o ,o ,26,26,25,25,BB,BB,27,o ,o ,o ,o ,o ,5 ,15,15,15,15,15,15,15,5 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,14,14,14, /*13*/ 
/*14*/ 21,21,21,21,21,21,21,21,22,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,23,23,23,23,23,o ,o ,o ,o ,o ,o ,o ,o ,o ,25,25,19,19,5 ,5 ,5 ,o ,o ,o ,5 ,15,15,15,5 ,5 ,5 ,5 ,5 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,14,14,14, /*14*/ 
/*15*/ 21,21,21,21,21,21,21,21,22,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,5 ,19,19,19,19,19,5 ,5 ,5 ,5 ,5 ,15,15,15,5 ,5 ,6 ,6 ,5 ,5 ,o ,1 ,1 ,1 ,1 ,1 ,1 ,o ,14,14, /*15*/ 
/*16*/ 21,21,21,21,21,21,21,21,22,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,8 ,8 ,8 ,36,o ,o ,o ,5 ,19,19,19,19,19,5 ,o ,o ,o ,19,15,15,15,5 ,o ,o ,o ,o ,3 ,o ,1 ,1 ,1 ,1 ,1 ,1 ,o ,14,14, /*16*/ 
/*17*/ 22,22,22,22,22,21,21,21,22,31,31,o ,o ,31,31,o ,o ,o ,31,31,o ,o ,o ,31,31,o ,12,9 ,9 ,9 ,12,o ,31,31,5 ,19,19,19,19,19,5 ,o ,o ,o ,5 ,17,17,17,5 ,26,o ,o ,26,3 ,o ,o ,o ,o ,o ,o ,o ,o ,14,14, /*17*/ 
/*18*/ o ,o ,o ,o ,22,20,20,20,39,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,12,10,10,10,12,2 ,2 ,2 ,2 ,5 ,5 ,6 ,CC,6 ,5 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,3 ,3 ,o ,o ,o ,o ,o ,o ,14,14, /*18*/ 
/*19*/ o ,o ,o ,o ,22,21,21,21,39,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,12,10,10,10,12,2 ,2 ,2 ,2 ,5 ,5 ,19,19,19,5 ,o ,o ,o ,o ,o ,o ,o ,o ,8 ,8 ,8 ,26,o ,o ,3 ,o ,o ,o ,o ,o ,o ,14,14, /*19*/ 
/*20*/ o ,o ,o ,o ,39,20,20,20,39,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,12,10,10,10,12,2 ,2 ,2 ,2 ,6 ,5 ,15,15,15,19,11,10,9 ,8 ,o ,o ,o ,o ,8 ,8 ,8 ,26,o ,o ,3 ,o ,o ,o ,o ,o ,o ,14,14, /*20*/ 
/*21*/ o ,o ,o ,o ,39,21,21,21,39,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,12,10,10,10,12,2 ,2 ,2 ,2 ,6 ,5 ,15,15,15,19,11,10,9 ,8 ,o ,o ,o ,o ,8 ,8 ,8 ,26,o ,o ,3 ,o ,o ,o ,o ,o ,o ,14,14, /*21*/ 
/*22*/ o ,o ,o ,o ,22,20,20,20,22,DD,o ,o ,o ,o ,o ,o ,26,26,26,26,26,o ,o ,o ,o ,o ,12,9 ,9 ,9 ,12,o ,2 ,2 ,2 ,6 ,5 ,15,15,15,19,11,10,9 ,8 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,o ,o ,o ,o ,o ,o ,14,14, /*22*/ 
/*23*/ o ,o ,o ,o ,22,21,21,21,22,12,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,8 ,8 ,8 ,36,o ,2 ,2 ,2 ,5 ,5 ,15,15,15,19,11,10,9 ,8 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,o ,o ,o ,o ,o ,o ,14,14, /*23*/ 
/*24*/ o ,o ,o ,o ,39,21,21,21,21,11,10,9 ,8 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,2 ,2 ,2 ,6 ,5 ,19,19,19,5 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,o ,o ,o ,o ,o ,o ,14,14, /*24*/ 
/*25*/ o ,o ,o ,o ,39,21,21,21,21,11,10,9 ,8 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,2 ,2 ,2 ,6 ,5 ,15,15,15,5 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,3 ,3 ,o ,o ,o ,o ,o ,o ,14,14, /*25*/ 
/*26*/ o ,o ,o ,o ,22,21,21,21,22,12,o ,o ,o ,o ,o ,o ,8 ,8 ,o ,o ,o ,8 ,8 ,o ,o ,o ,8 ,8 ,o ,o ,o ,o ,DD,2 ,2 ,6 ,5 ,15,15,15,5 ,o ,o ,o ,5 ,17,17,17,5 ,26,o ,o ,26,3 ,o ,o ,o ,o ,o ,o ,o ,o ,14,14, /*26*/ 
/*27*/ o ,o ,o ,o ,22,20,20,20,22,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,31,31,2 ,2 ,2 ,5 ,5 ,15,15,15,5 ,o ,o ,o ,5 ,15,15,15,5 ,o ,o ,o ,o ,3 ,o ,1 ,1 ,1 ,1 ,1 ,1 ,o ,14,14, /*27*/ 
/*28*/ o ,o ,o ,o ,39,21,21,21,39,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,2 ,2 ,2 ,2 ,2 ,5 ,15,15,15,15,5 ,5 ,5 ,5 ,5 ,19,19,19,5 ,5 ,6 ,6 ,5 ,5 ,o ,1 ,1 ,1 ,1 ,1 ,1 ,3 ,14,14, /*28*/ 
/*29*/ o ,o ,o ,o ,39,20,20,20,39,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,2 ,2 ,2 ,2 ,2 ,5 ,5 ,15,15,15,15,15,15,15,19,15,15,15,5 ,o ,o ,o ,o ,o ,o ,1 ,1 ,1 ,1 ,1 ,1 ,3 ,14,14, /*29*/ 
/*30*/ o ,o ,o ,o ,22,21,21,21,39,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,2 ,2 ,2 ,2 ,2 ,5 ,15,15,15,15,15,15,15,15,19,15,15,15,5 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,14,14,14, /*30*/ 
/*31*/ o ,o ,o ,o ,22,20,20,20,39,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,2 ,2 ,2 ,2 ,2 ,5 ,5 ,15,15,15,15,15,15,15,19,15,15,15,5 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,14,14,14, /*31*/ 
/*32*/ o ,o ,o ,o ,39,21,21,21,22,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,1 ,1 ,1 ,1 ,5 ,2 ,2 ,2 ,2 ,2 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,19,19,19,5 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,14,14,14, /*32*/ 
/*33*/ o ,o ,o ,o ,39,21,21,21,22,40,40,40,40,40,40,40,40,40,26,35,31,o ,o ,31,35,31,o ,o ,31,35,2 ,2 ,2 ,2 ,2 ,6 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,15,15,15,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 , /*33*/ 
/*34*/ o ,o ,o ,o ,22,21,21,21,21,11,10,40,40,40,40,40,40,40,26,35,31,o ,o ,31,35,31,o ,o ,31,35,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,5 ,1 ,1 ,1 ,5 ,15,15,15,5 ,o ,14,14,14,14,14,14,14,o ,o ,o ,o ,o ,o ,14, /*34*/ 
/*35*/ o ,o ,o ,o ,22,21,21,21,21,11,10,40,40,40,40,40,40,40,26,35,o ,o ,o ,o ,35,o ,o ,o ,o ,35,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,6 ,1 ,1 ,1 ,5 ,15,15,15,5 ,o ,14,14,14,14,14,14,14,o ,o ,o ,o ,o ,o ,14, /*35*/ 
/*36*/ o ,o ,o ,o ,22,21,21,21,21,11,10,40,40,40,40,40,40,40,40,35,o ,o ,o ,o ,35,o ,o ,o ,o ,35,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,6 ,1 ,1 ,1 ,5 ,15,15,15,5 ,6 ,14,14,14,14,14,14,14,3 ,3 ,3 ,3 ,o ,o ,14, /*36*/ 
/*37*/ o ,o ,o ,o ,22,39,39,39,22,40,40,40,40,40,40,40,40,40,40,35,35,o ,o ,35,35,35,o ,o ,35,35,31,31,31,31,6 ,6 ,6 ,6 ,2 ,2 ,6 ,1 ,1 ,1 ,1 ,6 ,CC,6 ,1 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,o ,14,14, /*37*/ 
/*38*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,10,34,10,34,40,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,1 ,1 ,1 ,6 ,2 ,2 ,5 ,1 ,1 ,1 ,11,11,11,11,11,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,o ,14,14, /*38*/ 
/*39*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,10,34,10,34,40,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,1 ,1 ,1 ,6 ,2 ,2 ,6 ,1 ,1 ,1 ,11,11,11,11,11,o ,o ,o ,o ,12,o ,o ,o ,o ,o ,o ,3 ,o ,14,14, /*39*/ 
/*40*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,35,35,35,35,35,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,1 ,1 ,1 ,6 ,2 ,2 ,6 ,1 ,1 ,1 ,11,11,11,11,11,11,11,11,11,12,11,10,9 ,8 ,o ,o ,3 ,o ,14,14, /*40*/ 
/*41*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,31,o ,o ,o ,35,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,1 ,1 ,1 ,6 ,2 ,2 ,6 ,1 ,1 ,1 ,11,11,11,11,11,11,11,11,11,12,11,10,9 ,8 ,o ,o ,3 ,o ,14,14, /*41*/ 
/*42*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,31,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,5 ,1 ,1 ,6 ,2 ,2 ,5 ,1 ,1 ,1 ,11,11,11,11,11,11,11,11,11,12,11,10,9 ,8 ,o ,o ,3 ,o ,o ,1 , /*42*/ 
/*43*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,31,o ,o ,o ,35,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,3 ,3 ,3 ,o ,o ,o ,o ,o ,o ,o ,26,2 ,2 ,6 ,1 ,1 ,1 ,11,11,11,11,11,11,4 ,o ,o ,12,o ,o ,o ,o ,o ,o ,3 ,o ,o ,1 , /*43*/ 
/*44*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,35,35,35,35,35,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,40,40,o ,o ,o ,o ,o ,o ,o ,o ,26,2 ,2 ,6 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,5 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,o ,o ,1 , /*44*/ 
/*45*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,o ,o ,o ,3 ,3 ,3 ,3 ,3 ,40,40,o ,o ,o ,o ,o ,o ,o ,o ,26,2 ,2 ,6 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,o ,o ,1 , /*45*/ 
/*46*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,40,40,o ,o ,o ,o ,o ,o ,o ,o ,31,2 ,2 ,5 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,5 ,1 ,1 ,1 ,1 ,1 ,o ,o ,1 , /*46*/ 
/*47*/ 1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,23,23,23,23,23,29,o ,o ,o ,o ,o ,31,2 ,2 ,6 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,1 ,1 ,1 ,1 ,1 ,1 ,o ,o ,1 , /*47*/ 
/*48*/ 1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,23,23,23,23,23,23,o ,o ,o ,o ,o ,31,2 ,2 ,6 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,8 ,o ,o ,8 ,1 ,1 ,1 ,1 ,1 ,1 ,o ,o ,1 , /*48*/ 
/*49*/ 1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,o ,o ,o ,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,23,23,23,23,23,23,23,29,o ,o ,o ,26,2 ,2 ,6 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*49*/ 
/*50*/ 1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,o ,o ,o ,3 ,o ,o ,o ,o ,o ,o ,11,11,11,o ,o ,o ,o ,23,23,23,23,23,23,23,23,o ,o ,o ,26,2 ,2 ,5 ,5 ,6 ,1 ,1 ,1 ,6 ,1 ,1 ,1 ,5 ,8 ,o ,o ,8 ,6 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*50*/ 
/*51*/ 1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,o ,o ,o ,3 ,o ,o ,o ,8 ,9 ,10,11,11,11,10,9 ,8 ,o ,23,23,23,23,23,27,23,29,o ,o ,o ,26,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,o ,o ,o ,o ,o ,o ,o ,o ,6 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*51*/ 
/*52*/ 1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,o ,o ,o ,3 ,3 ,3 ,o ,8 ,9 ,10,11,11,11,10,9 ,8 ,o ,23,23,23,27,1 ,1 ,9 ,9 ,o ,o ,o ,o ,o ,o ,o ,o ,2 ,2 ,2 ,2 ,2 ,o ,o ,o ,o ,o ,o ,o ,o ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*52*/ 
/*53*/ 1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,23,27,1 ,27,15,15,15,27,1 ,27,23,23,23,23,27,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,2 ,2 ,2 ,2 ,2 ,o ,o ,o ,o ,o ,o ,o ,o ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*53*/ 
/*54*/ 1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,23,23,23,27,15,15,15,27,23,23,23,23,23,33,EE,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,2 ,2 ,2 ,2 ,2 ,o ,o ,o ,o ,o ,o ,o ,o ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*54*/ 
/*55*/ 1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,23,23,23,27,15,15,15,27,23,23,23,23,23,23,27,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,2 ,2 ,2 ,2 ,2 ,o ,o ,o ,8 ,8 ,8 ,8 ,8 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*55*/ 
/*56*/ 1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,23,15,15,15,15,15,15,15,15,15,23,23,23,23,27,1 ,1 ,9 ,9 ,o ,o ,o ,o ,o ,o ,o ,o ,2 ,2 ,2 ,2 ,2 ,o ,o ,o ,8 ,8 ,8 ,8 ,8 ,5 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*56*/ 
/*57*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,o ,23,15,15,15,15,15,15,15,15,15,23,23,23,23,23,23,27,23,29,o ,o ,o ,o ,o ,o ,o ,o ,2 ,2 ,2 ,2 ,DD,o ,o ,o ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,9 ,10,11,11,3 ,1 ,1 , /*57*/ 
/*58*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,o ,23,15,15,23,23,23,23,23,15,15,23,23,23,23,23,23,23,23,23,o ,o ,o ,o ,o ,o ,o ,o ,2 ,2 ,2 ,2 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,1 ,1 ,1 ,11,3 ,1 ,1 , /*58*/ 
/*59*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,o ,23,15,15,23,23,23,23,23,15,15,23,23,23,23,23,23,23,23,29,o ,o ,o ,o ,o ,o ,o ,o ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,8 ,8 ,8 ,8 ,1 ,11,3 ,1 ,1 , /*59*/ 
/*60*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,o ,23,15,15,23,23,23,23,23,15,15,23,23,23,23,23,23,23,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,8 ,8 ,8 ,8 ,8 ,8 ,1 ,1 ,1 , /*60*/ 
/*61*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,o ,23,15,15,23,23,23,23,23,15,15,23,23,23,23,23,23,29,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,8 ,8 ,8 ,8 ,8 ,8 ,1 ,1 ,1 , /*61*/ 
/*62*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,o ,o ,o ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,8 ,8 ,1 ,1 ,1 , /*62*/ 
/*63*/ 1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,o ,o ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,3 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,3 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,3 ,3 ,3 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,8 ,8 ,o ,o ,o   /*63*/ 
    // 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 
    #undef AA
    #undef BB
    #undef CC
    #undef DD
    #undef EE
    #undef o
    },
    {        // tileDictionary
      SFG_TD( 7,31,1,0),SFG_TD(26,31,0,0),SFG_TD( 0,31,0,0),SFG_TD(14,31,2,0), // 0 
      SFG_TD(11,31,2,0),SFG_TD(26,31,2,0),SFG_TD(26,31,3,0),SFG_TD( 7,11,0,0), // 4 
      SFG_TD( 8,31,4,0),SFG_TD( 9,31,4,0),SFG_TD(10,31,4,0),SFG_TD(11,31,4,0), // 8 
      SFG_TD(11,31,2,0),SFG_TD( 7,11,4,0),SFG_TD(30,31,0,0),SFG_TD(11, 7,2,0), // 12 
      SFG_TD(15, 0,4,3),SFG_TD(13, 2,2,2),SFG_TD( 0, 7,5,0),SFG_TD(11, 5,2,2), // 16 
      SFG_TD(11,31,2,0),SFG_TD(11, 7,2,7),SFG_TD(18, 0,2,7),SFG_TD( 7, 0,0,0), // 20 
      SFG_TD(11,15,5,0),SFG_TD(30,31,2,0),SFG_TD(12,31,6,0),SFG_TD( 7, 0,3,3), // 24 
      SFG_TD(26, 4,0,0),SFG_TD(31, 0,2,0),SFG_TD(26, 5,0,0),SFG_TD(10,31,6,0), // 28 
      SFG_TD(11, 0,1,0),SFG_TD( 7, 4,1,0),SFG_TD(10,31,2,0),SFG_TD(14,31,4,0), // 32 
      SFG_TD(14,31,2,0),SFG_TD( 3,23,5,0),SFG_TD( 3, 4,1,0),SFG_TD(18,31,4,0), // 36 
      SFG_TD( 8,31,2,0),SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0), // 40 
      SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0), // 44 
      SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0), // 48 
      SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0), // 52 
      SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0), // 56 
      SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0),SFG_TD( 7,11,0,0)  // 60 
      },                    // tileDictionary
    {1 ,3 ,12,14,5 ,11,6 }, // textureIndices
    13,                     // doorTextureIndex
    20,                     // floorColor
    3,                      // ceilingColor
    {52, 55, 80 },          // player start: x, y, direction
    0,                      // backgroundImage
    {                       // elements
      {SFG_LEVEL_ELEMENT_BULLETS, {31,2}},{SFG_LEVEL_ELEMENT_BULLETS, {29,2}},
      {SFG_LEVEL_ELEMENT_BULLETS, {35,4}},{SFG_LEVEL_ELEMENT_BULLETS, {25,4}},
      {SFG_LEVEL_ELEMENT_BULLETS, {51,5}},{SFG_LEVEL_ELEMENT_ROCKETS, {50,5}},
      {SFG_LEVEL_ELEMENT_TELEPORT, {18,5}},{SFG_LEVEL_ELEMENT_BULLETS, {51,6}},
      {SFG_LEVEL_ELEMENT_ROCKETS, {50,6}},{SFG_LEVEL_ELEMENT_TELEPORT, {50,9}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {29,13}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {36,14}},
      {SFG_LEVEL_ELEMENT_TERMINAL, {35,15}},{SFG_LEVEL_ELEMENT_LAMP, {32,15}},
      {SFG_LEVEL_ELEMENT_RUIN, {19,15}},{SFG_LEVEL_ELEMENT_BULLETS, {49,16}},
      {SFG_LEVEL_ELEMENT_RUIN, {18,16}},{SFG_LEVEL_ELEMENT_RUIN, {19,17}},
      {SFG_LEVEL_ELEMENT_LOCK0, {38,18}},{SFG_LEVEL_ELEMENT_CARD0, {50,20}},
      {SFG_LEVEL_ELEMENT_RUIN, {6,21}},{SFG_LEVEL_ELEMENT_RUIN, {5,21}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {46,22}},{SFG_LEVEL_ELEMENT_RUIN, {7,22}},
      {SFG_LEVEL_ELEMENT_LAMP, {9,23}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {16,24}},
      {SFG_LEVEL_ELEMENT_LAMP, {31,25}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {16,25}},
      {SFG_LEVEL_ELEMENT_LAMP, {9,26}},{SFG_LEVEL_ELEMENT_HEALTH, {49,27}},
      {SFG_LEVEL_ELEMENT_BULLETS, {47,27}},{SFG_LEVEL_ELEMENT_BULLETS, {45,27}},
      {SFG_LEVEL_ELEMENT_LAMP, {36,28}},{SFG_LEVEL_ELEMENT_LAMP, {36,30}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {34,32}},{SFG_LEVEL_ELEMENT_HEALTH, {30,33}},
      {SFG_LEVEL_ELEMENT_BULLETS, {27,33}},{SFG_LEVEL_ELEMENT_BULLETS, {22,33}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {21,36}},{SFG_LEVEL_ELEMENT_TREE, {53,39}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {46,39}},{SFG_LEVEL_ELEMENT_TREE, {15,39}},
      {SFG_LEVEL_ELEMENT_TREE, {13,39}},{SFG_LEVEL_ELEMENT_BULLETS, {44,41}},
      {SFG_LEVEL_ELEMENT_TREE, {53,43}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {32,43}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {31,43}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {30,43}},
      {SFG_LEVEL_ELEMENT_RUIN, {21,45}},{SFG_LEVEL_ELEMENT_RUIN, {20,45}},
      {SFG_LEVEL_ELEMENT_RUIN, {19,45}},{SFG_LEVEL_ELEMENT_LAMP, {54,48}},
      {SFG_LEVEL_ELEMENT_BARREL, {53,48}},{SFG_LEVEL_ELEMENT_BARREL, {52,48}},
      {SFG_LEVEL_ELEMENT_LAMP, {51,48}},{SFG_LEVEL_ELEMENT_LAMP, {54,50}},
      {SFG_LEVEL_ELEMENT_LAMP, {51,50}},{SFG_LEVEL_ELEMENT_RUIN, {40,51}},
      {SFG_LEVEL_ELEMENT_TREE, {32,52}},{SFG_LEVEL_ELEMENT_FINISH, {28,54}},
      {SFG_LEVEL_ELEMENT_BULLETS, {49,55}},{SFG_LEVEL_ELEMENT_BULLETS, {48,55}},
      {SFG_LEVEL_ELEMENT_BULLETS, {49,56}},{SFG_LEVEL_ELEMENT_BULLETS, {48,56}},
      {SFG_LEVEL_ELEMENT_TREE, {32,56}},{SFG_LEVEL_ELEMENT_BLOCKER, {58,57}},
      {SFG_LEVEL_ELEMENT_RUIN, {46,59}},{SFG_LEVEL_ELEMENT_RUIN, {46,60}},
      {SFG_LEVEL_ELEMENT_RUIN, {44,61}},{SFG_LEVEL_ELEMENT_RUIN, {43,61}},
      {SFG_LEVEL_ELEMENT_LAMP, {42,61}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}}
    }, // elements
  }; // level

SFG_PROGRAM_MEMORY SFG_Level SFG_level1 =
  {          // level 1
    {        // mapArray
    #define o 0
    #define AA (21 | SFG_TILE_PROPERTY_DOOR)
    #define BB (23 | SFG_TILE_PROPERTY_ELEVATOR)
    #define CC (3 | SFG_TILE_PROPERTY_SQUEEZER)
    #define DD (31 | SFG_TILE_PROPERTY_ELEVATOR)
    #define EE (1 | SFG_TILE_PROPERTY_DOOR)
    #define FF (22 | SFG_TILE_PROPERTY_DOOR)
    #define GG (50 | SFG_TILE_PROPERTY_ELEVATOR)
    #define HH (49 | SFG_TILE_PROPERTY_ELEVATOR)
    #define II (48 | SFG_TILE_PROPERTY_ELEVATOR)
    // 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 
/*0 */ 4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,6 ,7 ,7 ,7 ,7 ,6 ,7 ,7 ,7 ,7 ,6 ,6 ,6 ,6 ,6 ,7 ,7 ,6 ,6 ,6 ,9 ,9 ,9 ,9 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,12,6 ,6 ,6 ,12,6 ,6 ,6 ,12,6 ,6 ,6 ,12,6 ,6 ,6 ,12,6 ,6 , /*0 */ 
/*1 */ 4 ,4 ,4 ,4 ,5 ,5 ,5 ,4 ,4 ,4 ,4 ,4 ,6 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,21,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,7 ,9 ,9 ,9 ,9 ,6 ,5 ,5 ,5 ,5 ,5 ,21,6 ,5 ,5 ,5 ,57,57,5 ,5 ,5 ,13,16,16,16,16,16,16,16,16,16,16,6 , /*1 */ 
/*2 */ 4 ,4 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,4 ,4 ,4 ,6 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,21,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,7 ,9 ,9 ,9 ,9 ,6 ,28,28,28,5 ,5 ,21,4 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,16,16,16,16,16,16,16,16,16,16,16,15, /*2 */ 
/*3 */ 4 ,4 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,4 ,4 ,4 ,6 ,54,54,54,54,4 ,8 ,8 ,8 ,8 ,21,8 ,8 ,4 ,54,54,54,54,54,6 ,9 ,9 ,9 ,9 ,6 ,21,21,28,5 ,5 ,21,21,57,5 ,5 ,5 ,5 ,5 ,5 ,5 ,16,16,16,16,16,16,16,16,16,16,16,6 , /*3 */ 
/*4 */ 4 ,5 ,5 ,5 ,9 ,9 ,9 ,5 ,5 ,5 ,4 ,4 ,4 ,8 ,8 ,8 ,8 ,54,8 ,8 ,8 ,8 ,21,8 ,8 ,54,8 ,8 ,8 ,8 ,8 ,6 ,6 ,6 ,6 ,6 ,6 ,21,21,28,5 ,5 ,21,39,57,5 ,5 ,5 ,5 ,5 ,5 ,5 ,13,16,16,16,16,16,16,16,16,16,16,6 , /*4 */ 
/*5 */ 4 ,5 ,5 ,5 ,9 ,9 ,9 ,5 ,5 ,5 ,8 ,8 ,AA,8 ,8 ,8 ,8 ,54,8 ,8 ,8 ,8 ,21,8 ,8 ,54,8 ,8 ,8 ,8 ,8 ,6 ,16,16,16,16,16,21,21,28,5 ,5 ,21,13,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,6 , /*5 */ 
/*6 */ 4 ,5 ,5 ,5 ,9 ,9 ,9 ,5 ,5 ,5 ,4 ,4 ,4 ,8 ,8 ,8 ,8 ,54,8 ,8 ,8 ,8 ,21,8 ,8 ,54,8 ,8 ,8 ,8 ,8 ,6 ,16,16,16,16,16,63,21,28,5 ,5 ,21,39,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,57,6 , /*6 */ 
/*7 */ 9 ,4 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,4 ,4 ,4 ,6 ,8 ,8 ,8 ,8 ,4 ,21,21,21,21,21,AA,21,4 ,8 ,8 ,8 ,8 ,8 ,6 ,16,16,16,16,16,63,21,28,5 ,5 ,21,21,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,57,6 , /*7 */ 
/*8 */ 9 ,4 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,4 ,4 ,4 ,6 ,8 ,8 ,21,21,4 ,54,54,54,54,54,54,54,4 ,21,21,21,BB,BB,27,16,16,16,16,16,21,21,28,5 ,5 ,21,39,5 ,5 ,5 ,5 ,5 ,5 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 , /*8 */ 
/*9 */ 10,10,4 ,4 ,5 ,5 ,5 ,4 ,4 ,4 ,4 ,4 ,6 ,8 ,8 ,8 ,8 ,54,8 ,8 ,8 ,8 ,8 ,8 ,8 ,54,8 ,8 ,21,BB,BB,27,16,16,17,17,17,21,21,28,5 ,5 ,21,13,5 ,5 ,5 ,5 ,5 ,5 ,4 ,5 ,5 ,5 ,5 ,53,53,53,53,5 ,5 ,5 ,56,6 , /*9 */ 
/*10*/ 10,10,10,9 ,4 ,3 ,4 ,4 ,4 ,4 ,4 ,4 ,6 ,8 ,8 ,8 ,8 ,54,8 ,8 ,8 ,8 ,8 ,8 ,8 ,54,8 ,8 ,21,BB,BB,27,16,16,18,18,18,21,21,28,5 ,5 ,21,21,5 ,5 ,5 ,5 ,5 ,5 ,20,5 ,5 ,5 ,5 ,53,53,53,53,5 ,5 ,5 ,57,6 , /*10*/ 
/*11*/ 10,10,10,9 ,9 ,9 ,9 ,1 ,6 ,6 ,15,6 ,6 ,6 ,6 ,6 ,6 ,6 ,7 ,7 ,7 ,7 ,7 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,19,19,19,21,21,28,5 ,5 ,21,21,59,5 ,5 ,5 ,5 ,5 ,21,5 ,5 ,5 ,5 ,53,53,53,53,1 ,13,13,13,1 , /*11*/ 
/*12*/ 10,10,10,9 ,9 ,9 ,9 ,1 ,12,16,16,16,16,16,16,12,5 ,56,28,28,28,28,28,60,58,58,5 ,5 ,5 ,5 ,5 ,5 ,21,21,20,20,20,21,21,28,5 ,5 ,21,21,5 ,5 ,59,5 ,5 ,5 ,21,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,1 ,o ,o ,o ,1 , /*12*/ 
/*13*/ 9 ,11,9 ,9 ,9 ,9 ,11,1 ,6 ,16,16,16,16,16,16,16,5 ,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,21,28,28,28,28,28,28,28,28,28,4 ,21,21,21,21,20,20,20,21,1 ,1 ,1 ,1 ,2 ,4 ,1 ,1 ,4 ,3 ,3 ,4 ,1 , /*13*/ 
/*14*/ 9 ,11,9 ,9 ,9 ,9 ,11,1 ,6 ,16,16,16,16,16,16,16,5 ,25,5 ,5 ,5 ,5 ,5 ,5 ,56,5 ,5 ,5 ,5 ,5 ,5 ,5 ,20,28,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,21,21,21,21,o ,CC,CC,CC,o ,21,1 ,4 ,o ,o ,o ,o ,o ,o ,14,14,o ,1 , /*14*/ 
/*15*/ 9 ,11,11,11,9 ,9 ,9 ,1 ,12,16,16,16,16,16,16,12,5 ,25,5 ,5 ,5 ,5 ,5 ,59,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,20,28,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,21,21,21,21,21,28,28,28,21,21,1 ,3 ,o ,o ,o ,o ,o ,o ,14,14,o ,1 , /*15*/ 
/*16*/ 10,10,11,11,9 ,9 ,9 ,1 ,6 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,25,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,20,28,5 ,5 ,5 ,5 ,5 ,5 ,5 ,60,29,5 ,5 ,21,21,53,53,53,21,21,1 ,3 ,o ,o ,o ,o ,o ,o ,14,14,o ,1 , /*16*/ 
/*17*/ 10,10,9 ,9 ,9 ,9 ,9 ,1 ,6 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,25,25,25,25,25,25,25,25,25,25,25,25,25,25,25,21,28,5 ,5 ,5 ,5 ,5 ,5 ,5 ,60,29,5 ,5 ,53,53,53,53,53,53,53,1 ,3 ,o ,o ,o ,o ,o ,o ,14,14,o ,1 , /*17*/ 
/*18*/ 10,10,9 ,9 ,9 ,9 ,9 ,1 ,6 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,58,58,5 ,5 ,5 ,5 ,21,4 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,29,5 ,5 ,53,53,53,53,53,53,53,1 ,3 ,o ,o ,o ,o ,o ,o ,14,14,o ,1 , /*18*/ 
/*19*/ 10,9 ,9 ,9 ,9 ,9 ,9 ,4 ,21,21,20,20,21,6 ,6 ,58,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,59,5 ,6 ,6 ,21,21,21,21,21,24,24,24,24,24,24,24,24,5 ,21,5 ,5 ,53,53,53,53,53,53,53,1 ,3 ,o ,o ,o ,o ,13,o ,14,14,o ,1 , /*19*/ 
/*20*/ 10,10,10,9 ,9 ,9 ,9 ,8 ,8 ,8 ,20,20,8 ,6 ,6 ,58,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,6 ,6 ,31,30,30,31,31,31,31,31,31,31,30,31,31,6 ,6 ,5 ,5 ,53,53,53,53,53,53,53,1 ,3 ,o ,o ,o ,o ,1 ,o ,14,14,o ,1 , /*20*/ 
/*21*/ 9 ,10,10,9 ,9 ,9 ,9 ,8 ,8 ,8 ,20,20,8 ,6 ,6 ,5 ,5 ,59,5 ,5 ,5 ,5 ,5 ,5 ,5 ,56,6 ,6 ,31,31,31,31,31,31,31,31,31,31,30,31,31,6 ,DD,8 ,8 ,53,53,53,53,53,53,53,1 ,3 ,3 ,3 ,3 ,3 ,1 ,3 ,3 ,3 ,3 ,1 , /*21*/ 
/*22*/ 11,11,9 ,9 ,9 ,9 ,9 ,4 ,8 ,8 ,20,20,8 ,6 ,6 ,58,58,5 ,5 ,5 ,5 ,5 ,5 ,56,5 ,59,6 ,6 ,31,30,30,30,31,31,31,31,31,31,30,31,31,6 ,31,8 ,8 ,53,53,53,53,53,53,53,1 ,o ,o ,o ,o ,3 ,1 ,o ,14,14,o ,1 , /*22*/ 
/*23*/ 11,11,9 ,9 ,9 ,9 ,9 ,21,8 ,8 ,20,20,8 ,8 ,8 ,58,58,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,57,6 ,6 ,31,30,31,30,30,31,31,31,31,31,30,31,31,31,31,8 ,8 ,53,53,53,53,53,53,53,1 ,o ,o ,o ,o ,3 ,1 ,o ,14,14,o ,1 , /*23*/ 
/*24*/ 9 ,10,10,10,9 ,9 ,9 ,21,8 ,8 ,20,20,8 ,8 ,8 ,62,58,5 ,5 ,5 ,5 ,57,61,5 ,61,57,6 ,6 ,31,30,31,31,30,30,30,30,30,30,30,31,31,6 ,6 ,6 ,6 ,1 ,1 ,1 ,1 ,EE,1 ,1 ,1 ,o ,o ,o ,o ,3 ,1 ,o ,14,14,o ,1 , /*24*/ 
/*25*/ 9 ,10,10,10,9 ,9 ,9 ,21,8 ,5 ,5 ,5 ,8 ,6 ,6 ,57,61,56,5 ,5 ,5 ,57,5 ,5 ,56,58,6 ,6 ,31,30,31,31,31,31,31,31,31,31,31,31,31,6 ,58,o ,o ,o ,1 ,o ,o ,o ,o ,o ,61,o ,o ,o ,o ,3 ,1 ,1 ,19,19,1 ,1 , /*25*/ 
/*26*/ 9 ,10,10,10,9 ,9 ,9 ,21,8 ,5 ,5 ,5 ,8 ,6 ,12,57,5 ,5 ,5 ,5 ,5 ,5 ,56,5 ,5 ,5 ,12,6 ,31,30,31,31,31,31,31,31,31,31,31,31,31,6 ,58,o ,o ,o ,2 ,o ,o ,o ,o ,o ,EE,o ,o ,o ,o ,3 ,1 ,1 ,18,18,1 ,1 , /*26*/ 
/*27*/ 9 ,11,11,9 ,9 ,9 ,9 ,20,5 ,5 ,5 ,5 ,8 ,6 ,6 ,61,5 ,5 ,5 ,5 ,5 ,5 ,5 ,59,58,5 ,6 ,6 ,31,31,31,31,31,31,31,31,31,31,31,31,31,6 ,1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,3 ,61,3 ,3 ,3 ,3 ,4 ,1 ,1 ,17,17,1 ,1 , /*27*/ 
/*28*/ 9 ,11,11,9 ,9 ,9 ,9 ,21,8 ,8 ,8 ,8 ,8 ,21,6 ,58,5 ,5 ,5 ,5 ,5 ,5 ,5 ,56,5 ,5 ,6 ,6 ,31,30,31,31,31,31,31,31,31,31,31,31,31,6 ,58,o ,o ,o ,1 ,o ,o ,o ,o ,3 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,16,16,16,16,1 , /*28*/ 
/*29*/ 1 ,1 ,1 ,1 ,1 ,1 ,20,20,20,20,20,20,20,21,6 ,58,5 ,5 ,5 ,5 ,5 ,5 ,5 ,58,5 ,56,6 ,6 ,31,30,31,30,30,30,30,30,30,30,31,31,31,6 ,58,o ,o ,o ,2 ,o ,o ,o ,o ,3 ,EE,5 ,5 ,5 ,5 ,5 ,1 ,16,16,16,16,1 , /*29*/ 
/*30*/ 1 ,58,58,57,57,57,21,21,21,21,20,20,20,21,6 ,56,5 ,5 ,5 ,5 ,5 ,5 ,5 ,57,5 ,60,6 ,6 ,31,30,30,30,31,31,31,31,31,30,30,31,31,6 ,58,o ,o ,o ,1 ,o ,o ,o ,o ,3 ,1 ,o ,o ,o ,5 ,5 ,1 ,16,16,16,16,1 , /*30*/ 
/*31*/ 1 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,1 ,21,20,20,20,21,6 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,59,5 ,6 ,6 ,31,31,31,31,31,31,31,31,31,31,30,31,31,6 ,58,o ,o ,1 ,1 ,o ,o ,o ,o ,3 ,1 ,o ,o ,o ,5 ,5 ,1 ,16,16,16,16,1 , /*31*/ 
/*32*/ 1 ,5 ,58,58,5 ,1 ,5 ,57,57,21,20,20,20,21,21,21,21,21,20,20,20,20,21,4 ,7 ,7 ,6 ,6 ,31,31,31,31,31,31,31,31,31,31,30,31,31,6 ,58,o ,o ,o ,EE,o ,o ,o ,o ,3 ,EE,o ,o ,o ,5 ,5 ,1 ,16,16,16,16,1 , /*32*/ 
/*33*/ 1 ,5 ,1 ,5 ,5 ,5 ,5 ,5 ,1 ,57,5 ,5 ,5 ,5 ,5 ,12,13,13,53,53,53,53,21,21,13,13,6 ,6 ,6 ,6 ,6 ,6 ,31,31,30,30,30,30,30,31,31,6 ,58,o ,o ,o ,1 ,o ,o ,o ,o ,3 ,1 ,5 ,5 ,5 ,5 ,5 ,1 ,16,16,16,16,1 , /*33*/ 
/*34*/ 1 ,58,5 ,5 ,1 ,1 ,5 ,5 ,5 ,57,5 ,5 ,5 ,5 ,5 ,13,13,13,53,53,53,53,53,53,13,13,13,13,53,53,53,53,24,24,24,24,6 ,6 ,6 ,6 ,6 ,6 ,1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,3 ,1 ,1 ,1 ,1 ,16,16,1 ,16,16,16,16,15, /*34*/ 
/*35*/ 1 ,5 ,5 ,1 ,5 ,5 ,5 ,57,5 ,57,5 ,5 ,5 ,5 ,5 ,13,13,13,53,53,53,53,53,53,13,5 ,5 ,13,53,53,53,53,53,53,53,53,6 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,2 ,o ,o ,o ,o ,3 ,2 ,o ,o ,1 ,16,16,1 ,16,16,16,16,21, /*35*/ 
/*36*/ 1 ,5 ,1 ,1 ,5 ,57,1 ,5 ,1 ,1 ,1 ,5 ,5 ,5 ,5 ,12,13,13,53,53,53,53,53,5 ,5 ,5 ,5 ,5 ,5 ,53,53,53,13,13,13,13,13,o ,o ,o ,o ,o ,o ,o ,o ,o ,1 ,o ,o ,o ,o ,3 ,1 ,o ,o ,21,16,16,16,16,21,27,27,21, /*36*/ 
/*37*/ 1 ,19,18,1 ,5 ,1 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,53,13,13,53,53,53,53,13,5 ,5 ,5 ,5 ,5 ,5 ,13,13,13,13,13,4 ,61,1 ,2 ,1 ,1 ,2 ,1 ,1 ,2 ,1 ,61,4 ,o ,o ,o ,o ,3 ,1 ,o ,o ,21,16,16,16,16,21,BB,BB,21, /*37*/ 
/*38*/ 1 ,57,17,1 ,5 ,1 ,5 ,57,57,5 ,58,5 ,5 ,5 ,5 ,53,53,13,13,13,13,13,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,13,13,13,1 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,o ,o ,o ,o ,3 ,4 ,o ,o ,21,21,21,21,21,21,8 ,8 ,21, /*38*/ 
/*39*/ 1 ,16,16,1 ,5 ,5 ,5 ,58,5 ,5 ,58,5 ,5 ,5 ,5 ,12,53,53,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,59,57,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,1 ,o ,o ,1 ,o ,o ,21,7 ,7 ,54,54,4 , /*39*/ 
/*40*/ 1 ,16,1 ,1 ,1 ,1 ,1 ,5 ,5 ,58,62,5 ,5 ,5 ,5 ,53,53,53,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,EE,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,1 ,1 ,1 ,1 ,o ,o ,21,7 ,8 ,8 ,54,21, /*40*/ 
/*41*/ 1 ,16,1 ,16,16,16,16,1 ,5 ,5 ,62,5 ,5 ,5 ,5 ,53,53,53,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,EE,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,2 ,o ,o ,o ,o ,o ,21,7 ,8 ,8 ,54,21, /*41*/ 
/*42*/ 1 ,16,16,16,16,16,1 ,1 ,58,5 ,62,59,5 ,5 ,5 ,12,53,53,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,59,57,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,3 ,1 ,o ,o ,o ,o ,o ,21,7 ,8 ,8 ,54,21, /*42*/ 
/*43*/ 1 ,58,5 ,1 ,5 ,57,5 ,5 ,5 ,5 ,1 ,5 ,5 ,5 ,5 ,53,53,13,13,13,13,13,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,13,13,13,1 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,1 ,o ,o ,21,21,21,21,7 ,8 ,8 ,54,21, /*43*/ 
/*44*/ 1 ,5 ,5 ,1 ,5 ,5 ,5 ,1 ,1 ,5 ,1 ,5 ,5 ,5 ,5 ,53,13,13,13,13,13,13,13,5 ,5 ,5 ,5 ,5 ,5 ,13,13,1 ,1 ,1 ,4 ,1 ,1 ,2 ,1 ,1 ,2 ,1 ,1 ,57,EE,57,1 ,1 ,1 ,EE,1 ,4 ,1 ,o ,o ,1 ,7 ,7 ,7 ,7 ,54,54,54,4 , /*44*/ 
/*45*/ 1 ,5 ,5 ,5 ,57,1 ,1 ,5 ,58,5 ,1 ,5 ,56,5 ,5 ,12,13,13,13,13,13,13,13,13,5 ,5 ,5 ,5 ,13,13,13,1 ,o ,o ,o ,o ,o ,o ,1 ,o ,o ,22,22,22,FF,22,22,22,1 ,o ,1 ,1 ,1 ,o ,o ,1 ,8 ,8 ,8 ,8 ,8 ,8 ,54,1 , /*45*/ 
/*46*/ 1 ,5 ,57,5 ,5 ,5 ,1 ,5 ,5 ,5 ,58,21,21,21,21,21,21,21,21,21,13,13,13,13,13,5 ,5 ,13,13,53,53,1 ,o ,o ,o ,o ,o ,o ,1 ,o ,o ,22,o ,o ,o ,o ,o ,22,1 ,o ,o ,o ,1 ,o ,o ,1 ,8 ,8 ,8 ,8 ,8 ,8 ,54,1 , /*46*/ 
/*47*/ 1 ,58,57,1 ,1 ,5 ,58,58,1 ,1 ,58,21,5 ,5 ,5 ,5 ,56,5 ,5 ,21,13,13,13,13,13,5 ,5 ,13,13,53,53,EE,o ,o ,55,1 ,1 ,1 ,1 ,o ,o ,22,o ,o ,o ,o ,o ,22,1 ,o ,o ,o ,1 ,o ,o ,1 ,8 ,8 ,8 ,8 ,8 ,8 ,54,1 , /*47*/ 
/*48*/ 6 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,20,5 ,5 ,5 ,5 ,5 ,5 ,5 ,20,53,53,53,EE,53,53,53,53,53,53,53,1 ,o ,o ,55,1 ,o ,o ,o ,o ,o ,22,o ,o ,o ,o ,o ,22,4 ,5 ,5 ,5 ,4 ,1 ,1 ,1 ,1 ,1 ,1 ,8 ,8 ,8 ,54,1 , /*48*/ 
/*49*/ 7 ,57,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,20,5 ,5 ,5 ,5 ,5 ,5 ,5 ,20,53,53,53,1 ,53,53,53,53,53,53,53,1 ,1 ,1 ,1 ,1 ,o ,o ,o ,o ,o ,22,o ,o ,o ,o ,o ,22,1 ,8 ,8 ,8 ,54,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,54,1 , /*49*/ 
/*50*/ 7 ,57,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,20,5 ,5 ,5 ,5 ,5 ,5 ,5 ,21,13,13,13,13,13,13,13,13,53,53,53,1 ,1 ,22,22,22,22,22,22,22,22,4 ,o ,o ,o ,o ,o ,22,1 ,8 ,8 ,8 ,54,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,54,1 , /*50*/ 
/*51*/ 6 ,5 ,5 ,57,58,5 ,1 ,1 ,58,58,58,21,59,5 ,5 ,5 ,5 ,5 ,5 ,21,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,28,28,28,1 ,4 ,GG,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,o ,o ,o ,o ,o ,22,1 ,8 ,8 ,8 ,54,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,54,1 , /*51*/ 
/*52*/ 6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,6 ,28,28,28,28,28,28,28,1 ,28,28,28,1 ,1 ,22,3 ,o ,o ,o ,o ,3 ,3 ,3 ,o ,o ,o ,o ,o ,22,1 ,8 ,8 ,8 ,54,54,54,54,54,54,54,54,54,54,54,4 , /*52*/ 
/*53*/ 11,11,11,11,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,6 ,28,28,28,28,28,28,28,1 ,28,28,28,1 ,1 ,22,3 ,o ,o ,o ,o ,3 ,3 ,3 ,o ,o ,o ,o ,o ,22,1 ,8 ,8 ,1 ,4 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*53*/ 
/*54*/ 11,11,11,11,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,6 ,28,28,28,28,28,28,28,1 ,28,28,28,1 ,4 ,HH,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,o ,o ,o ,o ,o ,22,1 ,8 ,8 ,1 ,22,22,22,22,22,22,22,22,22,22,22,22, /*54*/ 
/*55*/ 11,11,10,10,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,6 ,28,28,28,28,28,28,28,1 ,28,28,28,1 ,1 ,22,22,22,22,22,22,22,22,4 ,22,GG,HH,II,22,22,1 ,8 ,8 ,1 ,22,22,22,22,22,63,6 ,6 ,6 ,6 ,6 ,6 , /*55*/ 
/*56*/ 6 ,10,10,10,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,6 ,o ,o ,o ,o ,1 ,26,26,1 ,28,28,28,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,63,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,1 ,22,5 ,5 ,56,5 ,5 ,5 ,56,6 ,6 ,6 ,6 , /*56*/ 
/*57*/ 6 ,10,10,10,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,6 ,o ,o ,o ,o ,26,28,28,28,28,28,28,5 ,5 ,5 ,5 ,5 ,5 ,5 ,63,54,54,8 ,54,54,54,54,54,54,54,54,54,1 ,22,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,6 ,6 ,6 , /*57*/ 
/*58*/ 6 ,6 ,6 ,6 ,6 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,6 ,o ,o ,o ,o ,26,28,28,28,28,28,28,5 ,5 ,5 ,5 ,5 ,5 ,5 ,1 ,28,28,28,28,28,28,28,28,28,28,12,12,12,22,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,4 ,6 ,6 , /*58*/ 
/*59*/ 6 ,6 ,6 ,6 ,6 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,6 ,o ,o ,o ,o ,26,28,28,28,28,28,28,5 ,5 ,5 ,5 ,5 ,5 ,5 ,EE,28,28,28,28,28,28,28,28,28,28,28,FF,FF,FF,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,3 ,31,13, /*59*/ 
/*60*/ 6 ,6 ,6 ,6 ,6 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,6 ,o ,o ,o ,o ,EE,28,28,28,28,28,28,5 ,5 ,5 ,5 ,5 ,5 ,5 ,1 ,28,28,28,28,28,28,28,28,28,28,12,12,12,22,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,4 ,6 ,6 , /*60*/ 
/*61*/ 6 ,6 ,6 ,6 ,6 ,11,11,11,11,9 ,9 ,9 ,9 ,9 ,9 ,9 ,11,11,11,6 ,o ,o ,o ,o ,26,28,28,28,28,28,28,5 ,5 ,5 ,5 ,5 ,5 ,5 ,63,54,54,8 ,54,54,54,54,54,54,54,54,54,1 ,22,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,6 ,6 ,6 , /*61*/ 
/*62*/ 9 ,9 ,9 ,9 ,9 ,11,11,11,11,10,10,10,10,10,10,10,11,11,11,6 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,1 ,1 ,2 ,1 ,1 ,1 ,1 ,1 ,26,26,26,1 ,63,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,1 ,22,5 ,5 ,5 ,5 ,5 ,5 ,59,6 ,6 ,6 ,6 , /*62*/ 
/*63*/ 9 ,9 ,9 ,9 ,9 ,11,11,11,11,10,10,10,10,10,10,10,11,11,11,6 ,28,28,28,28,22,22,22,22,22,22,22,22,22,22,6 ,9 ,9 ,9 ,6 ,22,22,4 ,22,II,GG,HH,22,22,22,22,22,22,22,22,22,22,22,63,6 ,6 ,6 ,6 ,6 ,6   /*63*/ 
    // 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 
    #undef AA
    #undef BB
    #undef CC
    #undef DD
    #undef EE
    #undef FF
    #undef GG
    #undef HH
    #undef II
    #undef o
    },
    {        // tileDictionary
      SFG_TD(14, 7,0,0),SFG_TD(18, 0,0,1),SFG_TD(18, 0,2,1),SFG_TD(14, 6,0,4), // 0 
      SFG_TD(14, 0,4,4),SFG_TD(14,20,4,4),SFG_TD(29, 0,3,4),SFG_TD(27, 2,3,4), // 4 
      SFG_TD(25, 6,4,4),SFG_TD(14,31,0,0),SFG_TD(26,31,1,0),SFG_TD(30,31,0,0), // 8 
      SFG_TD(31, 0,2,2),SFG_TD(14, 0,6,6),SFG_TD(14, 8,0,0),SFG_TD(22, 0,2,3), // 12 
      SFG_TD(18, 8,6,0),SFG_TD(17, 9,6,0),SFG_TD(16,10,6,0),SFG_TD(15,11,6,0), // 16 
      SFG_TD(14, 7,3,3),SFG_TD(29, 0,3,3),SFG_TD(18, 0,5,2),SFG_TD(18, 7,4,0), // 20 
      SFG_TD(14,20,3,4),SFG_TD(13,21,0,4),SFG_TD(16, 2,0,1),SFG_TD(18, 4,6,3), // 24 
      SFG_TD(14,17,0,4),SFG_TD(18, 8,3,3),SFG_TD(13,21,0,0),SFG_TD( 4,30,0,0), // 28 
      SFG_TD(14, 8,0,0),SFG_TD(14, 8,0,0),SFG_TD(14, 8,0,0),SFG_TD(14, 8,0,0), // 32 
      SFG_TD(14, 8,0,0),SFG_TD(14, 8,0,0),SFG_TD(14, 8,0,0),SFG_TD(30, 0,3,6), // 36 
      SFG_TD(14, 8,0,0),SFG_TD(14, 8,0,0),SFG_TD(14, 8,0,0),SFG_TD(14, 8,0,0), // 40 
      SFG_TD(14, 8,0,0),SFG_TD(14, 8,0,0),SFG_TD(14, 8,0,0),SFG_TD(14, 8,0,0), // 44 
      SFG_TD(29,17,5,5),SFG_TD(29,18,5,5),SFG_TD(29,19,5,5),SFG_TD(14, 8,0,0), // 48 
      SFG_TD(14, 8,0,0),SFG_TD(14, 7,0,6),SFG_TD(25, 5,4,4),SFG_TD(16, 5,1,0), // 52 
      SFG_TD(18,16,1,0),SFG_TD(22,12,1,0),SFG_TD(26, 8,0,0),SFG_TD(18,16,0,0), // 56 
      SFG_TD(17,17,1,0),SFG_TD(21,13,0,0),SFG_TD(25, 9,1,0),SFG_TD(14, 0,5,5)  // 60 
      },                    // tileDictionary
    {0 ,4 ,7 ,5 ,11,15,3 }, // textureIndices
    7,                      // doorTextureIndex
    3,                      // floorColor
    18,                     // ceilingColor
    {62, 2 , 240},          // player start: x, y, direction
    1,                      // backgroundImage
    {                       // elements
      {SFG_LEVEL_ELEMENT_BULLETS, {28,1}},{SFG_LEVEL_ELEMENT_BULLETS, {27,1}},
      {SFG_LEVEL_ELEMENT_BULLETS, {56,2}},{SFG_LEVEL_ELEMENT_HEALTH, {21,4}},
      {SFG_LEVEL_ELEMENT_LOCK1, {12,5}},{SFG_LEVEL_ELEMENT_CARD2, {10,5}},
      {SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {5,7}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {45,8}},
      {SFG_LEVEL_ELEMENT_BARREL, {45,9}},{SFG_LEVEL_ELEMENT_RUIN, {40,9}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {20,9}},{SFG_LEVEL_ELEMENT_RUIN, {41,10}},
      {SFG_LEVEL_ELEMENT_RUIN, {39,10}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {20,10}},
      {SFG_LEVEL_ELEMENT_RUIN, {39,11}},{SFG_LEVEL_ELEMENT_HEALTH, {61,12}},
      {SFG_LEVEL_ELEMENT_BULLETS, {58,12}},{SFG_LEVEL_ELEMENT_BULLETS, {57,12}},
      {SFG_LEVEL_ELEMENT_HEALTH, {11,12}},{SFG_LEVEL_ELEMENT_BULLETS, {41,15}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {23,15}},{SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {36,16}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {18,16}},{SFG_LEVEL_ELEMENT_BULLETS, {53,17}},
      {SFG_LEVEL_ELEMENT_BULLETS, {53,18}},{SFG_LEVEL_ELEMENT_TERMINAL, {14,18}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {24,19}},{SFG_LEVEL_ELEMENT_ROCKETS, {7,20}},
      {SFG_LEVEL_ELEMENT_BULLETS, {7,21}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {61,23}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {42,23}},{SFG_LEVEL_ELEMENT_BARREL, {23,23}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {61,26}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {37,26}},
      {SFG_LEVEL_ELEMENT_BULLETS, {16,28}},{SFG_LEVEL_ELEMENT_RUIN, {53,29}},
      {SFG_LEVEL_ELEMENT_BULLETS, {16,29}},{SFG_LEVEL_ELEMENT_HEALTH, {57,31}},
      {SFG_LEVEL_ELEMENT_RUIN, {44,31}},{SFG_LEVEL_ELEMENT_BULLETS, {43,32}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {43,39}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {41,39}},
      {SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {26,40}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {41,41}},
      {SFG_LEVEL_ELEMENT_TREE, {32,45}},{SFG_LEVEL_ELEMENT_BULLETS, {7,45}},
      {SFG_LEVEL_ELEMENT_TERMINAL, {37,46}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {43,47}},
      {SFG_LEVEL_ELEMENT_LOCK1, {23,48}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {44,50}},
      {SFG_LEVEL_ELEMENT_COLUMN, {29,50}},{SFG_LEVEL_ELEMENT_HEALTH, {39,52}},
      {SFG_LEVEL_ELEMENT_TERMINAL, {34,52}},{SFG_LEVEL_ELEMENT_BULLETS, {39,53}},
      {SFG_LEVEL_ELEMENT_TERMINAL, {34,53}},{SFG_LEVEL_ELEMENT_TELEPORT, {46,54}},
      {SFG_LEVEL_ELEMENT_TELEPORT, {35,54}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {9,54}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {25,55}},{SFG_LEVEL_ELEMENT_BARREL, {18,55}},
      {SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {11,55}},{SFG_LEVEL_ELEMENT_TERMINAL, {57,56}},
      {SFG_LEVEL_ELEMENT_COLUMN, {23,56}},{SFG_LEVEL_ELEMENT_COLUMN, {20,56}},
      {SFG_LEVEL_ELEMENT_HEALTH, {4,56}},{SFG_LEVEL_ELEMENT_TREE, {37,57}},
      {SFG_LEVEL_ELEMENT_TREE, {32,57}},{SFG_LEVEL_ELEMENT_BULLETS, {20,58}},
      {SFG_LEVEL_ELEMENT_CARD1, {8,58}},{SFG_LEVEL_ELEMENT_FINISH, {62,59}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {58,59}},{SFG_LEVEL_ELEMENT_LOCK2, {52,59}},
      {SFG_LEVEL_ELEMENT_CARD0, {41,59}},{SFG_LEVEL_ELEMENT_LOCK0, {38,59}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {28,59}},{SFG_LEVEL_ELEMENT_BULLETS, {20,60}},
      {SFG_LEVEL_ELEMENT_TREE, {37,61}},{SFG_LEVEL_ELEMENT_TREE, {32,61}},
      {SFG_LEVEL_ELEMENT_TERMINAL, {57,62}},{SFG_LEVEL_ELEMENT_ROCKETS, {49,62}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}}
    }, // elements
  }; // level

SFG_PROGRAM_MEMORY SFG_Level SFG_level2 =
  {          // level
    {        // mapArray
    #define o 0
    #define AA (20 | SFG_TILE_PROPERTY_DOOR)
    #define BB (32 | SFG_TILE_PROPERTY_DOOR)
    #define CC (22 | SFG_TILE_PROPERTY_ELEVATOR)
    #define DD (40 | SFG_TILE_PROPERTY_SQUEEZER)
    #define EE (44 | SFG_TILE_PROPERTY_DOOR)
    // 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 
/*0 */ 23,23,23,23,23,23,23,23,23,20,4 ,4 ,4 ,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 , /*0 */ 
/*1 */ 23,22,22,22,22,22,22,22,22,16,16,4 ,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,20,16,16,16,16,16,20,4 ,2 ,2 ,2 ,48,3 ,3 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,2 ,o ,o ,48,4 ,4 ,4 ,4 ,4 ,4 ,4 , /*1 */ 
/*2 */ 23,22,22,22,22,22,22,22,22,o ,o ,3 ,o ,o ,18,18,18,18,18,18,18,o ,o ,46,o ,o ,o ,o ,o ,16,20,21,19,19,19,19,4 ,2 ,2 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,2 ,2 ,48,4 ,3 ,o ,5 ,5 ,3 ,4 ,4 ,4 ,4 ,4 ,4 ,4 , /*2 */ 
/*3 */ 23,22,22,22,22,22,22,22,22,17,17,3 ,17,17,18,18,18,18,18,18,18,17,17,47,17,17,17,17,o ,16,AA,19,19,19,19,19,2 ,o ,o ,5 ,5 ,o ,o ,3 ,48,4 ,5 ,5 ,o ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,6 ,7 ,6 ,4 ,4 ,4 , /*3 */ 
/*4 */ 23,22,22,22,22,22,22,22,22,o ,3 ,o ,o ,o ,18,18,18,18,18,18,18,o ,o ,46,o ,o ,o ,17,o ,16,20,19,19,19,19,19,3 ,o ,o ,o ,o ,2 ,4 ,4 ,4 ,4 ,2 ,o ,3 ,3 ,4 ,o ,2 ,4 ,4 ,4 ,4 ,4 ,10,11,11,4 ,4 ,4 , /*4 */ 
/*5 */ 23,22,22,22,22,22,22,22,22,16,4 ,4 ,16,16,16,16,16,16,16,16,16,16,16,16,16,16,o ,17,o ,16,20,16,16,16,16,16,20,20,20,20,20,20,20,20,20,4 ,4 ,4 ,4 ,4 ,4 ,8 ,4 ,4 ,4 ,4 ,9 ,9 ,10,4 ,11,12,4 ,4 , /*5 */ 
/*6 */ 23,22,22,22,22,22,23,23,23,20,4 ,4 ,4 ,20,20,20,20,20,20,20,20,20,20,20,20,16,o ,17,o ,16,20,20,20,AA,20,20,20,19,19,19,19,19,19,19,20,4 ,4 ,4 ,4 ,4 ,9 ,9 ,4 ,4 ,4 ,4 ,9 ,4 ,4 ,4 ,4 ,12,4 ,4 , /*6 */ 
/*7 */ 23,22,22,22,22,22,23,23,23,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,16,o ,17,o ,16,20,20,20,19,19,19,19,19,19,19,19,19,19,19,20,4 ,4 ,4 ,4 ,10,10,9 ,10,11,4 ,8 ,8 ,4 ,4 ,13,13,12,4 ,4 , /*7 */ 
/*8 */ 23,22,22,22,22,22,23,23,23,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,4 ,3 ,18,18,18,16,20,20,20,20,20,20,20,20,20,24,24,29,29,29,20,4 ,4 ,4 ,11,11,4 ,4 ,4 ,12,12,8 ,4 ,4 ,13,13,4 ,4 ,4 ,4 , /*8 */ 
/*9 */ 23,22,22,22,22,22,23,23,23,20,20,20,20,20,20,20,20,20,20,20,20,20,20,20,4 ,4 ,18,18,18,16,20,29,29,29,29,29,29,29,20,25,25,29,29,29,20,4 ,4 ,12,12,4 ,4 ,4 ,4 ,4 ,4 ,8 ,4 ,4 ,13,4 ,4 ,4 ,4 ,4 , /*9 */ 
/*10*/ 30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,20,20,20,4 ,3 ,18,18,18,3 ,20,29,29,29,29,29,29,29,20,26,26,29,20,29,20,4 ,4 ,12,12,4 ,4 ,15,9 ,9 ,8 ,8 ,13,13,13,13,15,4 ,4 ,4 , /*10*/ 
/*11*/ 30,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,40,41,o ,o ,20,16,18,18,18,16,20,29,29,34,34,34,29,29,20,27,27,29,20,29,20,4 ,4 ,13,13,4 ,4 ,15,8 ,8 ,8 ,8 ,13,13,13,13,13,15,4 ,4 , /*11*/ 
/*12*/ 30,40,41,41,41,41,41,40,40,40,40,40,41,41,41,41,41,41,40,40,40,41,o ,o ,20,16,18,18,18,16,20,29,29,34,34,34,29,29,20,28,28,29,20,29,20,4 ,4 ,13,13,13,4 ,13,8 ,8 ,8 ,8 ,13,13,4 ,13,13,13,4 ,4 , /*12*/ 
/*13*/ 30,40,41,40,40,40,41,41,41,41,40,40,41,40,40,40,40,41,40,40,40,41,o ,o ,31,16,18,18,18,16,20,29,29,29,29,29,29,29,20,29,29,29,20,29,20,4 ,4 ,4 ,4 ,13,4 ,8 ,8 ,8 ,8 ,8 ,13,13,4 ,4 ,13,13,6 ,4 , /*13*/ 
/*14*/ 30,40,41,40,40,40,40,40,40,41,40,40,41,40,40,41,41,41,40,40,40,41,o ,o ,32,16,18,18,18,16,20,29,29,29,29,29,29,29,20,29,29,29,20,29,20,4 ,4 ,4 ,4 ,13,13,8 ,8 ,8 ,8 ,8 ,12,13,4 ,6 ,13,13,7 ,4 , /*14*/ 
/*15*/ 30,40,41,40,40,40,40,40,40,41,41,41,41,40,40,41,40,40,40,40,42,41,o ,36,31,16,o ,17,o ,16,20,20,20,29,29,29,20,20,20,32,32,32,32,29,20,20,4 ,4 ,4 ,13,13,8 ,8 ,8 ,8 ,8 ,8 ,13,4 ,7 ,12,12,6 ,4 , /*15*/ 
/*16*/ 30,40,41,40,40,40,40,40,40,40,40,40,40,40,40,41,40,40,40,40,43,41,o ,35,20,16,o ,17,o ,16,20,19,19,28,28,28,19,19,20,29,29,29,29,29,29,20,4 ,4 ,4 ,13,13,8 ,8 ,8 ,8 ,8 ,8 ,13,13,6 ,11,11,4 ,4 , /*16*/ 
/*17*/ 30,40,41,41,41,41,41,41,40,40,40,40,40,40,40,41,40,40,41,41,41,41,o ,16,AA,16,o ,17,o ,16,20,19,19,27,27,27,19,19,20,29,30,31,31,30,29,31,4 ,4 ,4 ,13,13,12,8 ,8 ,8 ,8 ,12,13,13,4 ,11,10,10,4 , /*17*/ 
/*18*/ 30,40,40,40,40,40,40,41,40,40,40,40,40,41,40,41,40,40,41,40,43,41,o ,35,20,16,o ,17,o ,16,20,19,19,26,26,26,19,19,20,29,31,31,31,31,29,30,4 ,4 ,4 ,4 ,13,13,13,13,10,13,13,13,4 ,4 ,4 ,10,9 ,4 , /*18*/ 
/*19*/ 30,40,40,40,40,40,40,41,40,40,41,40,40,41,40,41,40,40,41,40,42,41,o ,36,31,16,o ,17,o ,16,20,19,19,25,25,25,19,19,20,29,31,31,31,31,29,30,4 ,4 ,15,15,15,13,13,13,13,13,13,4 ,4 ,4 ,4 ,9 ,9 ,4 , /*19*/ 
/*20*/ 30,41,41,41,41,40,40,41,41,41,41,40,40,41,40,41,41,41,41,40,40,41,o ,o ,32,16,o ,17,o ,16,20,19,19,24,24,24,19,19,20,29,30,31,31,30,29,31,4 ,4 ,15,4 ,15,4 ,4 ,14,4 ,4 ,4 ,4 ,4 ,4 ,8 ,8 ,8 ,4 , /*20*/ 
/*21*/ 30,40,40,40,40,40,40,40,40,40,41,40,40,41,40,40,40,40,40,40,40,41,o ,o ,31,16,o ,17,o ,16,20,19,19,19,19,19,19,19,20,29,29,29,29,29,29,20,4 ,4 ,15,4 ,11,11,11,12,4 ,4 ,o ,o ,3 ,4 ,8 ,8 ,4 ,4 , /*21*/ 
/*22*/ 30,40,40,40,40,40,40,40,40,40,41,40,40,41,40,40,40,40,40,40,40,41,o ,o ,20,16,o ,17,o ,16,16,19,19,19,19,25,21,19,20,24,25,26,27,28,28,20,4 ,4 ,15,15,4 ,11,4 ,4 ,4 ,o ,5 ,5 ,o ,1 ,o ,4 ,4 ,4 , /*22*/ 
/*23*/ 30,40,41,41,41,41,41,41,40,40,41,40,30,30,30,30,30,30,30,30,30,20,20,20,20,16,o ,17,o ,16,16,19,19,19,19,25,34,19,20,19,19,19,19,19,19,20,4 ,4 ,4 ,15,15,8 ,33,4 ,4 ,48,o ,o ,3 ,4 ,4 ,4 ,4 ,4 , /*23*/ 
/*24*/ 30,40,41,40,40,40,40,41,41,40,41,40,30,31,31,31,31,31,31,31,31,31,31,31,20,16,o ,17,o ,16,16,19,19,19,19,19,19,19,20,19,19,19,19,19,19,20,4 ,4 ,4 ,4 ,4 ,14,14,4 ,4 ,2 ,3 ,6 ,4 ,4 ,4 ,4 ,4 ,4 , /*24*/ 
/*25*/ 30,40,41,40,40,40,40,40,41,41,41,40,30,31,o ,o ,o ,o ,o ,o ,o ,o ,o ,31,31,16,o ,17,o ,16,16,19,19,19,19,19,19,21,20,19,19,19,19,19,19,20,4 ,4 ,4 ,14,14,14,14,3 ,3 ,3 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 , /*25*/ 
/*26*/ 30,40,41,41,41,41,40,40,40,40,40,40,30,31,o ,o ,o ,o ,o ,o ,o ,o ,o ,31,32,16,o ,17,o ,16,16,19,19,19,19,19,19,19,20,19,19,19,19,19,19,20,20,4 ,14,14,14,14,4 ,4 ,2 ,3 ,4 ,2 ,3 ,3 ,2 ,4 ,4 ,4 , /*26*/ 
/*27*/ 30,40,40,40,40,41,41,40,40,40,40,40,30,31,o ,o ,o ,o ,o ,o ,o ,o ,o ,31,31,16,o ,17,o ,16,16,19,19,19,19,19,19,19,20,21,19,19,19,19,19,19,20,4 ,14,14,14,4 ,4 ,3 ,o ,o ,3 ,o ,o ,o ,o ,3 ,4 ,4 , /*27*/ 
/*28*/ 30,40,40,40,40,40,41,40,40,40,40,40,30,31,o ,o ,o ,o ,o ,o ,o ,o ,o ,31,20,16,o ,17,o ,16,16,19,19,19,19,25,34,19,20,19,19,21,19,19,19,19,20,4 ,14,14,14,4 ,3 ,o ,o ,o ,o ,o ,5 ,5 ,5 ,1 ,4 ,4 , /*28*/ 
/*29*/ 30,40,40,40,40,40,41,40,40,40,40,40,30,31,o ,o ,o ,o ,o ,o ,o ,o ,o ,31,20,16,o ,17,o ,16,16,19,19,19,19,25,21,19,20,20,20,20,20,20,16,16,20,4 ,14,14,14,8 ,o ,o ,2 ,o ,o ,o ,o ,o ,5 ,o ,3 ,4 , /*29*/ 
/*30*/ 30,40,30,40,40,41,41,41,40,40,30,40,30,31,o ,o ,o ,47,31,6 ,36,36,36,31,20,16,o ,17,o ,16,20,19,19,19,19,19,19,19,20,29,29,29,20,20,16,16,32,4 ,14,14,8 ,8 ,o ,3 ,3 ,3 ,o ,o ,4 ,3 ,o ,5 ,o ,4 , /*30*/ 
/*31*/ 30,43,30,41,41,41,41,41,41,41,30,43,30,31,o ,o ,o ,o ,46,o ,35,35,35,31,31,16,o ,17,o ,16,20,19,19,24,24,24,19,19,20,29,29,29,20,20,16,16,32,4 ,4 ,4 ,8 ,o ,4 ,4 ,4 ,4 ,o ,4 ,4 ,4 ,o ,5 ,o ,4 , /*31*/ 
/*32*/ 30,42,30,40,40,30,30,30,40,40,30,42,30,31,o ,o ,o ,47,46,o ,16,16,16,31,32,16,o ,17,o ,16,20,19,19,25,25,25,19,19,20,29,29,29,20,20,16,16,20,20,20,20,3 ,o ,4 ,4 ,4 ,4 ,2 ,4 ,4 ,4 ,o ,5 ,o ,4 , /*32*/ 
/*33*/ 30,40,40,40,40,30,30,30,40,40,40,40,30,31,4 ,3 ,3 ,31,31,31,32,BB,32,32,31,16,o ,17,o ,16,20,19,19,26,26,26,19,19,20,29,29,29,20,20,19,19,19,19,19,20,3 ,o ,o ,4 ,4 ,3 ,3 ,4 ,4 ,4 ,3 ,o ,1 ,4 , /*33*/ 
/*34*/ 30,30,30,30,30,30,30,30,30,30,30,30,30,4 ,4 ,o ,o ,4 ,4 ,32,16,16,16,16,20,16,o ,17,o ,16,20,19,19,27,27,27,19,19,20,29,29,29,20,20,19,19,19,19,19,20,6 ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,3 ,2 ,48,4 ,4 , /*34*/ 
/*35*/ 30,30,30,30,30,30,30,30,30,30,30,30,30,4 ,2 ,o ,o ,2 ,4 ,32,16,16,16,16,20,16,o ,17,o ,16,20,19,19,28,28,28,19,19,20,29,29,29,20,20,19,19,19,19,19,20,4 ,o ,5 ,o ,1 ,4 ,4 ,4 ,1 ,3 ,4 ,4 ,4 ,4 , /*35*/ 
/*36*/ 30,30,30,30,30,30,30,30,30,30,30,30,30,4 ,4 ,o ,o ,o ,4 ,32,16,16,16,16,20,16,o ,17,o ,16,16,20,20,29,29,29,20,20,20,29,29,29,20,34,19,25,25,25,19,34,4 ,3 ,o ,5 ,o ,3 ,3 ,1 ,2 ,2 ,4 ,4 ,4 ,4 , /*36*/ 
/*37*/ 30,30,30,30,30,30,30,30,30,30,30,30,30,32,32,4 ,3 ,3 ,4 ,32,16,16,16,16,31,16,o ,17,o ,16,20,29,29,29,29,29,29,29,29,29,29,29,20,34,19,25,25,25,19,34,4 ,4 ,3 ,o ,2 ,3 ,3 ,1 ,o ,o ,3 ,4 ,4 ,4 , /*37*/ 
/*38*/ 30,40,40,40,40,40,40,40,40,40,42,43,41,o ,o ,o ,o ,36,35,32,16,16,16,16,32,16,o ,17,o ,16,20,29,29,29,29,29,29,29,29,29,29,29,20,34,19,25,25,25,19,34,4 ,4 ,4 ,4 ,3 ,3 ,4 ,4 ,o ,5 ,o ,3 ,6 ,6 , /*38*/ 
/*39*/ 30,40,41,40,40,40,40,40,40,40,42,43,41,o ,o ,o ,o ,o ,16,BB,16,16,16,16,31,16,o ,17,o ,16,20,20,20,34,34,34,20,20,20,20,20,20,20,20,19,19,19,19,19,20,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,o ,5 ,5 ,o ,o ,7 , /*39*/ 
/*40*/ 30,40,40,40,40,40,40,40,40,40,40,40,41,o ,o ,o ,o ,36,35,32,16,16,16,16,20,16,o ,17,o ,16,34,20,20,20,20,20,20,34,34,34,20,20,20,20,20,20,20,20,20,20,4 ,4 ,4 ,4 ,4 ,4 ,4 ,2 ,o ,5 ,o ,3 ,6 ,6 , /*40*/ 
/*41*/ 30,40,40,40,40,40,40,40,40,40,40,40,30,32,32,38,38,38,32,32,16,16,16,16,20,16,o ,17,o ,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,20,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,2 ,o ,o ,o ,o ,4 ,4 ,4 , /*41*/ 
/*42*/ 30,40,40,40,40,40,40,40,40,40,40,40,30,o ,o ,o ,o ,o ,o ,32,16,16,34,AA,20,16,o ,17,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,16,34,20,7 ,7 ,7 ,7 ,7 ,4 ,4 ,1 ,5 ,o ,o ,3 ,4 ,4 ,4 , /*42*/ 
/*43*/ 30,40,40,40,40,40,40,40,40,40,40,40,30,o ,o ,o ,o ,o ,o ,32,16,16,34,16,16,16,o ,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,17,o ,16,16,31,7 ,7 ,o ,o ,7 ,7 ,4 ,1 ,5 ,o ,2 ,3 ,4 ,4 ,4 , /*43*/ 
/*44*/ 30,40,40,40,42,43,41,43,42,40,40,40,41,o ,o ,o ,o ,o ,o ,32,32,32,34,34,20,16,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,17,o ,16,34,20,7 ,o ,o ,o ,o ,7 ,4 ,3 ,5 ,5 ,o ,2 ,4 ,4 ,4 , /*44*/ 
/*45*/ 30,30,30,30,30,30,41,30,30,30,30,30,30,o ,30,o ,30,o ,30,32,32,32,20,20,20,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,o ,17,o ,16,20,7 ,7 ,o ,o ,o ,o ,7 ,4 ,4 ,o ,5 ,5 ,o ,4 ,4 ,4 , /*45*/ 
/*46*/ 44,44,44,44,44,34,46,34,44,44,44,44,44,32,32,32,32,32,32,32,32,32,20,20,20,20,20,31,32,31,20,20,31,32,31,20,20,31,16,31,20,20,16,o ,17,o ,16,20,7 ,o ,o ,o ,o ,7 ,7 ,4 ,4 ,3 ,o ,o ,o ,o ,4 ,4 , /*46*/ 
/*47*/ 44,46,46,46,39,o ,o ,o ,39,46,46,46,44,44,44,44,44,44,44,20,20,20,20,30,32,32,16,32,32,30,20,20,20,20,20,20,20,32,32,32,20,20,16,o ,17,o ,16,20,7 ,o ,o ,o ,o ,7 ,4 ,4 ,1 ,3 ,3 ,o ,o ,o ,3 ,4 , /*47*/ 
/*48*/ 44,46,46,46,39,o ,o ,o ,39,46,46,46,44,CC,CC,CC,CC,CC,44,20,20,16,16,16,16,16,16,16,16,16,16,16,20,20,20,20,20,20,20,20,20,20,16,o ,17,o ,16,20,7 ,7 ,o ,o ,7 ,7 ,4 ,1 ,1 ,3 ,3 ,4 ,o ,5 ,o ,4 , /*48*/ 
/*49*/ 44,46,46,46,39,o ,o ,o ,39,46,46,46,44,CC,DD,DD,DD,CC,44,20,31,16,16,16,16,16,16,16,16,16,16,16,31,16,16,16,16,16,16,16,16,20,16,o ,17,o ,16,20,7 ,7 ,7 ,o ,7 ,6 ,4 ,1 ,3 ,3 ,3 ,4 ,o ,5 ,o ,4 , /*49*/ 
/*50*/ 44,46,46,46,39,o ,o ,o ,39,46,46,46,44,CC,DD,DD,DD,CC,44,20,31,16,16,16,16,16,16,16,16,16,16,16,31,16,16,16,16,16,16,16,16,AA,16,o ,17,o ,16,20,4 ,4 ,6 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,4 ,4 ,4 ,o ,3 ,4 , /*50*/ 
/*51*/ 44,44,EE,44,44,o ,o ,o ,44,44,EE,44,44,CC,DD,DD,DD,CC,44,20,20,16,16,16,16,16,16,16,16,16,16,16,20,20,20,20,16,16,20,20,20,20,3 ,3 ,3 ,3 ,4 ,4 ,4 ,4 ,6 ,6 ,6 ,4 ,3 ,3 ,3 ,3 ,4 ,4 ,4 ,4 ,4 ,4 , /*51*/ 
/*52*/ 44,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,44,CC,CC,CC,CC,CC,44,20,20,16,16,16,20,20,20,20,20,16,16,16,20,20,20,20,16,16,20,16,16,AA,16,o ,17,3 ,3 ,4 ,4 ,4 ,4 ,4 ,4 ,3 ,3 ,o ,o ,3 ,1 ,3 ,4 ,4 ,4 ,4 , /*52*/ 
/*53*/ 44,44,44,44,34,o ,o ,o ,34,44,44,44,44,36,36,36,36,36,44,35,35,16,16,16,35,35,37,35,35,16,16,16,35,35,37,20,16,16,20,16,16,20,16,o ,17,o ,16,20,4 ,4 ,4 ,4 ,3 ,2 ,o ,o ,o ,o ,o ,1 ,3 ,4 ,4 ,4 , /*53*/ 
/*54*/ 4 ,4 ,4 ,4 ,44,o ,o ,o ,44,36,46,46,46,46,46,46,46,46,44,36,36,o ,o ,o ,36,36,37,36,36,o ,o ,o ,36,36,37,20,16,16,20,16,16,20,16,o ,17,o ,16,20,4 ,4 ,3 ,48,2 ,o ,o ,o ,5 ,5 ,o ,o ,1 ,4 ,4 ,4 , /*54*/ 
/*55*/ 4 ,4 ,4 ,4 ,44,o ,o ,o ,44,36,46,o ,o ,o ,o ,o ,o ,o ,44,o ,o ,o ,o ,o ,o ,o ,37,46,o ,o ,o ,o ,o ,46,37,20,16,16,34,16,16,20,16,o ,17,o ,16,20,5 ,5 ,o ,o ,o ,o ,5 ,5 ,5 ,5 ,5 ,o ,1 ,1 ,4 ,4 , /*55*/ 
/*56*/ 4 ,4 ,4 ,44,44,o ,o ,o ,44,36,46,45,45,o ,45,45,o ,o ,44,o ,o ,o ,o ,o ,o ,o ,37,46,o ,o ,o ,o ,o ,46,37,20,16,16,16,16,16,20,16,o ,17,o ,3 ,4 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,o ,o ,o ,o ,o ,o ,1 ,4 ,4 , /*56*/ 
/*57*/ 4 ,4 ,4 ,o ,o ,o ,o ,o ,o ,o ,46,45,45,o ,45,45,o ,o ,o ,o ,o ,o ,o ,o ,o ,30,37,46,o ,36,36,36,o ,46,37,20,16,16,16,16,16,20,16,o ,17,o ,o ,3 ,o ,5 ,5 ,5 ,o ,o ,o ,2 ,3 ,48,1 ,o ,5 ,5 ,5 ,6 , /*57*/ 
/*58*/ 4 ,4 ,4 ,o ,o ,o ,o ,o ,o ,o ,46,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,46,37,46,o ,36,36,36,o ,46,37,20,16,16,16,16,16,20,16,o ,17,o ,o ,3 ,2 ,o ,o ,o ,o ,2 ,2 ,4 ,4 ,4 ,4 ,1 ,o ,5 ,5 ,6 , /*58*/ 
/*59*/ 4 ,4 ,4 ,4 ,2 ,o ,o ,o ,o ,o ,46,45,45,o ,45,45,o ,o ,o ,o ,o ,o ,o ,o ,o ,46,37,46,o ,36,36,36,o ,46,37,20,31,31,31,31,31,20,16,o ,17,o ,3 ,4 ,2 ,2 ,o ,o ,2 ,2 ,4 ,4 ,4 ,4 ,4 ,3 ,1 ,o ,o ,4 , /*59*/ 
/*60*/ 4 ,4 ,4 ,4 ,4 ,4 ,4 ,3 ,o ,o ,46,45,45,o ,45,45,o ,o ,o ,o ,30,46,46,46,46,46,37,46,o ,o ,o ,o ,o ,46,37,20,20,20,20,20,20,20,16,o ,17,o ,16,20,2 ,2 ,2 ,2 ,2 ,3 ,4 ,4 ,4 ,4 ,4 ,4 ,3 ,3 ,4 ,4 , /*60*/ 
/*61*/ 4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,3 ,46,o ,o ,o ,o ,o ,o ,o ,37,37,37,37,37,37,37,37,37,30,o ,o ,o ,o ,o ,30,37,20,20,20,20,20,20,20,16,o ,17,22,16,20,2 ,2 ,2 ,3 ,4 ,3 ,3 ,4 ,3 ,3 ,3 ,4 ,4 ,4 ,4 ,4 , /*61*/ 
/*62*/ 4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,2 ,2 ,o ,o ,3 ,3 ,37,4 ,4 ,4 ,4 ,4 ,4 ,4 ,32,32,32,32,CC,32,32,32,32,20,20,20,20,20,20,20,4 ,3 ,17,2 ,3 ,20,3 ,3 ,4 ,4 ,4 ,4 ,3 ,3 ,3 ,4 ,4 ,4 ,4 ,4 ,4 ,4 , /*62*/ 
/*63*/ 4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,32,32,32,32,32,32,32,32,32,20,20,20,20,20,20,20,20,4 ,4 ,4 ,4 ,20,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4   /*63*/ 
    // 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 
    #undef AA
    #undef BB
    #undef CC
    #undef DD
    #undef EE
    #undef o
    },
    {        // tileDictionary
      SFG_TD(14,10,0,0),SFG_TD(16, 8,0,0),SFG_TD(14, 8,0,0),SFG_TD(16, 6,0,0), // 0 
      SFG_TD(14, 0,0,0),SFG_TD(13,12,0,0),SFG_TD(21, 0,1,1),SFG_TD(14, 0,2,2), // 4 
      SFG_TD(14,24,0,0),SFG_TD(16,22,0,0),SFG_TD(18,20,0,0),SFG_TD(20,18,0,0), // 8 
      SFG_TD(22,16,0,0),SFG_TD(24,14,0,0),SFG_TD(24, 4,0,0),SFG_TD(31, 7,0,0), // 12 
      SFG_TD(17, 5,1,1),SFG_TD(15, 9,3,0),SFG_TD(23, 1,3,0),SFG_TD(17,10,1,1), // 16 
      SFG_TD(21, 0,1,4),SFG_TD(22, 5,5,1),SFG_TD(14,31,3,0),SFG_TD(26, 0,1,7), // 20 
      SFG_TD(18, 9,1,1),SFG_TD(19, 8,1,1),SFG_TD(20, 7,1,1),SFG_TD(21, 6,1,1), // 24 
      SFG_TD(22, 5,1,1),SFG_TD(23, 4,1,1),SFG_TD(21, 0,3,3),SFG_TD(21, 0,5,5), // 28 
      SFG_TD(21, 0,6,6),SFG_TD(14, 4,0,0),SFG_TD(21, 0,4,4),SFG_TD(16, 6,1,1), // 32 
      SFG_TD(15, 7,1,1),SFG_TD(21, 0,5,1),SFG_TD(17, 2,6,6),SFG_TD(16, 3,1,1), // 36 
      SFG_TD( 9,15,1,3),SFG_TD(14, 4,3,3),SFG_TD(11,13,3,3),SFG_TD(13,11,3,3), // 40 
      SFG_TD(18, 0,1,1),SFG_TD(15, 9,5,1),SFG_TD(14, 9,0,3),SFG_TD(15, 8,3,3), // 44 
      SFG_TD(16, 6,1,0),SFG_TD(14,15,0,0),SFG_TD(14,15,0,0),SFG_TD(14,15,0,0), // 48 
      SFG_TD(14,15,0,0),SFG_TD(14,15,0,0),SFG_TD(14,15,0,0),SFG_TD(14,15,0,0), // 52 
      SFG_TD(14,15,0,0),SFG_TD(14,15,0,0),SFG_TD(14,15,0,0),SFG_TD(14,15,0,0), // 56 
      SFG_TD(14,15,0,0),SFG_TD(14,15,0,0),SFG_TD(14,15,0,0),SFG_TD(14,15,0,0)  // 60 
      },                    // tileDictionary
    {9 ,3 ,6 ,11,12,2 ,5 }, // textureIndices
    13,                     // doorTextureIndex
    18,                     // floorColor
    26,                     // ceilingColor
    {44, 61, 240},          // player start: x, y, direction
    0,                      // backgroundImage
    {                       // elements
      {SFG_LEVEL_ELEMENT_BULLETS, {55,2}},{SFG_LEVEL_ELEMENT_ROCKETS, {12,2}},
      {SFG_LEVEL_ELEMENT_RUIN, {11,2}},{SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {25,3}},
      {SFG_LEVEL_ELEMENT_RUIN, {11,3}},{SFG_LEVEL_ELEMENT_RUIN, {10,4}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {51,5}},{SFG_LEVEL_ELEMENT_BARREL, {32,5}},
      {SFG_LEVEL_ELEMENT_LAMP, {31,5}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {61,6}},
      {SFG_LEVEL_ELEMENT_HEALTH, {26,7}},{SFG_LEVEL_ELEMENT_BARREL, {55,10}},
      {SFG_LEVEL_ELEMENT_RUIN, {29,10}},{SFG_LEVEL_ELEMENT_BULLETS, {23,11}},
      {SFG_LEVEL_ELEMENT_BULLETS, {22,11}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {13,11}},
      {SFG_LEVEL_ELEMENT_BULLETS, {54,12}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {43,13}},
      {SFG_LEVEL_ELEMENT_LAMP, {50,14}},{SFG_LEVEL_ELEMENT_RUIN, {34,14}},
      {SFG_LEVEL_ELEMENT_RUIN, {33,14}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {50,15}},
      {SFG_LEVEL_ELEMENT_RUIN, {35,15}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {10,15}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {11,16}},{SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {55,18}},
      {SFG_LEVEL_ELEMENT_TELEPORT, {50,20}},{SFG_LEVEL_ELEMENT_LAMP, {31,20}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {5,21}},{SFG_LEVEL_ELEMENT_TELEPORT, {51,23}},
      {SFG_LEVEL_ELEMENT_LAMP, {25,23}},{SFG_LEVEL_ELEMENT_TREE, {36,24}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {32,24}},{SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {34,25}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {25,25}},{SFG_LEVEL_ELEMENT_BULLETS, {18,25}},
      {SFG_LEVEL_ELEMENT_BARREL, {55,26}},{SFG_LEVEL_ELEMENT_BARREL, {54,26}},
      {SFG_LEVEL_ELEMENT_PLASMA, {48,26}},{SFG_LEVEL_ELEMENT_LAMP, {39,26}},
      {SFG_LEVEL_ELEMENT_HEALTH, {37,26}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {32,26}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {17,26}},{SFG_LEVEL_ELEMENT_PLASMA, {48,27}},
      {SFG_LEVEL_ELEMENT_TREE, {36,27}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {20,27}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {19,27}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {9,27}},
      {SFG_LEVEL_ELEMENT_ROCKETS, {48,28}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {17,28}},
      {SFG_LEVEL_ELEMENT_TERMINAL, {18,29}},{SFG_LEVEL_ELEMENT_LAMP, {31,31}},
      {SFG_LEVEL_ELEMENT_HEALTH, {50,32}},{SFG_LEVEL_ELEMENT_BARREL, {16,32}},
      {SFG_LEVEL_ELEMENT_BARREL, {15,32}},{SFG_LEVEL_ELEMENT_HEALTH, {55,33}},
      {SFG_LEVEL_ELEMENT_BARREL, {25,33}},{SFG_LEVEL_ELEMENT_CARD1, {3,33}},
      {SFG_LEVEL_ELEMENT_MONSTER_EXPLODER, {56,36}},{SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {44,36}},
      {SFG_LEVEL_ELEMENT_LAMP, {30,36}},{SFG_LEVEL_ELEMENT_CARD0, {46,37}},
      {SFG_LEVEL_ELEMENT_RUIN, {36,37}},{SFG_LEVEL_ELEMENT_RUIN, {35,38}},
      {SFG_LEVEL_ELEMENT_HEALTH, {31,38}},{SFG_LEVEL_ELEMENT_RUIN, {18,39}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {2,39}},{SFG_LEVEL_ELEMENT_RUIN, {18,40}},
      {SFG_LEVEL_ELEMENT_BARREL, {33,41}},{SFG_LEVEL_ELEMENT_MONSTER_EXPLODER, {23,41}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {37,42}},{SFG_LEVEL_ELEMENT_LOCK1, {23,42}},
      {SFG_LEVEL_ELEMENT_BLOCKER, {9,42}},{SFG_LEVEL_ELEMENT_BLOCKER, {8,42}},
      {SFG_LEVEL_ELEMENT_BLOCKER, {7,42}},{SFG_LEVEL_ELEMENT_BLOCKER, {6,42}},
      {SFG_LEVEL_ELEMENT_BLOCKER, {5,42}},{SFG_LEVEL_ELEMENT_BLOCKER, {4,42}},
      {SFG_LEVEL_ELEMENT_BLOCKER, {3,42}},{SFG_LEVEL_ELEMENT_TERMINAL, {47,43}},
      {SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {35,43}},{SFG_LEVEL_ELEMENT_BULLETS, {21,43}},
      {SFG_LEVEL_ELEMENT_HEALTH, {20,43}},{SFG_LEVEL_ELEMENT_ROCKETS, {16,43}},
      {SFG_LEVEL_ELEMENT_ROCKETS, {52,44}},{SFG_LEVEL_ELEMENT_MONSTER_EXPLODER, {51,44}},
      {SFG_LEVEL_ELEMENT_MONSTER_EXPLODER, {37,44}},{SFG_LEVEL_ELEMENT_LAMP, {38,46}},
      {SFG_LEVEL_ELEMENT_LAMP, {26,47}},{SFG_LEVEL_ELEMENT_HEALTH, {61,49}},
      {SFG_LEVEL_ELEMENT_MONSTER_EXPLODER, {57,49}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {10,49}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {2,49}},{SFG_LEVEL_ELEMENT_BARREL, {1,49}},
      {SFG_LEVEL_ELEMENT_RUIN, {44,50}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {35,50}},
      {SFG_LEVEL_ELEMENT_HEALTH, {33,50}},{SFG_LEVEL_ELEMENT_PLASMA, {15,50}},
      {SFG_LEVEL_ELEMENT_RUIN, {45,51}},{SFG_LEVEL_ELEMENT_RUIN, {43,51}},
      {SFG_LEVEL_ELEMENT_RUIN, {42,51}},{SFG_LEVEL_ELEMENT_LOCK0, {41,52}},
      {SFG_LEVEL_ELEMENT_RUIN, {9,52}},{SFG_LEVEL_ELEMENT_LAMP, {58,55}},
      {SFG_LEVEL_ELEMENT_BULLETS, {53,56}},{SFG_LEVEL_ELEMENT_BULLETS, {52,56}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {30,56}},{SFG_LEVEL_ELEMENT_MONSTER_EXPLODER, {22,57}},
      {SFG_LEVEL_ELEMENT_RUIN, {46,60}},{SFG_LEVEL_ELEMENT_FINISH, {30,62}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}}
    }, // elements
  }; // level
  
SFG_PROGRAM_MEMORY SFG_Level SFG_level3 =
  {          // level 3
    {        // mapArray
    #define o 0
    #define AA (5 | SFG_TILE_PROPERTY_SQUEEZER)
    #define BB (19 | SFG_TILE_PROPERTY_DOOR)
    #define CC (24 | SFG_TILE_PROPERTY_DOOR)
    #define DD (56 | SFG_TILE_PROPERTY_ELEVATOR)
    #define EE (7 | SFG_TILE_PROPERTY_ELEVATOR)
    #define FF (8 | SFG_TILE_PROPERTY_DOOR)
    #define GG (63 | SFG_TILE_PROPERTY_DOOR)
    #define HH (47 | SFG_TILE_PROPERTY_SQUEEZER)
    #define II (4 | SFG_TILE_PROPERTY_SQUEEZER)
    #define JJ (51 | SFG_TILE_PROPERTY_ELEVATOR)
    #define KK (17 | SFG_TILE_PROPERTY_ELEVATOR)
    #define LL (50 | SFG_TILE_PROPERTY_ELEVATOR)
    #define MM (1 | SFG_TILE_PROPERTY_DOOR)
    // 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 
/*0 */ 8 ,8 ,8 ,8 ,8 ,24,24,24,8 ,8 ,24,24,24,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,21,8 ,8 ,8 ,8 ,8 ,8 ,8 ,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,24,8 , /*0 */ 
/*1 */ 8 ,5 ,5 ,5 ,5 ,24,5 ,24,9 ,9 ,24,5 ,24,5 ,5 ,5 ,5 ,5 ,5 ,27,27,27,59,59,21,59,59,27,27,27,27,27,24,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,8 , /*1 */ 
/*2 */ 8 ,5 ,24,24,24,9 ,5 ,9 ,9 ,9 ,9 ,5 ,9 ,9 ,5 ,5 ,5 ,5 ,5 ,27,27,27,59,59,21,59,59,27,27,27,27,27,24,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,8 , /*2 */ 
/*3 */ 8 ,5 ,24,5 ,5 ,AA,AA,AA,AA,9 ,AA,AA,AA,9 ,5 ,5 ,5 ,5 ,5 ,27,27,27,59,5 ,5 ,5 ,59,27,27,27,27,27,24,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,32,32,32,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,8 , /*3 */ 
/*4 */ 8 ,5 ,24,5 ,9 ,AA,AA,9 ,AA,AA,AA,AA,AA,9 ,5 ,5 ,5 ,5 ,57,9 ,59,59,59,5 ,5 ,5 ,59,59,59,9 ,9 ,27,24,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,32,32,32,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,8 , /*4 */ 
/*5 */ 63,57,24,5 ,9 ,AA,AA,9 ,AA,AA,AA,AA,9 ,9 ,5 ,5 ,5 ,5 ,57,59,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,59,57,5 ,6 ,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,32,32,32,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,8 , /*5 */ 
/*6 */ 8 ,57,9 ,AA,AA,AA,AA,9 ,9 ,9 ,9 ,9 ,5 ,5 ,5 ,5 ,5 ,5 ,57,59,5 ,59,59,59,59,59,59,59,5 ,59,57,5 ,6 ,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,32,32,32,32,32,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,8 , /*6 */ 
/*7 */ 8 ,57,9 ,AA,AA,AA,9 ,24,5 ,AA,AA,9 ,5 ,5 ,5 ,5 ,5 ,5 ,57,59,5 ,59,9 ,27,27,27,9 ,59,5 ,59,57,5 ,23,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,32,32,32,32,32,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,8 , /*7 */ 
/*8 */ 53,57,9 ,AA,AA,AA,9 ,24,5 ,AA,AA,21,5 ,5 ,5 ,5 ,5 ,5 ,57,59,5 ,59,27,5 ,5 ,5 ,27,59,5 ,59,57,5 ,23,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,32,32,32,32,32,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,8 , /*8 */ 
/*9 */ 8 ,57,24,5 ,9 ,AA,9 ,24,5 ,AA,AA,21,5 ,5 ,5 ,5 ,5 ,5 ,57,59,5 ,59,27,5 ,5 ,5 ,27,59,5 ,59,57,5 ,23,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,32,32,32,32,32,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,8 , /*9 */ 
/*10*/ 8 ,57,24,5 ,9 ,AA,9 ,24,5 ,AA,AA,9 ,5 ,5 ,5 ,5 ,5 ,5 ,57,59,5 ,19,27,5 ,5 ,5 ,27,19,5 ,59,57,5 ,6 ,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,32,32,32,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,8 , /*10*/ 
/*11*/ 63,57,9 ,9 ,9 ,AA,9 ,9 ,9 ,AA,AA,9 ,5 ,5 ,5 ,5 ,5 ,5 ,57,59,5 ,BB,27,5 ,5 ,5 ,27,BB,5 ,59,57,5 ,6 ,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,32,32,32,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,8 , /*11*/ 
/*12*/ 8 ,57,9 ,AA,AA,AA,AA,AA,9 ,AA,AA,9 ,9 ,9 ,9 ,27,27,27,9 ,58,59,58,9 ,27,27,27,9 ,58,59,58,9 ,27,24,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,32,32,32,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,8 , /*12*/ 
/*13*/ 8 ,57,9 ,AA,AA,AA,AA,AA,9 ,AA,AA,AA,AA,9 ,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,24,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,8 , /*13*/ 
/*14*/ 53,57,9 ,9 ,AA,AA,9 ,9 ,9 ,9 ,9 ,AA,AA,9 ,27,27,27,27,27,27,27,27,9 ,27,27,27,9 ,27,27,27,27,27,24,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,8 , /*14*/ 
/*15*/ 8 ,57,24,5 ,AA,AA,5 ,24,5 ,AA,AA,AA,AA,9 ,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,27,24,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,8 , /*15*/ 
/*16*/ 8 ,57,24,5 ,AA,AA,5 ,24,5 ,AA,AA,AA,9 ,9 ,27,27,27,27,27,27,27,27,9 ,27,9 ,27,9 ,27,27,27,27,27,24,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,8 , /*16*/ 
/*17*/ 63,57,9 ,9 ,AA,AA,9 ,9 ,9 ,AA,AA,AA,9 ,57,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,23,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,8 , /*17*/ 
/*18*/ 8 ,5 ,9 ,AA,AA,AA,AA,AA,AA,AA,AA,AA,9 ,57,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,23,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,6 ,6 ,24,8 , /*18*/ 
/*19*/ 8 ,5 ,9 ,9 ,9 ,9 ,AA,AA,AA,AA,AA,AA,9 ,57,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,23,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,8 , /*19*/ 
/*20*/ 53,5 ,9 ,9 ,9 ,9 ,9 ,AA,AA,AA,AA,AA,9 ,57,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,24,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,25,24,8 , /*20*/ 
/*21*/ 53,5 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,57,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,6 ,6 ,6 ,6 ,6 ,6 ,24,24,24,24,24,24,24,24,24,24,24,24,CC,24,24,24,24,24,24,24,24,24,24,42,24,8 , /*21*/ 
/*22*/ 8 ,5 ,19,59,59,59,63,63,63,59,59,59,19,57,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,26,6 ,6 ,6 ,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,6 ,24,24,42,24,8 , /*22*/ 
/*23*/ 8 ,5 ,BB,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,59,57,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,24,48,48,48,48,48,24,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,26,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,42,42,24,8 , /*23*/ 
/*24*/ 8 ,59,19,59,59,59,19,21,19,59,59,59,19,57,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,27,5 ,5 ,5 ,5 ,5 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,26,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,24,24,24,8 , /*24*/ 
/*25*/ 53,27,27,27,27,27,27,27,27,27,27,27,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,24,48,48,48,48,5 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,26,6 ,24,24,24,6 ,6 ,6 ,24,24,24,6 ,6 ,6 ,24,24,24,6 ,26,6 ,6 ,8 ,8 , /*25*/ 
/*26*/ 8 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,27,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,23,5 ,5 ,5 ,48,5 ,6 ,6 ,6 ,6 ,6 ,6 ,24,26,26,24,6 ,24,24,24,6 ,6 ,6 ,24,6 ,24,6 ,6 ,6 ,24,24,24,6 ,26,6 ,6 ,DD,8 , /*26*/ 
/*27*/ 8 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,27,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,23,5 ,48,5 ,5 ,5 ,23,6 ,6 ,6 ,6 ,6 ,26,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,26,6 ,6 ,8 ,8 , /*27*/ 
/*28*/ 8 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,27,19,57,57,57,57,57,57,57,57,57,5 ,5 ,5 ,5 ,23,5 ,48,48,48,48,23,6 ,6 ,6 ,6 ,6 ,26,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,24,6 ,6 ,DD,8 , /*28*/ 
/*29*/ 8 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,27,19,59,59,59,58,58,59,59,59,19,5 ,5 ,5 ,5 ,23,5 ,5 ,5 ,5 ,5 ,23,6 ,6 ,6 ,6 ,6 ,26,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,8 ,8 , /*29*/ 
/*30*/ 8 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,27,BB,27,27,27,27,27,27,27,27,BB,5 ,5 ,5 ,5 ,24,48,48,48,63,11,22,63,20,CC,20,20,20,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,6 ,8 ,8 , /*30*/ 
/*31*/ 53,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,27,19,9 ,9 ,9 ,9 ,9 ,9 ,9 ,9 ,19,5 ,5 ,5 ,5 ,5 ,5 ,5 ,5 ,22,EE,EE,12,6 ,6 ,20,20,20,6 ,6 ,6 ,23,6 ,6 ,6 ,26,6 ,6 ,6 ,23,6 ,6 ,6 ,23,6 ,6 ,6 ,23,6 ,6 ,6 ,8 ,8 , /*31*/ 
/*32*/ 8 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,15,14,3 ,3 ,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,10,EE,EE,22,47,47,18,4 ,4 ,18,47,47,47,47,47,47,63,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,33,8 , /*32*/ 
/*33*/ 8 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,15,14,3 ,3 ,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,63,22,FF,63,47,47,18,4 ,4 ,18,47,47,47,47,47,47,63,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,33,8 , /*33*/ 
/*34*/ 63,o ,o ,o ,o ,63,63,o ,o ,o ,o ,o ,o ,3 ,3 ,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,46,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,55,8 , /*34*/ 
/*35*/ 8 ,o ,o ,o ,63,63,63,63,o ,o ,o ,o ,o ,3 ,3 ,3 ,61,61,63,24,63,61,61,o ,o ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,46,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,18,13,18,4 ,55,DD, /*35*/ 
/*36*/ 8 ,o ,o ,o ,63,63,63,63,o ,o ,o ,o ,o ,3 ,3 ,3 ,61,63,62,62,62,63,61,o ,o ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,46,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,13,13,13,4 ,55,DD, /*36*/ 
/*37*/ 63,o ,o ,o ,o ,63,63,o ,o ,o ,o ,o ,o ,62,62,62,63,62,62,62,62,62,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,46,46,46,46,46,46,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,13,13,13,4 ,55,8 , /*37*/ 
/*38*/ 8 ,o ,54,54,54,o ,o ,54,54,54,o ,o ,o ,62,62,62,GG,62,62,62,62,62,24,o ,o ,o ,o ,o ,o ,o ,8 ,53,4 ,4 ,4 ,4 ,4 ,4 ,4 ,46,46,46,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,18,13,13,13,18,18,4 ,53,8 , /*38*/ 
/*39*/ 8 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,15,15,62,62,62,63,62,62,62,62,62,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,4 ,46,46,16,16,16,16,16,16,16,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,13,13,13,13,18,4 ,4 ,55,8 , /*39*/ 
/*40*/ 8 ,3 ,JJ,JJ,JJ,3 ,3 ,JJ,JJ,JJ,3 ,14,14,3 ,3 ,3 ,61,63,62,62,62,63,61,o ,o ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,4 ,46,4 ,16,HH,16,II,16,HH,16,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,13,13,13,4 ,4 ,4 ,4 ,55,8 , /*40*/ 
/*41*/ 63,3 ,JJ,JJ,JJ,3 ,3 ,JJ,JJ,JJ,3 ,3 ,3 ,3 ,3 ,3 ,61,61,63,24,63,61,61,o ,o ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,46,46,4 ,4 ,47,4 ,47,4 ,47,4 ,4 ,8 ,18,4 ,4 ,4 ,4 ,4 ,13,13,13,4 ,4 ,4 ,4 ,55,8 , /*41*/ 
/*42*/ 8 ,3 ,JJ,JJ,JJ,3 ,3 ,JJ,JJ,JJ,3 ,14,14,3 ,3 ,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,61,61,o ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,46,4 ,4 ,4 ,47,4 ,47,4 ,47,4 ,4 ,13,45,4 ,4 ,4 ,4 ,4 ,18,13,18,4 ,4 ,4 ,4 ,55,8 , /*42*/ 
/*43*/ 8 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,15,15,3 ,3 ,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,61,61,o ,o ,8 ,53,4 ,4 ,4 ,4 ,4 ,46,4 ,4 ,4 ,45,4 ,45,4 ,45,4 ,4 ,13,45,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,53,8 , /*43*/ 
/*44*/ 8 ,o ,54,54,54,o ,o ,54,54,54,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,61,61,61,o ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,46,4 ,4 ,4 ,47,47,47,47,47,4 ,4 ,18,45,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,8 ,8 , /*44*/ 
/*45*/ 8 ,o ,49,49,49,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,61,61,61,61,61,61,61,24,24,o ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,19,4 ,4 ,4 ,46,46,46,46,47,4 ,4 ,18,45,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,8 ,8 , /*45*/ 
/*46*/ 63,o ,49,49,49,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,61,61,61,61,61,61,61,24,24,o ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,46,46,46,46,47,4 ,4 ,13,45,4 ,4 ,46,46,4 ,4 ,4 ,4 ,4 ,46,46,4 ,8 ,8 , /*46*/ 
/*47*/ 8 ,o ,49,49,49,o ,o ,o ,15,15,15,o ,o ,15,15,15,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,46,46,46,46,47,4 ,4 ,13,45,4 ,4 ,46,46,4 ,4 ,4 ,4 ,4 ,46,46,4 ,8 ,8 , /*47*/ 
/*48*/ 8 ,o ,49,49,49,o ,o ,o ,14,14,14,o ,o ,14,14,14,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,8 ,53,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,47,47,47,47,47,4 ,4 ,18,18,4 ,4 ,4 ,4 ,4 ,18,13,18,4 ,4 ,4 ,4 ,8 ,8 , /*48*/ 
/*49*/ 8 ,o ,49,49,49,o ,o ,o ,3 ,3 ,3 ,o ,o ,3 ,3 ,3 ,o ,o ,54,o ,o ,o ,54,o ,o ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,18,18,4 ,4 ,4 ,45,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,16,13,16,4 ,4 ,4 ,4 ,8 ,8 , /*49*/ 
/*50*/ 8 ,o ,49,49,49,o ,o ,o ,3 ,o ,o ,o ,o ,o ,o ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,18,18,4 ,4 ,4 ,45,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,16,KK,16,4 ,4 ,4 ,4 ,8 ,8 , /*50*/ 
/*51*/ 8 ,o ,24,LL,24,o ,o ,54,3 ,54,o ,o ,o ,o ,54,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,18,18,4 ,4 ,4 ,47,47,47,47,47,4 ,4 ,18,18,4 ,4 ,4 ,4 ,4 ,16,16,16,4 ,4 ,4 ,4 ,DD,8 , /*51*/ 
/*52*/ 63,o ,2 ,2 ,2 ,o ,o ,o ,3 ,o ,o ,o ,o ,o ,o ,3 ,3 ,49,61,61,49,3 ,3 ,3 ,3 ,54,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,18,4 ,4 ,4 ,4 ,4 ,4 ,4 ,47,4 ,4 ,13,45,4 ,4 ,4 ,4 ,4 ,18,13,18,4 ,4 ,4 ,4 ,8 ,8 , /*52*/ 
/*53*/ 8 ,o ,2 ,2 ,2 ,o ,o ,o ,3 ,o ,o ,o ,o ,o ,o ,o ,o ,60,61,61,61,3 ,3 ,3 ,3 ,o ,o ,o ,o ,o ,8 ,53,4 ,4 ,4 ,4 ,4 ,18,4 ,4 ,4 ,4 ,4 ,4 ,4 ,47,4 ,4 ,13,45,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,8 ,8 , /*53*/ 
/*54*/ 8 ,15,2 ,2 ,2 ,o ,o ,54,3 ,54,o ,o ,o ,o ,o ,o ,o ,60,61,61,61,3 ,3 ,3 ,3 ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,18,18,4 ,4 ,4 ,47,47,47,4 ,47,4 ,4 ,18,45,4 ,19,46,46,46,46,46,46,46,46,46,46,19,8 , /*54*/ 
/*55*/ 8 ,14,2 ,2 ,2 ,o ,o ,o ,3 ,o ,o ,o ,o ,o ,o ,3 ,3 ,49,61,61,49,3 ,3 ,3 ,3 ,54,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,18,18,4 ,4 ,4 ,45,4 ,45,4 ,47,4 ,4 ,18,45,4 ,46,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,8 ,8 , /*55*/ 
/*56*/ 8 ,3 ,2 ,2 ,2 ,2 ,8 ,24,3 ,61,61,o ,o ,o ,54,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,18,18,4 ,4 ,4 ,47,4 ,47,47,47,4 ,4 ,13,45,4 ,46,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,8 ,8 , /*56*/ 
/*57*/ 8 ,3 ,2 ,2 ,2 ,2 ,2 ,8 ,3 ,61,61,o ,o ,o ,54,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,47,4 ,4 ,4 ,4 ,4 ,4 ,13,45,4 ,46,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,8 ,8 , /*57*/ 
/*58*/ 63,14,2 ,2 ,2 ,2 ,2 ,2 ,MM,61,61,o ,o ,o ,o ,3 ,o ,o ,54,o ,o ,o ,54,o ,o ,o ,63,63,o ,o ,8 ,53,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,18,HH,18,4 ,4 ,4 ,4 ,4 ,8 ,18,4 ,46,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,8 ,8 , /*58*/ 
/*59*/ 8 ,15,2 ,2 ,2 ,2 ,2 ,8 ,3 ,61,61,o ,o ,o ,o ,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,63,63,63,63,o ,o ,o ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,16,16,16,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,46,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,8 ,8 , /*59*/ 
/*60*/ 8 ,o ,2 ,2 ,2 ,2 ,8 ,24,3 ,61,61,54,o ,54,o ,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,63,63,63,63,o ,o ,o ,4 ,4 ,4 ,4 ,19,46,46,46,19,46,19,46,46,46,46,46,46,46,46,19,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,53,8 , /*60*/ 
/*61*/ 8 ,o ,o ,o ,o ,o ,o ,o ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,3 ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,63,63,o ,o ,o ,o ,4 ,4 ,4 ,4 ,46,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,34,8 , /*61*/ 
/*62*/ 8 ,o ,o ,o ,o ,o ,o ,o ,o ,54,o ,54,o ,54,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,4 ,4 ,4 ,4 ,46,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,4 ,34,8 , /*62*/ 
/*63*/ 8 ,8 ,8 ,DD,8 ,DD,8 ,8 ,8 ,8 ,53,53,53,8 ,8 ,8 ,53,8 ,34,34,34,8 ,53,8 ,34,34,34,8 ,53,8 ,19,19,33,33,33,53,19,8 ,8 ,8 ,8 ,8 ,53,34,34,53,8 ,8 ,8 ,8 ,53,34,34,53,8 ,8 ,8 ,8 ,8 ,53,34,34,34,8   /*63*/ 
    // 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 
    #undef AA
    #undef BB
    #undef CC
    #undef DD
    #undef EE
    #undef FF
    #undef GG
    #undef HH
    #undef II
    #undef JJ
    #undef KK
    #undef LL
    #undef MM
    #undef o
    },
    {        // tileDictionary
      SFG_TD(10,20,0,0),SFG_TD( 4,26,0,0),SFG_TD( 0, 4,0,0),SFG_TD( 4,26,0,0), // 0 
      SFG_TD( 0,30,2,2),SFG_TD(17,13,0,2),SFG_TD(24, 6,0,1),SFG_TD( 0,24,1,0), // 4 
      SFG_TD( 4, 0,1,0),SFG_TD(19, 0,4,1),SFG_TD(10, 5,0,0),SFG_TD(17, 5,0,0), // 8 
      SFG_TD(24, 5,0,0),SFG_TD( 8,22,3,0),SFG_TD( 6,24,0,0),SFG_TD( 8,22,0,0), // 12 
      SFG_TD(11,19,1,0),SFG_TD( 0,10,0,0),SFG_TD( 8,22,0,0),SFG_TD(21, 0,4,4), // 16 
      SFG_TD(24, 0,0,5),SFG_TD(17, 5,0,1),SFG_TD( 0, 0,0,0),SFG_TD(24, 6,5,5), // 20 
      SFG_TD(28, 0,5,1),SFG_TD(24, 8,0,1),SFG_TD(26, 3,5,1),SFG_TD(17,10,0,5), // 24 
      SFG_TD(13,15,0,0),SFG_TD(13,15,0,0),SFG_TD(13,15,0,0),SFG_TD(13,15,0,0), // 28 
      SFG_TD(24, 8,0,1),SFG_TD( 8, 0,6,0),SFG_TD(18, 0,0,6),SFG_TD(13,15,0,0), // 32 
      SFG_TD(13,15,0,0),SFG_TD(13,15,0,0),SFG_TD(13,15,0,0),SFG_TD(13,15,0,0), // 36 
      SFG_TD(13,15,0,0),SFG_TD(13,15,0,0),SFG_TD(28, 4,5,1),SFG_TD(13,15,0,0), // 40 
      SFG_TD(13,15,0,0),SFG_TD( 2,28,4,2),SFG_TD( 0,13,0,4),SFG_TD( 2,28,2,2), // 44 
      SFG_TD(22, 8,5,2),SFG_TD(15,15,0,0),SFG_TD( 0,15,0,0),SFG_TD(15,11,5,0), // 48 
      SFG_TD(13,15,0,0),SFG_TD( 0, 0,1,1),SFG_TD(10,20,1,0),SFG_TD(22, 8,0,0), // 52 
      SFG_TD(31,24,4,4),SFG_TD(19,11,6,2),SFG_TD( 0, 0,3,3),SFG_TD(19, 3,4,3), // 56 
      SFG_TD(12,18,5,0),SFG_TD(14,16,5,0),SFG_TD(17,13,1,1),SFG_TD(21, 0,2,2)  // 60 
      },                    // tileDictionary
    {14,3 ,11,15,2 ,12,10}, // textureIndices
    13,                     // doorTextureIndex
    50,                     // floorColor
    186,                     // ceilingColor
    {56, 50, 240},          // player start: x, y, direction
    1,                      // backgroundImage
    {                       // elements
      {SFG_LEVEL_ELEMENT_FINISH, {24,0}},{SFG_LEVEL_ELEMENT_BULLETS, {29,1}},
      {SFG_LEVEL_ELEMENT_BULLETS, {28,1}},{SFG_LEVEL_ELEMENT_CARD0, {11,1}},
      {SFG_LEVEL_ELEMENT_PLASMA, {4,1}},{SFG_LEVEL_ELEMENT_ROCKETS, {3,1}},
      {SFG_LEVEL_ELEMENT_TELEPORT, {35,2}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {60,3}},
      {SFG_LEVEL_ELEMENT_CARD2, {40,6}},{SFG_LEVEL_ELEMENT_MONSTER_PLASMABOT, {51,7}},
      {SFG_LEVEL_ELEMENT_LAMP, {12,7}},{SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {36,9}},
      {SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {47,10}},{SFG_LEVEL_ELEMENT_LOCK2, {27,10}},
      {SFG_LEVEL_ELEMENT_LOCK2, {21,10}},{SFG_LEVEL_ELEMENT_LAMP, {12,10}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {1,10}},{SFG_LEVEL_ELEMENT_MONSTER_PLASMABOT, {42,11}},
      {SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {34,11}},{SFG_LEVEL_ELEMENT_BARREL, {5,11}},
      {SFG_LEVEL_ELEMENT_COLUMN, {37,12}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {35,12}},
      {SFG_LEVEL_ELEMENT_COLUMN, {33,12}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {55,13}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {1,13}},{SFG_LEVEL_ELEMENT_HEALTH, {3,15}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {50,17}},{SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {53,20}},
      {SFG_LEVEL_ELEMENT_BARREL, {18,20}},{SFG_LEVEL_ELEMENT_LOCK2, {50,21}},
      {SFG_LEVEL_ELEMENT_BARREL, {19,21}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {18,21}},
      {SFG_LEVEL_ELEMENT_BARREL, {17,21}},{SFG_LEVEL_ELEMENT_BARREL, {18,22}},
      {SFG_LEVEL_ELEMENT_TELEPORT, {60,23}},{SFG_LEVEL_ELEMENT_HEALTH, {39,23}},
      {SFG_LEVEL_ELEMENT_ROCKETS, {39,24}},{SFG_LEVEL_ELEMENT_ROCKETS, {61,25}},
      {SFG_LEVEL_ELEMENT_HEALTH, {60,25}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {50,26}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {29,27}},{SFG_LEVEL_ELEMENT_HEALTH, {5,28}},
      {SFG_LEVEL_ELEMENT_LAMP, {22,29}},{SFG_LEVEL_ELEMENT_LOCK1, {35,30}},
      {SFG_LEVEL_ELEMENT_COLUMN, {56,31}},{SFG_LEVEL_ELEMENT_COLUMN, {50,31}},
      {SFG_LEVEL_ELEMENT_ROCKETS, {54,32}},{SFG_LEVEL_ELEMENT_BULLETS, {53,32}},
      {SFG_LEVEL_ELEMENT_BULLETS, {38,32}},{SFG_LEVEL_ELEMENT_PLASMA, {37,32}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {32,32}},{SFG_LEVEL_ELEMENT_LAMP, {33,34}},
      {SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {33,35}},{SFG_LEVEL_ELEMENT_PLASMA, {9,35}},
      {SFG_LEVEL_ELEMENT_HEALTH, {23,36}},{SFG_LEVEL_ELEMENT_TERMINAL, {19,36}},
      {SFG_LEVEL_ELEMENT_PLASMA, {9,36}},{SFG_LEVEL_ELEMENT_CARD1, {21,38}},
      {SFG_LEVEL_ELEMENT_MONSTER_PLASMABOT, {19,38}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {57,39}},
      {SFG_LEVEL_ELEMENT_PLASMA, {56,40}},{SFG_LEVEL_ELEMENT_ROCKETS, {56,41}},
      {SFG_LEVEL_ELEMENT_BARREL, {37,48}},{SFG_LEVEL_ELEMENT_BARREL, {36,48}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {44,49}},{SFG_LEVEL_ELEMENT_HEALTH, {36,52}},
      {SFG_LEVEL_ELEMENT_BULLETS, {36,53}},{SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {18,53}},
      {SFG_LEVEL_ELEMENT_MONSTER_PLASMABOT, {11,53}},{SFG_LEVEL_ELEMENT_BARREL, {3,53}},
      {SFG_LEVEL_ELEMENT_BULLETS, {58,57}},{SFG_LEVEL_ELEMENT_BULLETS, {56,57}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {44,57}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {3,57}},
      {SFG_LEVEL_ELEMENT_BARREL, {51,58}},{SFG_LEVEL_ELEMENT_BARREL, {50,58}},
      {SFG_LEVEL_ELEMENT_TERMINAL, {39,58}},{SFG_LEVEL_ELEMENT_LOCK0, {8,58}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {5,58}},{SFG_LEVEL_ELEMENT_BARREL, {51,59}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {3,59}},{SFG_LEVEL_ELEMENT_BULLETS, {20,61}},
      {SFG_LEVEL_ELEMENT_HEALTH, {3,61}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}}
    }, // elements
  }; // level

SFG_PROGRAM_MEMORY SFG_Level SFG_level4 =
  {          // level 4
    {        // mapArray
    #define o 0
    #define AA (5 | SFG_TILE_PROPERTY_DOOR)
    #define BB (10 | SFG_TILE_PROPERTY_DOOR)
    #define CC (6 | SFG_TILE_PROPERTY_DOOR)
    #define DD (8 | SFG_TILE_PROPERTY_DOOR)
    // 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 
/*0 */ 62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,60,60,62,62,62,62,62,62,62,62,62,o ,o ,o ,o ,o ,62,62,62,18,2 ,2 ,2 ,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18, /*0 */ 
/*1 */ 62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,o ,o ,62,62,62,62,62,62,62,62,62,o ,o ,o ,o ,o ,62,62,62,18,2 ,2 ,2 ,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18,18, /*1 */ 
/*2 */ 62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,o ,o ,62,62,62,62,62,62,62,62,62,o ,o ,o ,o ,o ,62,62,62,18,2 ,2 ,2 ,18,18,18,18,18,18,18,18,18,17,18,18,18,17,18,18,18,18,18,18, /*2 */ 
/*3 */ 62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,62,o ,o ,62,62,62,62,62,62,62,62,62,o ,o ,o ,o ,o ,62,62,62,18,2 ,2 ,2 ,17,18,18,18,17,18,18,18,7 ,7 ,7 ,7 ,7 ,7 ,7 ,17,18,18,18,18, /*3 */ 
/*4 */ 62,62,62,62,62,62,62,62,62,62,62,62,62,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,62,62,62,62,62,62,62,62,62,o ,o ,o ,o ,o ,62,62,62,18,2 ,2 ,2 ,17,2 ,2 ,2 ,2 ,2 ,2 ,2 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,2 ,2 ,2 ,2 ,18, /*4 */ 
/*5 */ o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,62,62,62,18,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,AA,7 ,7 ,7 ,7 ,7 ,7 ,7 ,2 ,2 ,2 ,2 ,18, /*5 */ 
/*6 */ o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,62,62,62,18,2 ,2 ,2 ,17,2 ,2 ,2 ,2 ,2 ,2 ,2 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,2 ,2 ,2 ,2 ,18, /*6 */ 
/*7 */ o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,62,62,62,18,18,18,18,17,18,18,18,17,18,18,18,7 ,7 ,7 ,7 ,7 ,7 ,7 ,17,2 ,2 ,2 ,17, /*7 */ 
/*8 */ o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,62,62,62,18,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,18,2 ,2 ,2 ,18, /*8 */ 
/*9 */ o ,o ,o ,o ,63,63,63,61,61,61,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,62,62,62,18,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,18,2 ,2 ,2 ,18, /*9 */ 
/*10*/ o ,o ,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,63,o ,o ,o ,o ,o ,62,62,62,62,62,62,62,18,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,17,2 ,2 ,2 ,17, /*10*/ 
/*11*/ 62,62,o ,o ,63,o ,o ,o ,o ,o ,60,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,63,o ,o ,o ,o ,o ,62,62,62,62,62,62,62,18,7 ,7 ,11,11,11,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,18,2 ,2 ,2 ,18, /*11*/ 
/*12*/ 62,62,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,59,59,59,o ,o ,63,o ,o ,o ,o ,o ,62,62,62,62,62,62,62,18,7 ,7 ,11,11,11,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,18,2 ,2 ,2 ,18, /*12*/ 
/*13*/ 62,62,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,59,59,59,o ,o ,63,o ,o ,o ,o ,o ,62,62,62,62,62,62,62,18,7 ,7 ,11,11,11,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,17,2 ,2 ,2 ,17, /*13*/ 
/*14*/ 62,62,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,59,59,59,o ,o ,63,o ,o ,o ,o ,o ,62,62,62,62,62,62,62,18,18,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,7 ,18,2 ,2 ,2 ,18, /*14*/ 
/*15*/ 62,62,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,63,o ,o ,o ,o ,o ,62,62,62,62,62,62,62,1 ,18,18,18,17,18,18,18,17,18,18,18,17,18,18,18,17,18,18,18,2 ,2 ,2 ,18, /*15*/ 
/*16*/ 62,62,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,63,o ,o ,o ,o ,o ,62,62,62,62,62,62,1 ,17,17,17,17,17,17,17,17,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,18,2 ,2 ,2 ,18, /*16*/ 
/*17*/ 62,62,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,63,o ,o ,o ,62,62,62,62,62,62,62,62,1 ,17,17,17,17,17,2 ,2 ,17,1 ,1 ,55,1 ,1 ,1 ,1 ,1 ,55,1 ,1 ,17,2 ,2 ,2 ,17, /*17*/ 
/*18*/ 62,62,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,63,o ,o ,o ,62,62,62,62,62,62,62,62,1 ,17,17,17,17,17,2 ,2 ,2 ,2 ,2 ,2 ,25,1 ,1 ,1 ,25,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 , /*18*/ 
/*19*/ 60,o ,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,63,o ,o ,o ,62,62,62,62,62,62,62,62,1 ,17,17,17,17,17,2 ,2 ,2 ,2 ,2 ,2 ,25,25,25,25,25,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 , /*19*/ 
/*20*/ 60,o ,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,63,o ,o ,o ,62,62,o ,o ,o ,o ,o ,62,1 ,17,17,17,17,17,2 ,2 ,17,1 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 , /*20*/ 
/*21*/ 60,o ,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,63,60,60,60,62,62,o ,o ,o ,o ,o ,62,1 ,17,17,17,17,17,17,17,1 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 , /*21*/ 
/*22*/ 60,o ,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,63,o ,o ,o ,62,62,60,60,60,60,60,o ,62,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 , /*22*/ 
/*23*/ 60,o ,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,55,10,BB,BB,BB,10,55,o ,o ,o ,o ,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,60,o ,62,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 , /*23*/ 
/*24*/ 60,o ,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,10,10,36,36,36,36,36,10,10,o ,o ,o ,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,o ,o ,60,o ,62,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,2 ,1 ,1 , /*24*/ 
/*25*/ 60,o ,o ,o ,63,o ,o ,o ,o ,o ,o ,o ,10,36,36,36,36,36,36,36,36,36,10,o ,o ,o ,o ,59,o ,o ,o ,o ,o ,o ,o ,o ,o ,60,o ,62,1 ,1 ,1 ,1 ,1 ,51,51,1 ,1 ,1 ,1 ,2 ,3 ,2 ,3 ,2 ,1 ,1 ,1 ,1 ,51,51,1 ,1 , /*25*/ 
/*26*/ 10,49,49,10,10,10,10,10,10,10,10,10,10,36,36,36,36,36,36,36,36,36,10,10,10,10,10,10,10,10,10,10,49,49,10,o ,o ,60,o ,62,62,1 ,1 ,1 ,1 ,51,51,1 ,1 ,1 ,1 ,2 ,CC,2 ,CC,2 ,1 ,1 ,1 ,1 ,51,51,1 ,1 , /*26*/ 
/*27*/ 10,33,33,33,45,46,46,46,46,46,46,46,46,36,36,36,36,36,36,36,36,36,46,46,46,46,46,46,46,46,45,33,33,33,10,o ,o ,60,o ,62,62,62,1 ,1 ,1 ,51,51,51,51,51,51,2 ,2 ,2 ,2 ,2 ,51,51,51,51,51,51,1 ,1 , /*27*/ 
/*28*/ 10,33,33,33,45,46,46,46,46,46,46,46,46,36,36,36,36,36,36,36,36,36,46,46,46,46,46,46,46,46,45,33,33,33,10,o ,o ,60,o ,62,62,62,62,1 ,1 ,51,51,51,51,51,51,2 ,2 ,2 ,2 ,2 ,51,51,51,51,51,51,1 ,1 , /*28*/ 
/*29*/ 10,33,33,33,45,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,46,45,33,33,33,10,60,60,60,60,8 ,62,62,62,62,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*29*/ 
/*30*/ 10,33,33,33,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,33,33,33,10,o ,o ,o ,o ,o ,o ,21,21,62,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*30*/ 
/*31*/ 10,34,34,34,10,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,10,34,34,34,10,o ,o ,o ,o ,o ,o ,o ,o ,62,62,62,62,62,62,1 ,1 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*31*/ 
/*32*/ 49,35,35,35,10,35,35,43,35,43,35,43,35,43,35,43,35,43,35,43,35,43,35,43,35,43,35,43,35,35,10,35,35,35,49,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,8 ,1 ,1 ,4 ,2 ,2 ,2 ,4 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*32*/ 
/*33*/ 49,35,35,35,10,35,34,42,34,42,34,42,34,42,34,42,34,42,34,42,34,42,34,42,34,42,34,42,34,35,10,35,35,35,49,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,8 ,1 ,1 ,1 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*33*/ 
/*34*/ 49,35,35,35,10,43,42,41,33,41,33,41,33,41,33,41,33,41,33,41,33,41,33,41,33,41,33,41,42,43,10,35,35,35,49,o ,o ,o ,o ,o ,o ,o ,o ,62,62,62,62,62,62,1 ,1 ,1 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*34*/ 
/*35*/ 10,35,35,35,DD,35,34,33,32,40,32,40,32,40,32,40,32,40,32,40,32,40,32,40,32,40,32,33,34,35,DD,35,35,35,10,o ,o ,o ,o ,o ,o ,o ,o ,62,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*35*/ 
/*36*/ 10,10,10,10,10,43,42,41,40,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,40,41,42,43,10,10,10,10,10,o ,o ,o ,o ,o ,o ,o ,o ,62,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,1 ,1 ,1 ,17,17,17,17,17,17, /*36*/ 
/*37*/ 10,9 ,9 ,9 ,9 ,9 ,34,33,32,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,32,33,34,9 ,9 ,9 ,9 ,9 ,10,o ,o ,60,60,60,60,60,60,62,1 ,1 ,1 ,25,25,25,25,25,1 ,2 ,1 ,1 ,1 ,1 ,17,30,30,30,17,17, /*37*/ 
/*38*/ 10,9 ,9 ,9 ,9 ,9 ,42,41,40,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,40,41,42,9 ,9 ,9 ,9 ,9 ,10,o ,o ,60,o ,o ,o ,o ,o ,62,1 ,1 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,29,30,30,30,17,17, /*38*/ 
/*39*/ 10,9 ,9 ,9 ,9 ,9 ,34,33,32,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,32,33,34,9 ,9 ,9 ,9 ,9 ,10,o ,o ,60,o ,o ,o ,o ,o ,62,1 ,1 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,29,30,30,30,17,17, /*39*/ 
/*40*/ 10,BB,10,10,8 ,43,42,41,40,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,40,41,42,43,8 ,10,10,BB,10,o ,o ,60,62,62,8 ,8 ,62,62,1 ,1 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,2 ,1 ,1 ,1 ,1 ,17,30,30,30,17,17, /*40*/ 
/*41*/ 10,9 ,9 ,9 ,8 ,35,34,33,32,36,22,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,22,36,32,33,34,35,8 ,9 ,9 ,9 ,10,o ,o ,60,62,1 ,1 ,1 ,1 ,1 ,17,1 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,2 ,1 ,1 ,1 ,1 ,17,17,30,17,17,17, /*41*/ 
/*42*/ 10,9 ,9 ,9 ,8 ,43,42,41,40,36,22,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,22,36,40,41,42,43,8 ,9 ,9 ,9 ,10,o ,o ,60,62,1 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,2 ,1 ,1 ,1 ,1 ,17,30,30,30,17,17, /*42*/ 
/*43*/ 10,9 ,9 ,9 ,8 ,35,34,33,32,36,22,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,22,36,32,33,34,35,8 ,9 ,9 ,9 ,10,o ,o ,60,62,1 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,2 ,1 ,1 ,1 ,1 ,17,30,30,30,17,17, /*43*/ 
/*44*/ 10,9 ,9 ,9 ,8 ,43,42,41,40,36,22,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,22,36,40,41,42,43,8 ,9 ,9 ,9 ,10,62,62,62,62,1 ,2 ,2 ,2 ,1 ,17,1 ,1 ,1 ,1 ,1 ,1 ,1 ,4 ,2 ,4 ,1 ,1 ,1 ,17,30,30,30,17,17, /*44*/ 
/*45*/ 10,9 ,9 ,9 ,8 ,35,34,33,32,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,32,33,34,35,8 ,9 ,9 ,9 ,10,62,62,1 ,1 ,1 ,29,29,29,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,1 ,1 ,1 ,17,17,17,17,17,17, /*45*/ 
/*46*/ 10,9 ,9 ,9 ,9 ,9 ,36,36,36,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,36,36,36,9 ,9 ,9 ,9 ,9 ,10,62,62,1 ,1 ,30,30,30,30,30,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*46*/ 
/*47*/ BB,9 ,9 ,9 ,9 ,9 ,36,36,36,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,36,36,36,9 ,9 ,9 ,9 ,9 ,BB,62,62,1 ,31,31,31,31,31,31,31,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*47*/ 
/*48*/ 10,9 ,9 ,9 ,9 ,9 ,36,36,36,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,36,36,36,9 ,9 ,9 ,9 ,9 ,10,62,62,1 ,31,31,31,31,31,31,31,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*48*/ 
/*49*/ 10,9 ,9 ,9 ,8 ,35,34,33,32,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,32,33,34,35,8 ,9 ,9 ,9 ,10,62,62,1 ,31,31,31,31,31,31,31,1 ,1 ,1 ,1 ,4 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,4 ,1 ,1 ,1 ,1 ,1 ,1 , /*49*/ 
/*50*/ 10,9 ,9 ,9 ,8 ,43,42,41,40,36,22,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,22,36,40,41,42,43,8 ,9 ,9 ,9 ,10,62,62,1 ,1 ,31,31,31,31,31,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 , /*50*/ 
/*51*/ 10,9 ,9 ,9 ,8 ,35,34,33,32,36,22,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,22,36,32,33,34,35,8 ,9 ,9 ,9 ,10,62,62,1 ,1 ,31,31,31,31,31,1 ,1 ,17,1 ,1 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,17,1 ,1 ,1 , /*51*/ 
/*52*/ 10,9 ,9 ,9 ,8 ,43,42,41,40,36,22,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,22,36,40,41,42,43,8 ,9 ,9 ,9 ,10,62,62,1 ,1 ,1 ,31,31,31,1 ,1 ,1 ,3 ,1 ,1 ,4 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,4 ,1 ,1 ,3 ,1 ,1 ,1 , /*52*/ 
/*53*/ 10,9 ,9 ,9 ,8 ,35,34,33,32,36,22,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,22,36,32,33,34,35,8 ,9 ,9 ,9 ,12,o ,62,62,62,1 ,31,31,31,1 ,1 ,1 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 , /*53*/ 
/*54*/ 10,BB,10,10,8 ,43,42,41,40,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,40,41,42,43,8 ,10,10,BB,10,o ,o ,21,8 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,2 ,1 ,1 ,1 , /*54*/ 
/*55*/ 10,9 ,9 ,9 ,9 ,9 ,34,33,32,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,32,33,34,9 ,9 ,9 ,9 ,9 ,10,o ,o ,21,8 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,2 ,29,29,2 ,2 ,2 ,29,29,2 ,2 ,2 ,2 ,1 ,1 ,1 , /*55*/ 
/*56*/ 10,9 ,9 ,9 ,9 ,9 ,42,41,40,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,40,41,42,9 ,9 ,9 ,9 ,9 ,10,o ,o ,o ,8 ,62,62,62,62,1 ,1 ,1 ,4 ,2 ,2 ,30,30,30,2 ,2 ,2 ,30,30,30,2 ,2 ,4 ,1 ,1 ,1 , /*56*/ 
/*57*/ 10,9 ,9 ,9 ,9 ,9 ,34,33,32,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,32,33,34,9 ,9 ,9 ,9 ,9 ,10,o ,o ,o ,o ,o ,o ,o ,62,1 ,1 ,1 ,1 ,1 ,2 ,30,30,30,2 ,2 ,2 ,30,30,30,2 ,1 ,1 ,1 ,1 ,1 , /*57*/ 
/*58*/ 10,10,10,10,10,43,42,41,40,36,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,o ,36,40,41,42,43,10,10,10,10,10,o ,o ,o ,o ,o ,o ,o ,62,1 ,1 ,1 ,1 ,1 ,2 ,30,30,30,2 ,2 ,2 ,30,30,30,2 ,1 ,1 ,1 ,1 ,1 , /*58*/ 
/*59*/ 10,35,35,35,DD,35,34,33,32,40,32,40,40,40,32,40,32,40,32,40,32,40,40,40,32,40,32,33,34,35,DD,35,35,35,10,60,60,60,60,60,60,60,62,1 ,1 ,1 ,1 ,1 ,1 ,30,30,30,2 ,2 ,2 ,30,30,30,1 ,1 ,1 ,1 ,1 ,1 , /*59*/ 
/*60*/ 49,35,35,35,8 ,43,42,41,33,41,33,41,33,41,33,41,33,41,33,41,33,41,33,41,33,41,33,41,42,43,8 ,35,35,35,49,o ,o ,o ,o ,o ,o ,o ,62,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*60*/ 
/*61*/ 49,35,35,35,8 ,35,34,42,34,42,34,42,34,42,34,42,34,42,34,42,34,42,34,42,34,42,34,42,34,35,8 ,35,35,35,49,o ,o ,o ,o ,o ,o ,o ,62,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*61*/ 
/*62*/ 49,35,35,35,8 ,35,35,43,35,43,35,43,35,43,35,43,35,43,35,43,35,43,35,43,35,43,35,43,35,35,8 ,35,35,35,49,o ,o ,o ,o ,o ,o ,o ,62,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,2 ,2 ,2 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 , /*62*/ 
/*63*/ 10,35,35,35,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,8 ,35,35,35,10,o ,o ,o ,o ,o ,o ,o ,62,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,17,BB,17,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1 ,1   /*63*/ 
    // 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 
    #undef AA
    #undef BB
    #undef CC
    #undef DD
    #undef o
    },
    {        // tileDictionary
      SFG_TD(19,31,1,0),SFG_TD(23, 0,4,4),SFG_TD(19, 7,4,4),SFG_TD(22, 4,6,0), // 0 
      SFG_TD(19, 3,0,3),SFG_TD(23, 3,3,4),SFG_TD(24, 5,4,4),SFG_TD(19,12,4,4), // 4 
      SFG_TD(31, 0,0,1),SFG_TD(19, 6,1,1),SFG_TD(23, 0,1,1),SFG_TD(20,11,1,4), // 8 
      SFG_TD(19, 4,1,1),SFG_TD(19,15,0,0),SFG_TD(19,15,0,0),SFG_TD(19,15,0,0), // 12 
      SFG_TD(19,15,0,0),SFG_TD(23, 0,1,1),SFG_TD(23, 0,2,4),SFG_TD(19,15,0,0), // 16 
      SFG_TD(19,15,0,0),SFG_TD(23,31,2,0),SFG_TD(21,31,2,0),SFG_TD(27,31,1,0), // 20 
      SFG_TD(19,15,0,0),SFG_TD(22, 3,4,4),SFG_TD(19,15,0,0),SFG_TD(19,15,0,0), // 24 
      SFG_TD(19,15,0,0),SFG_TD(18, 8,4,4),SFG_TD(17, 9,4,4),SFG_TD(16,10,4,4), // 28 
      SFG_TD(21,20,0,0),SFG_TD(23,18,0,0),SFG_TD(25,16,0,0),SFG_TD(27,14,0,0), // 32 
      SFG_TD(19,22,0,0),SFG_TD(19,15,0,0),SFG_TD(19,15,0,0),SFG_TD(19,15,0,0), // 36 
      SFG_TD(21,20,2,0),SFG_TD(23,18,2,0),SFG_TD(25,16,2,0),SFG_TD(27,14,2,0), // 40 
      SFG_TD(19,15,0,0),SFG_TD(21,10,0,1),SFG_TD(19,12,1,1),SFG_TD(19,15,0,0), // 44 
      SFG_TD(19,15,0,0),SFG_TD(29, 3,1,1),SFG_TD(19,15,0,0),SFG_TD(19, 6,4,4), // 48 
      SFG_TD(19,15,0,0),SFG_TD(19,15,0,0),SFG_TD(19,15,0,0),SFG_TD(31,31,3,0), // 52 
      SFG_TD(19,15,0,0),SFG_TD(19,15,0,0),SFG_TD(19,15,0,0),SFG_TD(21,31,1,0), // 56 
      SFG_TD(25,31,4,0),SFG_TD(22,31,2,0),SFG_TD(19, 0,5,5),SFG_TD(27,31,1,0)  // 60 
      },                    // tileDictionary
    {3 ,12,0 ,8 ,5 ,1 ,15}, // textureIndices
    7,                      // doorTextureIndex
    202,                     // floorColor
    66,                     // ceilingColor
    {53, 61, 240},          // player start: x, y, direction
    2,                      // backgroundImage
    {                       // elements
      {SFG_LEVEL_ELEMENT_LAMP, {58,3}},{SFG_LEVEL_ELEMENT_COLUMN, {51,4}},
      {SFG_LEVEL_ELEMENT_LOCK2, {51,5}},{SFG_LEVEL_ELEMENT_FINISH, {44,5}},
      {SFG_LEVEL_ELEMENT_COLUMN, {51,6}},{SFG_LEVEL_ELEMENT_LAMP, {58,7}},
      {SFG_LEVEL_ELEMENT_BLOCKER, {9,9}},{SFG_LEVEL_ELEMENT_BLOCKER, {8,9}},
      {SFG_LEVEL_ELEMENT_BLOCKER, {7,9}},{SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {51,10}},
      {SFG_LEVEL_ELEMENT_LAMP, {6,11}},{SFG_LEVEL_ELEMENT_TELEPORT, {44,12}},
      {SFG_LEVEL_ELEMENT_TELEPORT, {23,13}},{SFG_LEVEL_ELEMENT_HEALTH, {17,17}},
      {SFG_LEVEL_ELEMENT_BULLETS, {15,17}},{SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {17,18}},
      {SFG_LEVEL_ELEMENT_HEALTH, {55,19}},{SFG_LEVEL_ELEMENT_BULLETS, {53,19}},
      {SFG_LEVEL_ELEMENT_HEALTH, {51,19}},{SFG_LEVEL_ELEMENT_MONSTER_EXPLODER, {56,21}},
      {SFG_LEVEL_ELEMENT_LAMP, {20,22}},{SFG_LEVEL_ELEMENT_LAMP, {14,22}},
      {SFG_LEVEL_ELEMENT_RUIN, {26,24}},{SFG_LEVEL_ELEMENT_HEALTH, {8,24}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {18,25}},{SFG_LEVEL_ELEMENT_ROCKETS, {54,26}},
      {SFG_LEVEL_ELEMENT_PLASMA, {52,26}},{SFG_LEVEL_ELEMENT_HEALTH, {36,26}},
      {SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {16,26}},{SFG_LEVEL_ELEMENT_TREE, {18,29}},
      {SFG_LEVEL_ELEMENT_TREE, {16,29}},{SFG_LEVEL_ELEMENT_MONSTER_EXPLODER, {17,33}},
      {SFG_LEVEL_ELEMENT_ROCKETS, {51,37}},{SFG_LEVEL_ELEMENT_HEALTH, {49,37}},
      {SFG_LEVEL_ELEMENT_COLUMN, {19,37}},{SFG_LEVEL_ELEMENT_COLUMN, {15,37}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {60,38}},{SFG_LEVEL_ELEMENT_BULLETS, {30,38}},
      {SFG_LEVEL_ELEMENT_BARREL, {21,38}},{SFG_LEVEL_ELEMENT_BARREL, {13,38}},
      {SFG_LEVEL_ELEMENT_HEALTH, {4,38}},{SFG_LEVEL_ELEMENT_MONSTER_PLASMABOT, {20,40}},
      {SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {13,40}},{SFG_LEVEL_ELEMENT_HEALTH, {21,41}},
      {SFG_LEVEL_ELEMENT_PLASMA, {13,41}},{SFG_LEVEL_ELEMENT_PLASMA, {60,43}},
      {SFG_LEVEL_ELEMENT_BARREL, {17,46}},{SFG_LEVEL_ELEMENT_MONSTER_WARRIOR, {12,46}},
      {SFG_LEVEL_ELEMENT_MONSTER_PLASMABOT, {42,47}},{SFG_LEVEL_ELEMENT_MONSTER_PLASMABOT, {40,47}},
      {SFG_LEVEL_ELEMENT_LOCK0, {34,47}},{SFG_LEVEL_ELEMENT_BULLETS, {23,47}},
      {SFG_LEVEL_ELEMENT_BARREL, {18,47}},{SFG_LEVEL_ELEMENT_BARREL, {16,47}},
      {SFG_LEVEL_ELEMENT_ROCKETS, {11,47}},{SFG_LEVEL_ELEMENT_MONSTER_TURRET, {3,47}},
      {SFG_LEVEL_ELEMENT_LOCK0, {0,47}},{SFG_LEVEL_ELEMENT_BARREL, {41,48}},
      {SFG_LEVEL_ELEMENT_MONSTER_TURRET, {32,48}},{SFG_LEVEL_ELEMENT_MONSTER_EXPLODER, {21,48}},
      {SFG_LEVEL_ELEMENT_BARREL, {17,48}},{SFG_LEVEL_ELEMENT_BARREL, {42,49}},
      {SFG_LEVEL_ELEMENT_BARREL, {40,49}},{SFG_LEVEL_ELEMENT_PLASMA, {42,50}},
      {SFG_LEVEL_ELEMENT_PLASMA, {40,50}},{SFG_LEVEL_ELEMENT_ROCKETS, {43,51}},
      {SFG_LEVEL_ELEMENT_PLASMA, {41,51}},{SFG_LEVEL_ELEMENT_PLASMA, {39,51}},
      {SFG_LEVEL_ELEMENT_RUIN, {33,52}},{SFG_LEVEL_ELEMENT_TERMINAL, {41,53}},
      {SFG_LEVEL_ELEMENT_HEALTH, {40,53}},{SFG_LEVEL_ELEMENT_PLASMA, {21,53}},
      {SFG_LEVEL_ELEMENT_HEALTH, {13,53}},{SFG_LEVEL_ELEMENT_MONSTER_SPIDER, {21,54}},
      {SFG_LEVEL_ELEMENT_MONSTER_ENDER, {17,55}},{SFG_LEVEL_ELEMENT_MONSTER_PLASMABOT, {13,55}},
      {SFG_LEVEL_ELEMENT_HEALTH, {30,56}},{SFG_LEVEL_ELEMENT_BARREL, {21,56}},
      {SFG_LEVEL_ELEMENT_BARREL, {13,56}},{SFG_LEVEL_ELEMENT_ROCKETS, {4,56}},
      {SFG_LEVEL_ELEMENT_HEALTH, {41,57}},{SFG_LEVEL_ELEMENT_COLUMN, {19,57}},
      {SFG_LEVEL_ELEMENT_COLUMN, {15,57}},{SFG_LEVEL_ELEMENT_RUIN, {29,59}},
      {SFG_LEVEL_ELEMENT_LOCK0, {4,59}},{SFG_LEVEL_ELEMENT_LOCK1, {53,63}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}},
      {SFG_LEVEL_ELEMENT_NONE, {0,0}},{SFG_LEVEL_ELEMENT_NONE, {0,0}}
    }, // elements
  }; // level

SFG_PROGRAM_MEMORY SFG_Level SFG_level5 =
  #include "/home/tastyfish/git/anarch/assets/tmp.txt"
  ;

SFG_PROGRAM_MEMORY SFG_Level SFG_level6 =
  #include "/home/tastyfish/git/anarch/assets/tmp.txt"
  ;

SFG_PROGRAM_MEMORY SFG_Level SFG_level7 =
  #include "/home/tastyfish/git/anarch/assets/tmp.txt"
  ;

SFG_PROGRAM_MEMORY SFG_Level SFG_level8 =
  #include "/home/tastyfish/git/anarch/assets/tmp.txt"
  ;

SFG_PROGRAM_MEMORY SFG_Level SFG_level9 =
  #include "/home/tastyfish/git/anarch/assets/tmp.txt"
  ;

static const SFG_Level * SFG_levels[SFG_NUMBER_OF_LEVELS] =
{
  &SFG_level0, &SFG_level1, &SFG_level2, &SFG_level3, &SFG_level4, &SFG_level5,
  &SFG_level6, &SFG_level7, &SFG_level8, &SFG_level9
};

#endif // guard

