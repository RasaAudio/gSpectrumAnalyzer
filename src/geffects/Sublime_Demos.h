// From Sublime for Novel Mutations Costume Controllers 2018, GPLv3
// https://github.com/Intrinsically-Sublime/esp8266-fastled-webserver
//
// Further adapted by Marc MERLIN for integration in FastLED::NeoMatrix
// standalone examples.

//#include "neomatrix_config.h"

/*
Before, with arrays
Heap Memory Available: 179392 bytes total, 86640 bytes largest free block
8-bit Accessible Memory Available: 92752 bytes total, 36576 bytes largest free block

After malloc cleanup
Heap Memory Available: 212344 bytes total, 86640 bytes largest free block
8-bit Accessible Memory Available: 125704 bytes total, 64624 bytes largest free block

39KB of static memory saved
*/

static uint8_t intensity = 42;  // was 255

CRGB solidColor = CRGB::Blue;
//
// Array of temp cells (used by fire, theMatrix, coloredRain, stormyRain)
uint8_t **tempMatrix;
uint8_t *splashArray;

CRGB solidRainColor = CRGB(60,80,90);

//CRGBPalette16 gCurrentPalette( CRGB::Black);

const CRGBPalette16 WoodFireColors_p = CRGBPalette16(CRGB::Black, CRGB::OrangeRed, CRGB::Orange, CRGB::Gold);		//* Orange
const CRGBPalette16 SodiumFireColors_p = CRGBPalette16(CRGB::Black, CRGB::Orange, CRGB::Gold, CRGB::Goldenrod);		//* Yellow
const CRGBPalette16 CopperFireColors_p = CRGBPalette16(CRGB::Black, CRGB::Green, CRGB::GreenYellow, CRGB::LimeGreen);	//* Green
const CRGBPalette16 AlcoholFireColors_p = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::DeepSkyBlue, CRGB::LightSkyBlue);//* Blue
const CRGBPalette16 RubidiumFireColors_p = CRGBPalette16(CRGB::Black, CRGB::Indigo, CRGB::Indigo, CRGB::DarkBlue);	//* Indigo
const CRGBPalette16 PotassiumFireColors_p = CRGBPalette16(CRGB::Black, CRGB::Indigo, CRGB::MediumPurple, CRGB::DeepPink);//* Violet
const CRGBPalette16 LithiumFireColors_p = CRGBPalette16(CRGB::Black, CRGB::FireBrick, CRGB::Pink, CRGB::DeepPink);	//* Red

const CRGBPalette16 palettes[] = {
	RainbowColors_p,
	RainbowStripeColors_p,
	CloudColors_p,
	LavaColors_p,
	OceanColors_p,
	ForestColors_p,
	PartyColors_p,
	HeatColors_p
};
const uint8_t paletteCount = ARRAY_SIZE(palettes);

CRGBPalette16 RotatingFire_p(WoodFireColors_p);

const CRGBPalette16 firePalettes[] = {
	WoodFireColors_p,
	SodiumFireColors_p,
	CopperFireColors_p,
	AlcoholFireColors_p,
	RubidiumFireColors_p,
	PotassiumFireColors_p,
	LithiumFireColors_p,
	RotatingFire_p
};

const uint8_t firePaletteCount = ARRAY_SIZE(firePalettes);

uint8_t currentPaletteIndex = 0;


// based on FastLED example Fire2012WithPalette: https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino
void fire()
{
#if NM_MATRIX_HEIGHT/6 > 6
	#define FIRE_BASE	6
#else
	#define FIRE_BASE	NM_MATRIX_HEIGHT/6+1
#endif
	//static uint8_t currentFirePaletteIndex = 0;
	static uint8_t currentFirePaletteIndex = firePaletteCount-1; // Force rotating fire palette
	// COOLING: How much does the air cool as it rises?
	// Less cooling = taller flames.  More cooling = shorter flames.
	uint8_t cooling = 70;
	// SPARKING: What chance (out of 255) is there that a new spark will be lit?
	// Higher chance = more roaring fire.  Lower chance = more flickery fire.
	uint8_t sparking = 130;
	// SMOOTHING; How much blending should be done between frames
	// Lower = more blending and smoother flames. Higher = less blending and flickery flames
	const uint8_t fireSmoothing = 80;

//#ifndef SUBLIME_INCLUDE
	FastLED.delay(1000/map8(speed,30,110));
//#endif
	// Add entropy to random number generator; we use a lot of it.
	random16_add_entropy(random(256));

	CRGBPalette16 fire_p( CRGB::Black);

	if (currentFirePaletteIndex < firePaletteCount-1) {
		fire_p = firePalettes[currentFirePaletteIndex];
	} else {
		fire_p = RotatingFire_p;
	}

	// Loop for each column individually
	for (int x = 0; x < NM_MATRIX_WIDTH; x++) {
		// Step 1.  Cool down every cell a little
		for (int i = 0; i < NM_MATRIX_HEIGHT; i++) {
			tempMatrix[x][i] = qsub8(tempMatrix[x][i], random(0, ((cooling * 10) / NM_MATRIX_HEIGHT) + 2));
		}

		// Step 2.  Heat from each cell drifts 'up' and diffuses a little
		for (int k = NM_MATRIX_HEIGHT; k > 1; k--) {
			tempMatrix[x][k] = (tempMatrix[x][k - 1] + tempMatrix[x][k - 2] + tempMatrix[x][k - 2]) / 3;
		}

		// Step 3.  Randomly ignite new 'sparks' of heat near the bottom
		if (random(255) < sparking) {
			int j = random(FIRE_BASE);
			tempMatrix[x][j] = qadd8(tempMatrix[x][j], random(160, 255));
		}

		// Step 4.  Map from heat cells to LED colors
		for (int y = 0; y < NM_MATRIX_HEIGHT; y++) {
			// Blend new data with previous frame. Average data between neighbouring pixels
			nblend(matrixleds[XY2(x,y)], ColorFromPalette(fire_p, ((tempMatrix[x][y]*0.7) + (tempMatrix[wrapX(x+1)][y]*0.3))), fireSmoothing);
		}
	}
}

void rain(byte backgroundDepth, byte maxBrightness, byte spawnFreq, byte tailLength, CRGB rainColor, bool splashes, bool clouds, bool storm)
{
	static uint16_t noiseX = random16();
	static uint16_t noiseY = random16();
	static uint16_t noiseZ = random16();

//#ifndef SUBLIME_INCLUDE
	FastLED.delay(1000/map8(speed,16,32));
//#endif

	CRGB lightningColor = CRGB(72,72,80);
	CRGBPalette16 rain_p( CRGB::Black, rainColor );
#ifdef SMARTMATRIX
	CRGBPalette16 rainClouds_p( CRGB::Black, CRGB(75,84,84), CRGB(49,75,75), CRGB::Black );
#else
	CRGBPalette16 rainClouds_p( CRGB::Black, CRGB(15,24,24), CRGB(9,15,15), CRGB::Black );
#endif

	fadeToBlackBy( matrixleds, NUM_LEDS, 255-tailLength);

	// Loop for each column individually
	for (int x = 0; x < NM_MATRIX_WIDTH; x++) {
		// Step 1.  Move each dot down one cell
		for (int i = 0; i < NM_MATRIX_HEIGHT; i++) {
			if (tempMatrix[x][i] >= backgroundDepth) {	// Don't move empty cells
				if (i > 0) tempMatrix[x][i-1] = tempMatrix[x][i];
				tempMatrix[x][i] = 0;
			}
		}

		// Step 2.  Randomly spawn new dots at top
		if (random(255) < spawnFreq) {
			tempMatrix[x][NM_MATRIX_HEIGHT-1] = random(backgroundDepth, maxBrightness);
		}

		// Step 3. Map from tempMatrix cells to LED colors
		for (int y = 0; y < NM_MATRIX_HEIGHT; y++) {
			if (tempMatrix[x][y] >= backgroundDepth) {	// Don't write out empty cells
				matrixleds[XY2(x,y)] = ColorFromPalette(rain_p, tempMatrix[x][y]);
			}
		}

		// Step 4. Add splash if called for
		if (splashes) {
			// FIXME, this is broken
			byte j = splashArray[x];
			byte v = tempMatrix[x][0];

			if (j >= backgroundDepth) {
				matrixleds[XY2(x-2,0,true)] = ColorFromPalette(rain_p, j/3);
				matrixleds[XY2(x+2,0,true)] = ColorFromPalette(rain_p, j/3);
				splashArray[x] = 0; 	// Reset splash
			}

			if (v >= backgroundDepth) {
				matrixleds[XY2(x-1,1,true)] = ColorFromPalette(rain_p, v/2);
				matrixleds[XY2(x+1,1,true)] = ColorFromPalette(rain_p, v/2);
				splashArray[x] = v;	// Prep splash for next frame
			}
		}

		// Step 5. Add lightning if called for
		if (storm) {
			//uint8_t lightning[NM_MATRIX_WIDTH][NM_MATRIX_HEIGHT];
			// ESP32 does not like static arrays  https://github.com/espressif/arduino-esp32/issues/2567
			uint8_t *lightning = (uint8_t *) malloc(NM_MATRIX_WIDTH * NM_MATRIX_HEIGHT);
			while (lightning == NULL) { Serial.println("lightning malloc failed"); }


			if (random16() < 72) {		// Odds of a lightning bolt
				lightning[scale8(random8(), NM_MATRIX_WIDTH-1) + (NM_MATRIX_HEIGHT-1) * NM_MATRIX_WIDTH] = 255;	// Random starting location
				for(int ly = NM_MATRIX_HEIGHT-1; ly > 1; ly--) {
					for (int lx = 1; lx < NM_MATRIX_WIDTH-1; lx++) {
						if (lightning[lx + ly * NM_MATRIX_WIDTH] == 255) {
							lightning[lx + ly * NM_MATRIX_WIDTH] = 0;
							uint8_t dir = random8(4);
							switch (dir) {
								case 0:
									matrixleds[XY2(lx+1,ly-1,true)] = lightningColor;
									lightning[wrapX(lx+1) + (ly-1) * NM_MATRIX_WIDTH] = 255;	// move down and right
								break;
								case 1:
									matrixleds[XY2(lx,ly-1,true)] = CRGB(128,128,128);
									lightning[lx + (ly-1) * NM_MATRIX_WIDTH] = 255;		// move down
								break;
								case 2:
									matrixleds[XY2(lx-1,ly-1,true)] = CRGB(128,128,128);
									lightning[wrapX(lx-1) + (ly-1) * NM_MATRIX_WIDTH] = 255;	// move down and left
								break;
								case 3:
									matrixleds[XY2(lx-1,ly-1,true)] = CRGB(128,128,128);
									lightning[wrapX(lx-1) + (ly-1) * NM_MATRIX_WIDTH] = 255;	// fork down and left
									matrixleds[XY2(lx-1,ly-1,true)] = CRGB(128,128,128);
									lightning[wrapX(lx+1) + (ly-1) * NM_MATRIX_WIDTH] = 255;	// fork down and right
								break;
							}
						}
					}
				}
			}
			free(lightning);
		}

		// Step 6. Add clouds if called for
		if (clouds) {
			uint16_t noiseScale = 250;	// A value of 1 will be so zoomed in, you'll mostly see solid colors. A value of 4011 will be very zoomed out and shimmery
			const uint16_t cloudHeight = (NM_MATRIX_HEIGHT*0.2)+1;

			// This is the array that we keep our computed noise values in
			//static uint8_t noise[NM_MATRIX_WIDTH][cloudHeight];
			static uint8_t *noise = (uint8_t *) malloc(NM_MATRIX_WIDTH * cloudHeight);
			while (noise == NULL) { Serial.println("noise malloc failed"); }
			int xoffset = noiseScale * x + gHue;

			for(int z = 0; z < cloudHeight; z++) {
				int yoffset = noiseScale * z - gHue;
				uint8_t dataSmoothing = 192;
				uint8_t noiseData = qsub8(inoise8(noiseX + xoffset,noiseY + yoffset,noiseZ),16);
				noiseData = qadd8(noiseData,scale8(noiseData,39));
				noise[x * cloudHeight + z] = scale8( noise[x * cloudHeight + z], dataSmoothing) + scale8( noiseData, 256 - dataSmoothing);
				nblend(matrixleds[XY2(x,NM_MATRIX_HEIGHT-z-1)], ColorFromPalette(rainClouds_p, noise[x * cloudHeight + z]), (cloudHeight-z)*(250/cloudHeight));
			}
			noiseZ ++;
		}
	}
}

void theMatrix()
{
	yield();
	// ( Depth of dots, maximum brightness, frequency of new dots, length of tails, color, splashes, clouds, ligthening )
	rain(60, 200, map8(intensity,5,100), 195, CRGB::Green, false, false, false);
}

void coloredRain()
{
	// ( Depth of dots, maximum brightness, frequency of new dots, length of tails, color, splashes, clouds, ligthening )
	//rain(60, 200, map8(intensity,2,60), 10, solidRainColor, true, true, false);
	rain(60, 180, map8(intensity,2,60), 30, solidRainColor, true, true, false);
}

void stormyRain()
{
	// ( Depth of dots, maximum brightness, frequency of new dots, length of tails, color, splashes, clouds, ligthening )
	//rain(0, 90, map8(intensity,0,150)+60, 10, solidRainColor, true, true, true);
	rain(60, 160, map8(intensity,0,100)+30, 30, solidRainColor, true, true, true);
}

void addGlitter( uint8_t chanceOfGlitter)
{
	if ( random8() < chanceOfGlitter) {
		matrixleds[ random16(NUM_LEDS) ] += CRGB::White;
	}
}

void bpm()
{
	// colored stripes pulsing at a defined Beats-Per-Minute (BPM)
	uint8_t beat = beatsin8(map8(speed,30,150), 64, 255);
	CRGBPalette16 palette = palettes[currentPaletteIndex];
	for ( int r = 0; r < NM_MATRIX_HEIGHT; r++) {
		for (uint16_t i = 0; i < NM_MATRIX_WIDTH; i++) {
			#ifdef REVERSE_ORDER
			matrixleds[XY2(i,NM_MATRIX_HEIGHT-1-r)] = ColorFromPalette(palette, gHue + (r * 2), beat - gHue + (r * 10));
			#else
			matrixleds[XY2(i,r)] = ColorFromPalette(palette, gHue + (r * 2), beat - gHue + (r * 10));
			#endif
		}
	}
}

void juggle()
{
	static uint8_t    numdots =   4; // Number of dots in use.
	static uint8_t   faderate =   2; // How long should the trails be. Very low value = longer trails.
	static uint8_t     hueinc =  255 / numdots - 1; // Incremental change in hue between each dot.
	static uint8_t    thishue =   0; // Starting hue.
	static uint8_t     curhue =   0; // The current hue
	static uint8_t    thissat = 255; // Saturation of the colour.
	static uint8_t thisbright = 255; // How bright should the LED/display be.
	static uint8_t   basebeat =   5; // Higher = faster movement.

	basebeat = map8(speed,5,30);

	static uint8_t lastSecond =  99;  // Static variable, means it's only defined once. This is our 'debounce' variable.
	uint8_t secondHand = (millis() / 1000) % 30; // IMPORTANT!!! Change '30' to a different value to change duration of the loop.

	if (lastSecond != secondHand) { // Debounce to make sure we're not repeating an assignment.
		lastSecond = secondHand;
		switch (secondHand) {
			case  0: numdots = 1; basebeat = 20; hueinc = 16; faderate = 2; thishue = 0; break; // You can change values here, one at a time , or altogether.
			case 10: numdots = 4; basebeat = 10; hueinc = 16; faderate = 8; thishue = 128; break;
			case 20: numdots = 8; basebeat =  3; hueinc =  0; faderate = 8; thishue = random8(); break; // Only gets called once, and not continuously for the next several seconds. Therefore, no rainbows.
			case 30: break;
		}
	}

	// Several colored dots, weaving in and out of sync with each other
	curhue = thishue; // Reset the hue values.
	fadeToBlackBy(matrixleds, NUM_LEDS, faderate);
	for ( int i = 0; i < numdots; i++) {
		uint16_t pos_n = beatsin16(basebeat + i + numdots, 0, NM_MATRIX_HEIGHT-1);
		for (uint16_t c = 0; c < NM_MATRIX_WIDTH; c++) {
			matrixleds[XY2(c,pos_n)] += CHSV(gHue + curhue, thissat, thisbright);
		}
		curhue += hueinc;
	}
}

// ColorWavesWithPalettes by Mark Kriegsman: https://gist.github.com/kriegsman/8281905786e8b2632aeb
// This function draws color waves with an ever-changing,
// widely-varying set of parameters, using a color palette.
void colorwaves( CRGB* ledarray, uint16_t numleds, CRGBPalette16& palette)
{
	static uint16_t sPseudotime = 0;
	static uint16_t sLastMillis = 0;
	static uint16_t sHue16 = 0;

	// uint8_t sat8 = beatsin88( 87, 220, 250);
	uint8_t brightdepth = beatsin88( 341, 96, 224);
	uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
	uint8_t msmultiplier = beatsin88(147, 23, 60);

	uint16_t hue16 = sHue16;//gHue * 256;
	uint16_t hueinc16 = beatsin88(113, 300, 1500);

	uint16_t ms = millis();
	uint16_t deltams = ms - sLastMillis ;
	sLastMillis  = ms;
	sPseudotime += deltams * msmultiplier;
	sHue16 += deltams * beatsin88( 400, 5, 9);
	uint16_t brightnesstheta16 = sPseudotime;

	for ( uint16_t i = 0 ; i < numleds; i++) {
		hue16 += hueinc16;
		uint8_t hue8 = hue16 / 256;
		uint16_t h16_128 = hue16 >> 7;
		if ( h16_128 & 0x100) {
			hue8 = 255 - (h16_128 >> 1);
		} else {
			hue8 = h16_128 >> 1;
		}

		brightnesstheta16  += brightnessthetainc16;
		uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

		uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
		uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
		bri8 += (255 - brightdepth);

		uint8_t index = hue8;
		//index = triwave8( index);
		index = scale8( index, 240);

		CRGB newcolor = ColorFromPalette( palette, index, bri8);

		uint16_t pixelnumber = i;
		pixelnumber = (numleds - 1) - pixelnumber;

		for (uint16_t c = 0; c < NM_MATRIX_WIDTH; c++) {
			#ifdef REVERSE_ORDER
			nblend( ledarray[XY2(c,numleds-1-pixelnumber)], newcolor, 128);
			#else
			nblend( ledarray[XY2(c,pixelnumber)], newcolor, 128);
			#endif
		}
	}
}

void colorWaves()
{
	colorwaves( matrixleds, NM_MATRIX_HEIGHT, gCurrentPalette);
}

// Pride2015 by Mark Kriegsman: https://gist.github.com/kriegsman/964de772d64c502760e5
// This function draws rainbows with an ever-changing, widely-varying set of parameters.
void pride()
{
	static uint16_t sPseudotime = 0;
	static uint16_t sLastMillis = 0;
	static uint16_t sHue16 = 0;

	uint8_t sat8 = beatsin88( 87, 220, 250);
	uint8_t brightdepth = beatsin88( 341, 200, 250);
	uint16_t brightnessthetainc16;
	uint8_t msmultiplier = beatsin88(147, 23, 60);

	brightnessthetainc16 = beatsin88( map(speed,1,255,150,475), (20 * 256), (40 * 256));

	uint16_t hue16 = sHue16;//gHue * 256;
	uint16_t hueinc16 = beatsin88(113, 1, 3000);

	uint16_t ms = millis();
	uint16_t deltams = ms - sLastMillis ;
	sLastMillis  = ms;
	sPseudotime += deltams * msmultiplier;
	sHue16 += deltams * beatsin88( 400, 5, 9);
	uint16_t brightnesstheta16 = sPseudotime;

	for ( uint32_t i = 0 ; i < NUMMATRIX; i++) {
		hue16 += hueinc16;
		uint8_t hue8 = hue16 / 256;

		brightnesstheta16  += brightnessthetainc16;
		uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

		uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
		uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
		bri8 += (255 - brightdepth);

		CRGB newcolor = CHSV( hue8, sat8, bri8);

		#ifdef REVERSE_ORDER
		uint16_t pixelnumber = (NUMMATRIX - 1) - i;
		#else
		uint16_t pixelnumber = i;
		#endif

		nblend( matrixleds[XY2(pixelnumber/NM_MATRIX_HEIGHT,pixelnumber%NM_MATRIX_HEIGHT)], newcolor, 64);
	}
}

void rainbow()
{
	CRGB tempHeightStrip[NM_MATRIX_HEIGHT];
	fill_rainbow(tempHeightStrip, NM_MATRIX_HEIGHT, gHue, 10);

	for (uint16_t x = 0; x < NM_MATRIX_WIDTH; x++) {
		for (int y = 0; y < NM_MATRIX_HEIGHT; y++) {
			#ifdef REVERSE_ORDER
			matrixleds[XY2(x,y)] = tempHeightStrip[y];
			#else
			matrixleds[XY2(x,y)] = tempHeightStrip[NM_MATRIX_HEIGHT-1-y];
			#endif
		}
	}
}

void rainbowWithGlitter()
{
	// built-in FastLED rainbow, plus some random sparkly glitter
	rainbow();
	addGlitter(80);
}

void sinelon()
{
	// This is not memory efficient if you use the function, but saves memory by avoiding 
	// a global if you never call the function
	static CRGB tempHeightStrip[NM_MATRIX_HEIGHT];
	// a colored dot sweeping back and forth, with fading trails
	fadeToBlackBy( tempHeightStrip, NM_MATRIX_HEIGHT, 20);
	int pos = beatsin16(map8(speed,30,150), 0, NM_MATRIX_HEIGHT - 1);
	static int prevpos = 0;
	CRGB color = ColorFromPalette(palettes[currentPaletteIndex], gHue, 255);

	if( pos < prevpos ) {
		fill_solid( tempHeightStrip+pos, (prevpos-pos)+1, color);
	} else {
		fill_solid( tempHeightStrip+prevpos, (pos-prevpos)+1, color);
	}

	for (uint16_t x = 0; x < NM_MATRIX_WIDTH; x++) {
		for (uint16_t y = 0; y < NM_MATRIX_HEIGHT; y++) {
			matrixleds[XY2(x,y)] = tempHeightStrip[y];
		}
	}
	prevpos = pos;
}

void sublime_setup() {
    // https://www.geeksforgeeks.org/dynamically-allocate-2d-array-c/
    tempMatrix = (uint8_t **) mallocordie("Sublime tempMatrix", (NM_MATRIX_WIDTH+1) * sizeof(int *) );

    for (uint16_t i=0; i < NM_MATRIX_WIDTH+1; i++) {
        tempMatrix[i] = (uint8_t *) mallocordie("Sublime tempMatrix[i]", NM_MATRIX_HEIGHT+1);
    }
    splashArray = (uint8_t *) mallocordie("Sublime splashArray", NM_MATRIX_WIDTH);
}

void sublime_reset() {
    for (uint16_t i=0; i < NM_MATRIX_WIDTH+1; i++) {
        memset(tempMatrix[i], 0, NM_MATRIX_HEIGHT+1);
    }
    memset(splashArray, 0, NM_MATRIX_WIDTH);
}



//#ifndef SUBLIME_INCLUDE
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
typedef void (*SimplePatternList[])();
const SimplePatternList gPatterns = { 
				stormyRain, 
				fire, 
				theMatrix, 
				coloredRain,  // 0-2
				bpm, 
				juggle,       // 3-5
				pride, 
				rainbow, 
				rainbowWithGlitter, // 6-8
				sinelon, //9 
				//colorWaves,  // broken until pallette support is added (but I don't like it so much)
};
#if 0
// Only use patterns that work ok and look good
SimplePatternList gPatterns = { 
				stormyRain, theMatrix, coloredRain,
				pride, fire, 
};
#endif

// fire, theMatrix, stormyRrain, pride
// debug colorWaves

void ChangePattern(int8_t dir)
{
  matrix->clear();
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + dir) % ARRAY_SIZE( gPatterns);
  Serial.print("Switched to pattern #"); Serial.println(gCurrentPatternNumber);
}


void SetSublimePattern(int8_t pat)
{
  matrix->clear();
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (pat) % ARRAY_SIZE( gPatterns);
  Serial.print("Switched to pattern #"); Serial.println(gCurrentPatternNumber);
}

/*
void setup() {
  matrix_setup();
  sublime_setup();
  sublime_reset();
  Serial.println("Setup done");
}
*/
//#endif // SUBLIME_INCLUDE

//#ifndef SUBLIME_INCLUDE
/*
void loop() {
        char readchar;
	if (Serial.available()) readchar = Serial.read(); else readchar = 0;

	switch(readchar) {
	case 'n': 
		Serial.println("Serial => next"); 
		ChangePattern(1);
		break;

	case 'p':
		Serial.println("Serial => previous");
		ChangePattern(-1);
		break;
	}
*/	
//#else
void sublime_loop() {
//#endif
	static int8_t rotatingFirePaletteIndex = 0;

	EVERY_N_SECONDS(3) {
		RotatingFire_p = firePalettes[ rotatingFirePaletteIndex ];
		rotatingFirePaletteIndex = addmod8( rotatingFirePaletteIndex, 1, firePaletteCount-1);
		//Serial.print("changed fire palette to ");
		//Serial.println(rotatingFirePaletteIndex);
	}



//#ifndef SUBLIME_INCLUDE
	EVERY_N_MILLISECONDS(40) { gHue++;  } // slowly cycle the "base color" through the rainbow
	//EVERY_N_SECONDS( 20 ) { ChangePattern(1); } // change patterns periodically

	// Call the current pattern function once, updating the 'matrixleds' array
	gPatterns[gCurrentPatternNumber]();

	matrix->show();
	// feed watchdog for ESP8266
	yield();
//#endif // SUBLIME_INCLUDE
}
