 /********************************************************************************************************************************************************
 *                                                                                                                                                       *
 *  Project:         FFT Spectrum Analyzer                                                                                                               *
 *  Target Platform: ESP32                                                                                                                               *
 *                                                                                                                                                       * 
 *  Version: 1.0                                                                                                                                         *
 *  Hardware setup: See github                                                                                                                           *
 *  Spectrum analyses done with analog chips MSGEQ7                                                                                                      *
 *                                                                                                                                                       * 
 *  Mark Donners                                                                                                                                         *
 *  The Electronic Engineer                                                                                                                              *
 *  Website:   www.theelectronicengineer.nl                                                                                                              *
 *  facebook:  https://www.facebook.com/TheelectronicEngineer                                                                                            *
 *  youtube:   https://www.youtube.com/channel/UCm5wy-2RoXGjG2F9wpDFF3w                                                                                  *
 *  github:    https://github.com/donnersm                                                                                                               *
 *                                                                                                                                                       *  
 ********************************************************************************************************************************************************/
 
/******************************************************************
* Setting below are only related to the demo Fire mode            *
*******************************************************************/
#define FPS 25              /* Refresh rate 15 looks good*/

/* Flare constants */
const uint8_t flarerows = 8;  //8  /* number of rows (from bottom) allowed to flare */
const uint8_t maxflare = 50;//4;     /* max number of simultaneous flares */
const uint8_t flarechance = 50; /* 50chance (%) of a new flare (if there's room) */
const uint8_t flaredecay = 14;  /* decay rate of flare radiation; 14 is good */

/* This is the map of colors from coolest (black) to hottest. Want blue flames? Go for it! */
#define NCOLORS 11
int FireColor=0;
const uint32_t fire_colors[3][NCOLORS] = {
	{
  0x000000,
  0x100000,
  0x300000,
  0x600000,
  0x800000,
  0xA00000,
  0xC02000,
  0xC04000,
  0xC06000,
  0xC08000,
  0x807080},
	{
  0x000000,
  0x001000,
  0x003000,
  0x006000,
  0x008000,
  0x00A000,
  0x00C020,
  0x00C040,
  0x00C060,
  0x00C080,
  0x808070
  },
	{
  0x000000,
  0x000010,
  0x000030,
  0x000060,
  0x000080,
  0x0000A0,
  0x2000C0,
  0x4000C0,
  0x6000C0,
  0x8000C0,
  0x708080
	}
  

};
 
 
 /*  This part of the code ( the fire part) has been adapted and heavly alterted from
 * Patrick Rigney (https://www.toggledbits.com/)
 * Github: https://github.com/toggledbits/MatrixFireFast */


#pragma once
#include 		"../../Settings.h"
#define MAT_W   kMatrixWidth          /* Size (columns) of entire matrix */
#define MAT_H   kMatrixHeight          /* and rows */
const uint16_t rows = MAT_H;
const uint16_t cols = MAT_W;
const uint16_t xorg = 0;
const uint16_t yorg = 0;
uint8_t pix[rows][cols];
//const uint8_t NCOLORS = (sizeof(fire_colors[0])/sizeof(fire_colors[0][0]));
uint8_t nflare = 0;
uint32_t flare[maxflare];
const uint8_t phy_h = MAT_W;
const uint8_t phy_w = MAT_H;
unsigned long t = 0; /* keep time */



uint16_t pos( uint16_t col, uint16_t row ) {
    uint16_t phy_x = xorg + (uint16_t) row;
    uint16_t phy_y = yorg + (uint16_t) col;
  return phy_x + phy_y * phy_w;
}

uint32_t isqrt(uint32_t n) {
  if ( n < 2 ) return n;
  uint32_t smallCandidate = isqrt(n >> 2) << 1;
  uint32_t largeCandidate = smallCandidate + 1;
  return (largeCandidate*largeCandidate > n) ? smallCandidate : largeCandidate;
}

// Set pixels to intensity around flare
void glow( int x, int y, int z ) {
  int b = z * 10 / flaredecay + 1;
  for ( int i=(y-b); i<(y+b); ++i ) {
    for ( int j=(x-b); j<(x+b); ++j ) {
      if ( i >=0 && j >= 0 && i < rows && j < cols ) {
        int d = ( flaredecay * isqrt((x-j)*(x-j) + (y-i)*(y-i)) + 5) / 10;
        uint8_t n = 0;
        if ( z > d ) n = z - d;
        if ( n > pix[i][j] ) { // can only get brighter
          pix[i][j] = n;
        }
      }
    }
  }
}

void newflare() {
  if ( nflare < maxflare && random(1,101) <= flarechance ) {
    int x = random(0, cols);
    int y = random(0, flarerows);
    int z = NCOLORS - 1;
    flare[nflare++] = (z<<16) | (y<<8) | (x&0xff);
    glow( x, y, z );
  }
}


void make_fire() {
  uint16_t i, j;

  if (t > millis()) return;
  t = millis() + (1000 / FPS);

  // First, move all existing heat points up the display and fade

  for (i = rows - 1; i > 0; --i) {
	for (j = 0; j < cols; ++j) {
	  uint8_t n = 0;
	  if (pix[i - 1][j] > 0)
		n = pix[i - 1][j] - 1;
	  pix[i][j] = n;
	}
  }

  // Heat the bottom row
  for (j = 0; j < cols; ++j) {
	i = pix[0][j];
	if (i > 0) {
	  pix[0][j] = random(NCOLORS - 6, NCOLORS - 2);
	}
  }

  // flare
  for (i = 0; i < nflare; ++i) {
	int x = flare[i] & 0xff;
	int y = (flare[i] >> 8) & 0xff;
	int z = (flare[i] >> 16) & 0xff;
	glow(x, y, z);
	if (z > 1) {
	  flare[i] = (flare[i] & 0xffff) | ((z - 1) << 16);
	} else {
	  // This flare is out
	  for (int j = i + 1; j < nflare; ++j) {
		flare[j - 1] = flare[j];
	  }
	  --nflare;
	}
  }
  newflare();

  const uint32_t *colors=fire_colors[FireColor];
  // Set and draw
  for (i = 0; i < rows; ++i) {
	for (j = 0; j < cols; ++j) {
	 // matrix -> drawPixel(j, rows - i, colors[pix[i][j]]);
	 #ifdef HUB75
	  CRGB COlsplit=colors[pix[i][j]];
	  dma_display->drawPixelRGB888(j,rows - i,COlsplit.r,COlsplit.g,COlsplit.b);
	 #endif
	 #ifdef Ledstrip
	  matrix -> drawPixel(j, rows - i, colors[pix[i][j]]);
	 #endif
	}
  }
}
