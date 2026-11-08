//
//  text.h
//  DOG_LCD
//
//  Created by Ruedi Heimlicher on 22.11.2013.
//
//

#ifndef DEFINES_h
#define DEFINES_h

#define BLINKRATE 0x0FFF

#define HOMESCREEN      0
#define MODELLSCREEN    1 // SYMPEL, TROTTLE ...
#define FUNKTIONSCREEN  2 // YAW,PITCH ...
#define AKTIONSCREEN    3 // LEVEL, EXPO ...


#define MODUSSCREEN     5

// 
#define  MODELL         0
#define  SIM            1
#define  CALIB          2

#define SAVE            1
#define CANCEL          0
#define CHANGED         2
#define CALIB_START     3
#define CALIB_END       4

#define arrow_width 16
#define arrow_height 16
/*
static unsigned char pfeil_l[] = {
   0b1,
   0b11,
   0b111,
   0b1111,
   0b11111,
   0b111111,
   0b1111111,
   0b11111111,
   0b1111111,
   0b111111,
   0b11111,
   0b1111,
   0b111,
   0b11,
   0b1


};
static unsigned char pfeil_r[] = 
{
   0b10000000,
   0b11000000,
   0b11100000,
   0b11110000,
   0b11111000,
   0b11111100,
   0b11111110,
   0b11111111,
   0b11111110,
   0b11111100,
   0b11111000,
   0b11110000,
   0b11100000,
   0b11000000,
   0b10000000
   
   };

   static unsigned char pfeil_d[] = {
  0x00, 0x00, // ................
  0x00, 0x00, // ................
  0x00, 0x00, // ................
  0x00, 0x00, // ................
  0x00, 0x00, // ................
  0x00, 0x00, // ................
  0x10, 0x00, // .......#........
  0x38, 0x00, // ......###.......
  0x7C, 0x00, // .....#####......
  0xFE, 0x00, // ....#######.....
  0xFF, 0x01, // ...#########....
  0xFF, 0x03, // ..###########...
  0xFF, 0x07, // .#############..
  0xFF, 0x0F, // ###############.
  0xFF, 0x1F, // ################
  0x00, 0x00  // ................
};
*/



   #define arrow_left_width 8
#define arrow_left_height 8

#endif


