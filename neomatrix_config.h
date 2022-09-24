#include <Adafruit_NeoPixel.h>
#include <FastLED_NeoMatrix.h>

#pragma once

uint8_t matrix_brightness = 64;
#define mmin(a,b) ((a<b)?(a):(b))
#define mmax(a,b) ((a>b)?(a):(b))
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

CRGBPalette16 gCurrentPalette;

//**********************************************************************************************************

#define CHIPSET         WS2812B                     // LED strip type
#define LED_PIN         21                          // LED strip data
//#define SERPENTINE     false                        // Set to false if you're LEDS are connected end to end, true if serpentine
#define COLOR_ORDER     GRB                         // If colours look wrong, play with this
#define LED_VOLTS       5                           // Usually 5 or 12
#define MAX_MILLIAMPS   2000                        // Careful with the amount of power here if running off USB port

#define MATRIX_TILE_WIDTH	32
#define MATRIX_TILE_HEIGHT	8
#define MATRIX_TILE_H		1
#define MATRIX_TILE_V		2

const uint8_t kMatrixWidth =  MATRIX_TILE_WIDTH *  MATRIX_TILE_H;
const uint8_t kMatrixHeight = MATRIX_TILE_HEIGHT * MATRIX_TILE_V;

const uint16_t mw = MATRIX_TILE_WIDTH *  MATRIX_TILE_H;
const uint16_t mh = MATRIX_TILE_HEIGHT * MATRIX_TILE_V;

const uint32_t NUMMATRIX = mw*mh;
const uint32_t NUM_LEDS = NUMMATRIX;

#define LED_COUNT   (( MATRIX_TILE_WIDTH *  MATRIX_TILE_H) * (MATRIX_TILE_HEIGHT * MATRIX_TILE_V))   // Total number of LEDs
CRGB matrixleds[LED_COUNT];

FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(matrixleds, MATRIX_TILE_WIDTH, MATRIX_TILE_HEIGHT, MATRIX_TILE_H, MATRIX_TILE_V, 
  NEO_MATRIX_BOTTOM     + NEO_MATRIX_RIGHT +
    NEO_MATRIX_COLUMNS + NEO_TILE_TOP + NEO_MATRIX_ZIGZAG );
	
void matrix_setup()
{
  Serial.begin(115200);
  Serial.println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> Serial.begin");
	
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(matrixleds, NUM_LEDS).setCorrection(TypicalSMD5050);
  FastLED.setMaxPowerInVoltsAndMilliamps(LED_VOLTS, MAX_MILLIAMPS);
  FastLED.setBrightness(matrix_brightness);
  FastLED.clear();
}



//**********************************************************************************************************

const uint16_t NM_MATRIX_WIDTH  = MATRIX_TILE_WIDTH *  MATRIX_TILE_H;
const uint16_t NM_MATRIX_HEIGHT = MATRIX_TILE_HEIGHT * MATRIX_TILE_V;


uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint16_t speed = 255;

float matrix_gamma = 1; // higher number is darker, needed for Neomatrix more than SmartMatrix

// Like XY, but for a mirror image from the top (used by misconfigured code)
int XY2( int x, int y, bool wrap=false) {
    wrap = wrap; // squelch compiler warning
    return matrix->XY(x,NM_MATRIX_HEIGHT-1-y);
}

// FastLED::colorutils needs a signature with uint8_t
uint16_t XY( uint8_t x, uint8_t y) {
    return matrix->XY(x,y);
}
// but x/y can be bigger than 256
uint16_t XY16( uint16_t x, uint16_t y) {
    return matrix->XY(x,y);
}

int wrapX(int x) {
    if (x < 0 ) return 0;
    if (x >= NM_MATRIX_WIDTH) return (NM_MATRIX_WIDTH-1);
    return x;
}

void show_free_mem(const char *pre=NULL) {
    Framebuffer_GFX::show_free_mem(pre);
}

void die(const char *mesg) {
    Serial.println(mesg);
    while(1) delay((uint32_t)1); // while 1 loop only triggers watchdog on ESP chips
}

void *mallocordie(const char *varname, uint32_t req, bool psram=true) {
    // If varname starts with @, show debug for the allocation
    void *mem;

#ifndef BOARD_HAS_PSRAM
    psram = false;
#endif
    if (varname[0] == '@') {
        if (psram) Serial.print("PS");
        Serial.print("Malloc ");
        Serial.print(varname);
        Serial.print(" . Requested bytes: ");
        Serial.println(req);
    }
#ifdef ESP32
    if (psram) {
        mem = ps_malloc(req);
    } else {
        mem = malloc(req);
    }
#else
    mem = malloc(req);
#endif

    if (mem) {
        return mem;
    } else {
        show_free_mem();
        Serial.println("vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv");
        Serial.print("FATAL: ");
        if (psram) Serial.print("ps_");
        Serial.print("malloc failed for ");
        Serial.print(varname);
        Serial.print(" . Requested bytes: ");
        Serial.println(req);
        Serial.println("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^");
        while (1); // delay(1);  Adding this seems to cause an ESP32 bug
    }
    return NULL;
}

uint32_t millisdiff(uint32_t before) {
    return((millis()-before) ? (millis()-before): 1);
}
