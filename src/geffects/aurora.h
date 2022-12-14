#include "matrix.h"
#include "Effects.h"
#include "Drawable.h"
#include "Boid.h"
#include "Attractor.h"
#include "Geometry.h"

#include "PatternAttract.h"
PatternAttract attract;
#include "PatternBounce.h"
PatternBounce bounce;
#include "PatternCube.h"
PatternCube cube;
#include "PatternFlock.h"
PatternFlock Aflock;
#include "PatternFlowField.h"
PatternFlowField flowfield;
#include "PatternIncrementalDrift.h"
PatternIncrementalDrift incrementaldrift;
#include "PatternIncrementalDrift2.h"
PatternIncrementalDrift2 incrementaldrift2;
#include "PatternPendulumWave.h"
PatternPendulumWave pendulumwave;
#include "PatternRadar.h"
PatternRadar radar;
#include "PatternSpiral.h"
PatternSpiral spiral;
#include "PatternSpiro.h"
PatternSpiro spiro;
#include "PatternSwirl.h"
PatternSwirl swirl;
#include "PatternWave.h"
PatternWave wave;

AuroraDrawable* items[] = {
    &attract,
    &bounce,
    &cube,
    &Aflock,
    &flowfield,
    &incrementaldrift,
    &incrementaldrift2,
    &pendulumwave,
    &radar,
    &spiral,
    &spiro,
    &swirl,
    &wave,
};
AuroraDrawable *pattern;

int8_t item = -1;
uint8_t numitems = sizeof(items) / sizeof(items[0]);

void SwitchPattern()
{
	pattern = items[item];
	pattern->start();
	Serial.print("Switching to pattern #");
	Serial.print(item);
	Serial.print(": ");
	Serial.println(pattern->name);
	matrix->clear();
}

void SetAuroraPattern(int pat)
{
	item=(pat % numitems);
	SwitchPattern();
}

void aurora_loop() 
{
    int8_t new_pattern = 0;
    char readchar;

	/*
    if (Serial.available()) readchar = Serial.read(); else readchar = 0;
    if (readchar) {
	while ((readchar >= '0') && (readchar <= '9')) {
	    new_pattern = 10 * new_pattern + (readchar - '0');
	    readchar = 0;
	    if (Serial.available()) readchar = Serial.read();
	}

	if (new_pattern) {
	    Serial.print("Got new pattern via serial ");
	    Serial.println(new_pattern);
	    item = new_pattern;
	} else {
	    Serial.print("Got serial char ");
	    Serial.println(readchar);
	}
    }
    if (readchar == 'n')      { Serial.println("Serial => next"); new_pattern = 1;  item++;}
    else if (readchar == 'p') { Serial.println("Serial => previous"); new_pattern = 1; item--;}
	*/

    /*
	EVERY_N_SECONDS(5) {
	new_pattern = 1;
	item++;
    }
	*/

    if (new_pattern || item == -1) 
	{ 
		if (item >= numitems) item = 0;
		if (item <= -1) item = numitems - 1;
		SwitchPattern();
    }
    pattern->drawFrame();
    matrix->show();
}


void aurora_setup() {
    //trix_setup();
    effects.leds = matrixleds;
    effects.Setup();
    matrix->clear();
}

// vim:sts=4:sw=4
