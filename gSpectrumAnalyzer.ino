/******************************************************************************************************************************************************** 
 *
 * Modified Version of FFT Spectrum Analyzer for ESP32 and fastled
 * Added Web UI for Config at runtime
 * Added Config Persisitence using the preference 
 * Added More Effects Aurora, Sublime , Twinke Fox.
 *
 ******************************************************************************************************************************************************** 

 ******************************************************************************************************************************************************** 
 *
 * ORIGINAL -  Please see below 
 * Project:    FFT Spectrum Analyzer
 * from
 * Website:    www.theelectronicengineer.nl         
 * 
 *  EffectsFrom
 *
 *  https://github.com/marcmerlin/FastLED_NeoMatrix_SmartMatrix_LEDMatrix_GFX_Demos 
 *  https://github.com/Intrinsically-Sublime/esp8266-fastled-webserver
 *
 ******************************************************************************************************************************************************** 
 
// FireworksXY
// Quick and dirty 2-D fireworks simulation using FastLED.
//
// Originaly designed for an Adafruit 5x8 WS2811 shield, but works fine
// on other XY matricies.
//
// by Mark Kriegsman, July 2013
//
// Updated by JP Roy to have multiple shell launches 2015
//
// Updated by Sublime for Novel Mutations Costume Controllers 2018
//
// Further adapted by Marc MERLIN for integration in FastLED::NeoMatrix
// standalone examples.
 
 ******************************************************************************************************************************************************** /
 
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
 ********************************************************************************************************************************************************* 
 * Version History                                                                                                                                       *
 *  1.0 First release, code extraced from 14 band spectrum analyzer 3.00 and modified to by used with FFT on a ESP32. No need for frequency board or     *
 *      MCGEQ7 chips.                                                                                                                                    *
 *      - HUB75 interface or                                                                                                                             *
 *      - WS2812 leds ( matrix/ledstrips)                                                                                                                *
 *      - 8/16/32 or 64 channel analyzer                                                                                                                 *
 *      - calibration for White noise, pink noise, brown noise sensitivity included and selectable                                                         *
 *      - Fire screensaver                                                                                                                               *
 *                                                                                                                                                       *
 *********************************************************************************************************************************************************
 * Version FFT 1.0 release July 2021                                                                                                                     *
 *********************************************************************************************************************************************************
 *  Status   | Description                                                                                                                               *
 *  Open     | Some Hub75 displays use a combination of chipsets of are from a different productions batch which will not work with this libary          *
 *  Open     | Sometime the long press for activating/de-activating the autoChange Pattern mode doesn't work                                             *
 *  Solved   | When using 64 bands, band 0 is always at max value. This was caused by the array dize [64]-> solved by chnaging it to 65                  *
 * Not a bug | Different types of HUB75 displays require different libary settings.It is what it is and it all depends on what the distributer sends you.*
 *           | For into on the libary settings, see the library documentation on Github: https://github.com/mrfaptastic/ESP32-HUB75-MatrixPanel-I2S-DMA  *
 * Wish      | Web interface. not possible without some heavy workaround cant use WIFI and ADC at same time                                              *
 * *******************************************************************************************************************************************************          
 * People who inspired me to do this build and figure out how stuff works:
 * Dave Plummer         https://www.youtube.com/channel/UCNzszbnvQeFzObW0ghk0Ckw
 * Mrfaptastic          https://github.com/mrfaptastic
 * Scott Marley         https://www.youtube.com/user/scottmarley85
 * Brian Lough          https://www.youtube.com/user/witnessmenow
 * atomic14             https://www.youtube.com/channel/UC4Otk-uDioJN0tg6s1QO9lw
 * 
 * Make sure your arduino IDE settings: Compiler warnings is set to default to make sure the code will compile                                           */

#define VERSION     "V1.1"

//************************************************************************

int ObuttonPushCounter =     0;                      // This number defines what pattern to start after boot (0 to 12)
int buttonPushCounter =     0;                      // This number defines what pattern to start after boot (0 to 12)
int autoChangePatterns =   1;                  // After boot, the pattern will not change automatically. 
int Peakdelay =             20;                     // Delay before peak falls down to stack. Overruled by PEAKDEALY Potmeter
#define BRIGHTNESSMAX       255                     // Max brightness of the leds...carefull...to bright might draw to much amps!
//int BRIGHTNESSMARK= 50;                            // Default brightnetss, however, overruled by the Brightness potmeter
int OBRIGHTNESSMARK= -1;                            // Default brightnetss, however, overruled by the Brightness potmeter
int BRIGHTNESSMARK= 25;                            // Default brightnetss, however, overruled by the Brightness potmeter
int BRIGHTNESSMIN = 5;                              // Min brightness

//************************************************************************

#ifndef WIN32

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_NeoPixel.h>
#include <Preferences.h>

#include <DNSServer.h>
#include <ESPmDNS.h>
#include <Update.h>
//************************************************************************
#include "neomatrix_config.h"

#include <FastLED_NeoMatrix.h>
#include <arduinoFFT.h>
#include <math.h>
#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#include "I2SPLUGIN.h"
#include "FFT.h"
#include "LEDDRIVER.H"
#include "Settings.h"

#include "src/geffects/fire.h"
#include "src/geffects/plasma.h"

#define  TWINKLEFOX_INCLUDE
#include "src/geffects/TwinkleFOX.h"

#define  SUBLIME_INCLUDE
#include "src/geffects/Sublime_Demos.h"


#include "src/geffects/aurora.h"
#include "src/geffects/fireworks.h"
#include "src/geffects/Vortex.h"

#else

#define numBands	32
#define SAMPLEBLOCK	1024

int speed,TWINKLE_SPEED;
//int PeakDirection;
int CalibrationType;


// R" does not work in Win32 so we load it from the file.
#define PAGE_SIZE	(128*1024)
#define LINE_SIZE	(10*1024)

char FLine[LINE_SIZE+10];

char setupindex[PAGE_SIZE+10];
char configindex[PAGE_SIZE+10];

void LoadFileFromName(char *fname,char *st)
{
	FILE *fp=fopen(fname,"rt");
	int i;
	st[0]='\0';
	if (fp!=0) 
	{
		for(i=0;;i++)
		{
			if (fgets(FLine,LINE_SIZE,fp)==0) break;
			if (strstr(FLine,"\"\"\"\"")!=0) continue;
			strncat(st,FLine,PAGE_SIZE);
		}
		fclose(fp);
	}
	else
	{
		CString st=fname;
		st+=" Missing";
		AfxMessageBox(st);
	}

}

void LoadFile()
{
	LoadFileFromName("src/gwebserver/html_setupindex.h",setupindex);
	LoadFileFromName("src/gwebserver/html_configindex.h",configindex);
}


#endif

#include "PatternsHUB75.h"
#include "PatternsLedstrip.h"                            

int skip=true;
int ButtonOnTimer=0;
int ButtonStarttime=0;
int ButtonSequenceCounter=0;
int ButtonOffTimer=0;
int ButtonStoptime=0;


int ButtonPressedCounter=0;
int ButtonReleasedCounter=0;
int ShortPressFlag=0;
int LongPressFlag=0;
int LongerPressFlag=0;
boolean Next_is_new_pressed= true;
boolean Next_is_new_release = true;
int PreviousPressTime=0;
#define up  1
#define down 0
int PeakDirection=0;
uint8_t SecToChangePattern=10;
uint8_t VUMeterPos=1;
long LastDoNothingTime = 0;       // only needed for screensaver
int DemoModeMem=0;                // to remember what mode we are in when going to demo, in order to restore it after wake up
bool AutoModeMem=false;           // same story
bool DemoFlag=false;              // we need to know if demo mode was manually selected or auto engadged. 

char LCDPrintBuf[30];

#define BUTTONPIN		32
// Last Effect Number
#define MAX_EFFECTS		50 
#define MAX_FFT_EFFECT	12

#define AUTO_CHANGE_NONE		0
#define AUTO_CHANGE_FFT_EFFECTS	1
#define AUTO_CHANGE_SS_EFFECTS	2	// Screen Saver

//************************************************************************
#define SETUP_MODE_PIN 		32 // Use the Same MAIN_PARAMS[MP_PIN] for Detecting Setup Mode
#define LED_PIN				14
#include "src/gwebserver/gwebserver.h"
//************************************************************************

void setup() 
{
	preferences.begin("gsa",false);

#ifndef WIN32
	matrix_setup();
	//Serial.begin(115200);
#endif

	Serial.println("Setting up Server");
	setup_server();
	Serial.println("Setting up Audio Input I2S");

	#ifndef WIN32
	setupI2S();
	Serial.println("Audio input setup completed");
	delay(1000);

	twinklefox_setup();
	sublime_setup();
	sublime_reset();
	aurora_setup();
	fireworks_setup();

	Serial.println("Setup done");

	#endif

	// #ifdef Ledstrip
	//  SetupLEDSTRIP(); // matrix_setup takes care of this.
	// #endif  

	#ifdef HUB75
	SetupHUB75();
	if (kMatrixHeight>60){
	dma_display->setBrightness8(100);
	}
	#endif

	pinMode(BUTTONPIN, INPUT_PULLUP);
	pinMode(LED_PIN, OUTPUT);
}

#ifdef WIN32

void loop() 
{
	loop_server();
}

#else
	
	void ClearVU()
	{
		if (ObuttonPushCounter!=buttonPushCounter) 
		{
			matrix->fillRect(0, 0, matrix->width(), 1, 0x0000); // delete the VU meter
			ObuttonPushCounter=buttonPushCounter;
		}
	}

	int GetNextSSEffect(int itm)
	{
		itm++;
		if (itm==12) return itm;
		if (itm==13) return itm;
		if (itm==14) return itm;
		if (itm==15) itm=20;
		if ((itm>=20)&&(itm<=42)) return itm;
		itm=12;// 43 will return it to 12
		return itm;
	}

	void ChangeCalibrationType()
	{
		  Serial.printf("Calibration table changed to: %s\n",Filtername[CalibrationType]);
		  sprintf(LCDPrintBuf,"Cal Filter: %s",Filtername[CalibrationType]);
		  DisplayPrint(LCDPrintBuf);
	}

	void loop() 
	{
	 size_t bytesRead = 0;
	 int TempADC=0;
	 
	 //Handle Userinterface
	 {
	  // set brightness and test if button is pressed
  
	  if (digitalRead(BUTTONPIN)==0)
	  { // ADC value < 10 so button is pressed
		ButtonOffTimer=0;
		ButtonOnTimer=millis()-ButtonStarttime;
		ButtonStoptime=millis();
	  }
	  else 
	  { // no Button pressed so ADC value is not related to button and can be proccessed
		  ButtonOnTimer=0;
		  ButtonOffTimer=millis()-ButtonStoptime;
		  ButtonStarttime = millis();
		  
			/*
			  TempADC = analogRead(BRIGHTNESSPOT);
			  if (TempADC<10){ // ADC value < 10 so button is pressed
				ButtonOffTimer=0;
				ButtonOnTimer=millis()-ButtonStarttime;
				ButtonStoptime=millis();
			  }
			  else { // no Button pressed so ADC value is not related to button and can be proccessed
			  ButtonOnTimer=0;
			  ButtonOffTimer=millis()-ButtonStoptime;
			  ButtonStarttime = millis();
		  
			  // read potmeters and process
			  Peakdelay=map(analogRead(PEAKDELAYPOT),0,4095,1,100);
			  BRIGHTNESSMARK=map(TempADC,100,2100,BRIGHTNESSMIN,BRIGHTNESSMAX);
			// dbgprint("potm:%d",BRIGHTNESSMARK);
			  #ifdef Ledstrip
				FastLED.setBrightness(BRIGHTNESSMARK);
			  #endif
			  #ifdef HUB75
				dma_display->setBrightness8(map(TempADC,100,2100,BRIGHTNESSMIN,BRIGHTNESSMAX));

			   #endif
		  
			*/ 

		}

	  if(ButtonOffTimer>ButtonTimeout)
	  {
		ButtonStoptime=millis(); // time that no switch was presset will reset the counter.
		ButtonSequenceCounter=0; // reset the sequencecounter
		if (ShortPressFlag==1)
		{
		 // Serial.printf("Short press detected\n");
		  buttonPushCounter = (buttonPushCounter + 1) % 13;
		  #ifdef HUB75
		  dma_display->clearScreen();
		  #endif
		  Serial.printf("Pattern Mode changed to: %d\n",buttonPushCounter);
		  ShortPressFlag=0;
		}
	  }
	  
	  if ((ButtonOnTimer>LongerPress)&&(ButtonOnTimer<(LongerPress+ShortPress))){LongerPressFlag=1;}
	  else if  ((ButtonOnTimer>LongPress)&&(ButtonOnTimer<(LongPress+ShortPress))){LongPressFlag=1;}
	  else if ((ButtonOnTimer>ShortPress)&&(ButtonOnTimer<(2*ShortPress)))
	  {
		ShortPressFlag=1;
		if((millis()-PreviousPressTime)<ButtonSequenceRepeatTime)
		{
		  ButtonSequenceCounter++;
		  dbgprint("Multible press counter: %d\n",ButtonSequenceCounter);
		  ShortPressFlag=0;
		  
			CalibrationType= (CalibrationType +1)% 4;
			ChangeCalibrationType();
		}
		PreviousPressTime=millis();
	  }
	  
	  if (LongerPressFlag==1)
	  { 
		dbgprint( "Longer press detected\n");
		autoChangePatterns = !autoChangePatterns;
		if(autoChangePatterns == true)
		{ 
		  Serial.print("Patterns wil now change every few seconds\n");
		  DisplayPrint((char*) "Autochange ON");
		}
		else 
		{
		  Serial.print("Automatically changing of pattern is now disabled\n");
		  DisplayPrint((char*) "Autochange OFF");
		}
		LongerPressFlag=0;
		ShortPressFlag=0;
	  } 
	  else if (LongPressFlag==1)
	  {
		dbgprint("long press detected\n");
		autoChangePatterns = !autoChangePatterns;
		if(autoChangePatterns == true)
		{ 
		  Serial.print("Patterns wil now change every few seconds\n");
		  DisplayPrint((char*) "Autochange ON");
		}
		else 
		{
		  Serial.print("Automatically changing of pattern is now disabled\n");
		  DisplayPrint((char*)"Autochange OFF");
		}
		LongPressFlag=0;
		ShortPressFlag=0;
	  }
	  
	 } // end user interface

 
	 if (buttonPushCounter<=MAX_FFT_EFFECT)
	 {
		 if (skip==false)i2s_adc_disable(I2S_NUM_0);
		 skip=false; // we only want to skip this the very first loop run.		 
		  //############ Step 1: read samples from the I2S Buffer ##################
		 i2s_adc_enable(I2S_NUM_0);

		 i2s_read(I2S_PORT, 
				 (void*)samples, 
				  sizeof(samples),  
				  &bytesRead,   // workaround This is the actual buffer size last half will be empty but why?
				  portMAX_DELAY); // no timeout

		 if (bytesRead != sizeof(samples))
		 {
		   Serial.printf("Could only read %u bytes of %u in FillBufferI2S()\n", bytesRead, sizeof(samples));
		   // return;
		 }


		 //############ Step 2: compensate for Channel number and offset, safe all to vReal Array   ############
		 for (uint16_t i = 0; i < ARRAYSIZE(samples); i++) 
		 {
		   vReal[i] = offset-samples[i];
		   vImag[i] = 0.0; //Imaginary part must be zeroed in case of looping to avoid wrong calculations and overflows
		   #if PrintADCRAW
			Serial.printf("%7d,",samples[i]); 
		   #endif

		   #if VisualizeAudio
			Serial.printf("%d\n",samples[i]);
		   #endif
		  }
	 
		   #if PrintADCRAW
			Serial.printf("\n"); 
		   #endif

		 //############ Step 3: Do FFT on the VReal array  ############
		  // compute FFT
		  FFT.DCRemoval();
		  FFT.Windowing(vReal, SAMPLEBLOCK, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
		  FFT.Compute(vReal, vImag, SAMPLEBLOCK, FFT_FORWARD);
		  FFT.ComplexToMagnitude(vReal, vImag, SAMPLEBLOCK);
		  FFT.MajorPeak(vReal, SAMPLEBLOCK, samplingFrequency); 
		  for (int i = 0; i < numBands; i++) 
		  {
			FreqBins[i] = 0;
		  }
		 //############ Step 4: Fill the frequency bins with the FFT Samples ############
		 float averageSum = 0.0f;
		 for (int i = 2; i < SAMPLEBLOCK / 2; i++)
		 { 
		   averageSum+=vReal[i];
		   if (vReal[i] > NoiseTresshold)
		   {
			 int freq = BucketFrequency(i);
			 int iBand = 0;
			 while (iBand < numBands)
			 {
			   if (freq < BandCutoffTable[iBand])break;
			   iBand++;
			 }
			 if (iBand > numBands)iBand = numBands;
			 FreqBins[iBand]+= vReal[i]; 
			 //  float scaledValue = vReal[i];
			 //  if (scaledValue > peak[iBand])
			 //    peak[iBand] = scaledValue;
		   }
		 }

		// bufmd[0]=FreqBins[12];
		 #if PrintRAWBins
		  for ( int y=0; y<numBands;y++)
		  {
			Serial.printf("%7.1f,",FreqBins[y]);
		  }
		  Serial.printf("\n");
		 #endif
	  
		 //############ Step 5: Determine the VU value  and mingle in the readout...( cheating the bands ) ############ Step 
		 float t=averageSum / (SAMPLEBLOCK / 2);
		 gVU = max(t, (oldVU * 3 + t) / 4);
		 oldVU = gVU; 
		 if(gVU>DemoTreshold)LastDoNothingTime = millis(); // if there is signal in any off the bands[>2] then no demo mode

		  // Serial.printf("gVu: %d\n",(int) gVU);

		 for(int j=0;j<numBands;j++)
		 {
		   if (CalibrationType==1)FreqBins[j]*= BandCalibration_Pink[j];
		   else if (CalibrationType==2)FreqBins[j]*= BandCalibration_White[j];
		   else if (CalibrationType==3)FreqBins[j]*= BandCalibration_Brown[j];
		 }



		 //*
		 //############ Step 6: Averaging and making it all fit on screen 
		 //for (int i = 0; i < numBands; i++) {
		   //Serial.printf ("Chan[%d]:%d",i,(int)FreqBins[i]);
			//FreqBins[i] = powf(FreqBins[i], gLogScale); // in case we want log scale..i leave it in here as reminder
		   //  Serial.printf( " - log: %d \n",(int)FreqBins[i]);
		// }
		 static float lastAllBandsPeak = 0.0f;
		 float allBandsPeak = 0;
		 //bufmd[1]=FreqBins[13];
		 //bufmd[2]=FreqBins[1];
		 for (int i = 0; i < numBands; i++)
		 {
		   //allBandsPeak = max (allBandsPeak, FreqBins[i]);
		   if (FreqBins[i]> allBandsPeak)
		   {
			 allBandsPeak = FreqBins[i];
		   }
		 }   
		 if (allBandsPeak < 1)allBandsPeak = 1;
		 //  The followinf picks allBandsPeak if it's gone up.  If it's gone down, it "averages" it by faking a running average of GAIN_DAMPEN past peaks
		 allBandsPeak = max(allBandsPeak, ((lastAllBandsPeak * (GAIN_DAMPEN-1)) + allBandsPeak) / GAIN_DAMPEN);  // Dampen rate of change a little bit on way down
		 lastAllBandsPeak = allBandsPeak;


		 if (allBandsPeak < 80000)allBandsPeak = 80000;
		 for (int i = 0; i < numBands; i++)
		 { 
		   FreqBins[i] /= (allBandsPeak * 1.0f);
		 }
		 

		 // Process the FFT data into bar heights
		 for (int band = 0; band < numBands; band++) 
		 {
		   int barHeight = FreqBins[band]*kMatrixHeight-1;  //(AMPLITUDE);
		   if (barHeight > TOP-2) barHeight = TOP-2;
	  
		   // Small amount of averaging between frames
		   barHeight = ((oldBarHeights[band] * 1) + barHeight) / 2;

		   // Move peak up
		   if (barHeight > peak[band]) {
			 peak[band] = min(TOP, barHeight);
			 PeakFlag[band]=1;
		   }
			bndcounter[band]+=barHeight; // ten behoeve calibratie

			// if there hasn't been much of a input signal for a longer time ( see settings ) go to demo mode
		   if ((millis() - LastDoNothingTime) > DemoAfterSec && DemoFlag==false)
		   { dbgprint("In loop 1:  %d", millis() - LastDoNothingTime);
			DemoFlag=true;
			// first store current mode so we can go back to it after wake up
			DemoModeMem=buttonPushCounter;
			AutoModeMem=autoChangePatterns;
			autoChangePatterns=false;
			buttonPushCounter=12;// Demo Mode.
			#ifdef HUB75
			dma_display->clearScreen();
			#endif
			dbgprint("Automode is turned of because of demo");
		   } 
		   // Wait,signal is back? then wakeup!     
			else if (DemoFlag==true &&   (millis() - LastDoNothingTime) < DemoAfterSec   )   
			{ //("In loop 2:  %d", millis() - LastDoNothingTime);
			  // while in demo the democounter was reset due to signal on one of the bars.
			  // So we need to exit demo mode.
			  #ifdef HUB75
			  dma_display->clearScreen();
			  #endif
			  buttonPushCounter=DemoModeMem; // restore settings
			  dbgprint ("automode setting restored to: %d",AutoModeMem);
			  autoChangePatterns=AutoModeMem;// restore settings
			  DemoFlag=false;  
			}
			#if BottomRowAlwaysOn
			  if (barHeight==0)barHeight=1; // make sure there is always one bar that lights up
			#endif
		  // Now visualize those bar heights
		  switch (buttonPushCounter) {
			case 0:
			 #ifdef HUB75
			  //PeakDirection=down;
			  BoxedBars(band, barHeight);
			  BluePeak(band);
			 #endif
			 #ifdef Ledstrip
			  changingBarsLS(band, barHeight);
			 #endif
			 break;
		 
			case 1: 
			 #ifdef HUB75
			  PeakDirection=down;
			  BoxedBars2(band, barHeight);
			  BluePeak(band);
			 #endif 
			 #ifdef Ledstrip
			 TriBarLS(band, barHeight);
			 TriPeakLS(band);
			 #endif
			 break;
			case 2:
			 #ifdef HUB75
			  PeakDirection=down;
			  BoxedBars3(band, barHeight);
			  RedPeak(band);
			 #endif
			 #ifdef Ledstrip
			  rainbowBarsLS(band, barHeight);
			  NormalPeakLS(band, PeakColor1);
			 #endif
			  break;
			case 3:
			 #ifdef HUB75
			  PeakDirection=down;
			  RedBars(band, barHeight); 
			  BluePeak(band);
			 #endif
			 #ifdef Ledstrip
			  purpleBarsLS(band, barHeight);
			  NormalPeakLS(band, PeakColor2);
			 #endif
			  break;
			case 4:
			 #ifdef HUB75
			  PeakDirection=down;
			  ColorBars(band, barHeight);
			 #endif
			 #ifdef Ledstrip
			  SameBarLS(band, barHeight); 
			  NormalPeakLS(band, PeakColor3);
			 #endif
			  break;
			case 5:
			 #ifdef HUB75
			  PeakDirection=down;
			  Twins(band, barHeight);
			  WhitePeak(band);
			 #endif
			 #ifdef Ledstrip
			  SameBar2LS(band, barHeight); 
			  NormalPeakLS(band, PeakColor3);
			 #endif
			 break;
			case 6:
			 #ifdef HUB75
			  PeakDirection=down;
			  Twins2(band, barHeight);
			  WhitePeak(band);
			 #endif
			 #ifdef Ledstrip
			  centerBarsLS(band, barHeight);
			 #endif
			  break;
			case 7:
			 #ifdef HUB75
			   PeakDirection=down;
			  TriBars(band, barHeight);
			  TriPeak(band);
			 #endif
			 #ifdef Ledstrip
			  centerBars2LS(band, barHeight);
			 #endif
			  break;
			case 8:
			 #ifdef HUB75 
			  PeakDirection=up;
			  TriBars(band, barHeight);
			  TriPeak(band);
			 #endif
			 #ifdef Ledstrip
			  centerBars3LS(band, barHeight);
			 #endif
			  break;
			case 9:
			 #ifdef HUB75
			 PeakDirection=down;
			 centerBars(band, barHeight);
			 #endif
			 #ifdef Ledstrip
			   BlackBarLS(band, barHeight);
			   outrunPeakLS(band);
			 #endif
			 break;
			case 10:
			 #ifdef HUB75
			 PeakDirection=down;
			 centerBars2(band, barHeight);
			 #endif
			 #ifdef Ledstrip
			  BlackBarLS(band, barHeight);
			  NormalPeakLS(band, PeakColor5);
			 #endif
			  break;
			case 11:
			 #ifdef HUB75
			  PeakDirection=down;
			  BlackBars(band, barHeight);
			  DoublePeak(band);     
			 #endif
			 #ifdef Ledstrip
			  BlackBarLS(band, barHeight);
			  TriPeak2LS(band);
			 #endif
			  break;
			case 12:
				matrix->fillRect(0, 0, matrix->width(), 1, 0x0000); // delete the VU meter
				if (ObuttonPushCounter!=buttonPushCounter) 
				{
					ObuttonPushCounter=buttonPushCounter;
				}
				FireColor=0;
				make_fire();				
				break;
		  } 
		  // Save oldBarHeights for averaging later
		  oldBarHeights[band] = barHeight;
		 } // for bands


// for calibration
		//bndcounter[h]+=barHeight;
		 if (loopcounter==256)
		 {
		  loopcounter=0;
		 #if CalibratieLog 
		   Calibration();
		   for(int g=0;g<numBands;g++)bndcounter[g]=0;
		 #endif
		 }
		 loopcounter++;
	 
		 if ((buttonPushCounter!=12)&&(VUMeterPos==1)) DrawVUMeter(0); // Draw it when not in screensaver mode
		 
		 #if PrintRAWBins
		  Serial.printf("\n");
		  //delay(10);
		 #endif

		   // Decay peak
		 EVERY_N_MILLISECONDS(Fallingspeed){
		   for (byte band = 0; band < numBands; band++){
			 if(PeakFlag[band]==1){
			   PeakTimer[band]++;
			   if (PeakTimer[band]> Peakdelay){PeakTimer[band]=0;PeakFlag[band]=0;}
			 }
			 else if ((peak[band] > 0) &&(PeakDirection==up)){ 
			   peak[band] += 1;
			   if (peak[band]>(kMatrixHeight+10))peak[band]=0;
			   } // when to far off screen then reset peak height
			 else if ((peak[band] > 0)&&(PeakDirection==down)){ peak[band] -= 1;}
		   }   
			 colorTimer++;
		 }

	 
		 EVERY_N_MILLISECONDS(10)colorTimer++; // Used in some of the patterns
	 }
	 else
	 {
		  switch(buttonPushCounter)
		  {
		  
			case 13:if (buttonPushCounter==12) FireColor=0;
			case 14:if (buttonPushCounter==13) FireColor=1;
			case 15:if (buttonPushCounter==14) FireColor=2;
			  
			 #ifdef HUB75
			  make_fire(); // go to demo mode
			 #endif
			 #ifdef Ledstrip
				matrix->fillRect(0, 0, matrix->width(), 1, 0x0000); // delete the VU meter
				if (ObuttonPushCounter!=buttonPushCounter) 
				{
					ObuttonPushCounter=buttonPushCounter;
				}
			  make_fire();
			 #endif
			 break;
			 
			 case 16:
				ClearVU();
				twinkle_loop();
				matrix->show();
				return;

			 case 17:
				ClearVU();
				plasma_loop();
				matrix->show();
				return;

				
			case 18:	
				ClearVU();
				fireworks() ;
				matrix->show();
				return;
				
			case 19:
				ClearVU();
				vortex();
				matrix->show();
				return;
		 }

		  if ((buttonPushCounter>=20)&&(buttonPushCounter<=29))
		  {
				if (ObuttonPushCounter!=buttonPushCounter) 
				{
					matrix->fillRect(0, 0, matrix->width(), 1, 0x0000); // delete the VU meter
					SetSublimePattern(buttonPushCounter-20);
					ObuttonPushCounter=buttonPushCounter;
				}
				sublime_loop();
		  }
		  if ((buttonPushCounter>=30)&&(buttonPushCounter<=42))
		  {
				if (ObuttonPushCounter!=buttonPushCounter) 
				{
					matrix->fillRect(0, 0, matrix->width(), 1, 0x0000); // delete the VU meter
					SetAuroraPattern(buttonPushCounter-30);
					ObuttonPushCounter=buttonPushCounter;
				}
				aurora_loop();
		  }
		  
	 }// Effects
 
	 EVERY_N_SECONDS(SecToChangePattern) {
	   // if (FastLED.getBrightness() == 0) FastLED.setBrightness(BRIGHTNESSMARK);  //Re-enable if lights are "off"
	   if (autoChangePatterns==AUTO_CHANGE_FFT_EFFECTS)
	   { 
		buttonPushCounter = (buttonPushCounter + 1) % MAX_EFFECTS;
		if (buttonPushCounter>(MAX_FFT_EFFECT-1)) buttonPushCounter=0; // Dont cycle to 12 its Fire Demo.
		#ifdef HUB75
		dma_display->clearScreen();
		#endif
	   }

	   if (autoChangePatterns==AUTO_CHANGE_SS_EFFECTS)
	   { 
		buttonPushCounter = GetNextSSEffect(buttonPushCounter);
		
		#ifdef HUB75
		dma_display->clearScreen();
		#endif
	   }
	   Serial.print("New Effect ");
	   Serial.println(buttonPushCounter);

	 }

 
	 #ifdef Ledstrip
	  delay(1); // needed to give fastled a minimum recovery time
	  FastLED.show();
	 #endif
  
	  yield();
	} // loop end

//***************************************************************************************************
	  // BucketFrequency
	  //
	  // Return the frequency corresponding to the Nth sample bucket.  Skips the first two 
	  // buckets which are overall amplitude and something else.

	int BucketFrequency(int iBucket)
	{
	 if (iBucket <= 1)return 0;
	 int iOffset = iBucket - 2;
	 return iOffset * (samplingFrequency / 2) / (SAMPLEBLOCK / 2);
	}

	void DrawVUPixels(int i, int yVU, int fadeBy = 0)
	{ 
	 CRGB VUC;
	 if (i>(PANE_WIDTH/3)){
	   VUC.r=255;
	   VUC.g=0;
	   VUC.b=0 ;
	 }
	 else if (i>(PANE_WIDTH/5)){
	   VUC.r=255;
	   VUC.g=255;
	   VUC.b=0;
	 }
	 else{ // green
	   VUC.r=0;
	   VUC.g=255;
	   VUC.b=0;
	 }
 
	 #ifdef Ledstrip
	  int xHalf = matrix->width()/2;
	//  matrix->drawPixel(xHalf-i-1, yVU, CRGB(0,100,0).fadeToBlackBy(fadeBy));
	//  matrix->drawPixel(xHalf+i,   yVU, CRGB(0,100,0).fadeToBlackBy(fadeBy));
	  matrix->drawPixel(xHalf-i-1, yVU, CRGB(VUC.r,VUC.g,VUC.b).fadeToBlackBy(fadeBy));
	  matrix->drawPixel(xHalf+i,   yVU, CRGB(VUC.r,VUC.g,VUC.b).fadeToBlackBy(fadeBy));
  
	 #endif

	 #ifdef HUB75
	  int xHalf = PANE_WIDTH/2;
	  dma_display->drawPixelRGB888(xHalf-i-2,yVU,VUC.r,VUC.g,VUC.b); //left side of screen line 0
	  dma_display->drawPixelRGB888(xHalf-i-2,yVU+1,VUC.r,VUC.g,VUC.b); //left side of screen line 1
	  dma_display->drawPixelRGB888(xHalf+i+1,yVU,VUC.r,VUC.g,VUC.b); // right side of screen line 0
	  dma_display->drawPixelRGB888(xHalf+i+1,yVU+1,VUC.r,VUC.g,VUC.b);// right side of screen line 1
	 #endif
	}



	void DrawVUMeter(int yVU)
	{
	 static int iPeakVUy = 0;        // size (in LED pixels) of the VU peak
	 static unsigned long msPeakVU = 0;       // timestamp in ms when that peak happened so we know how old it is
	 const int MAX_FADE = 256;
	 #ifdef HUB75
	  for(int x=0; x<PANE_WIDTH;x++){
		dma_display->drawPixelRGB888(x,yVU,0,0,0);
		dma_display->drawPixelRGB888(x,yVU+1,0,0,0);
	  }
	 #endif
	 #ifdef Ledstrip
	  matrix->fillRect(0, yVU, matrix->width(), 1, 0x0000);
	 #endif
	 if (iPeakVUy > 1)
	 {
	   int fade = MAX_FADE * (millis() - msPeakVU) / (float) 1000;
	   DrawVUPixels(iPeakVUy,   yVU, fade);
	 }
	 int xHalf = (PANE_WIDTH/2)-1;
	 int bars  = map(gVU, 0, MAX_VU, 1, xHalf);
	 bars = min(bars, xHalf);
	 if(bars > iPeakVUy)
	 {
	   msPeakVU = millis();
	   iPeakVUy = bars;
	 }
	 else if (millis() - msPeakVU > 1000)iPeakVUy = 0;
	 for (int i = 0; i < bars; i++)DrawVUPixels(i, yVU);
      
	}

	void Calibration(void)
	{
	  Serial.printf("BandCalibration_XXXX[%1d]=\n{",numBands);
	  long Totalbnd=0;
  
	  for (int g=0;g<numBands;g++){
		if (bndcounter[g]>Totalbnd)Totalbnd=bndcounter[g];
	  }
  
  
	  for (int g=0;g<numBands;g++){
		bndcounter[g]=Totalbnd/bndcounter[g];
		Serial.printf(" %2.2f",bndcounter[g]);
		if(g<numBands-1)Serial.printf(",");
		else Serial.print(" };\n");
	  }
	}

	//**************************************************************************************************
	//                                          D B G P R I N T                                        *
	//**************************************************************************************************
	// Send a line of info to serial output.  Works like vsprintf(), but checks the DEBUG flag.        *
	// Print only if DEBUG flag is true.  Always returns the formatted string.                         *
	// Usage dbgprint("this is the text you want: %d", variable);
	//**************************************************************************************************
	
	void dbgprint(const char * format, ...) {
	  if (DEBUG) {
		static char sbuf[DEBUG_BUFFER_SIZE]; // For debug lines
		va_list varArgs; // For variable number of params
		va_start(varArgs, format); // Prepare parameters
		vsnprintf(sbuf, sizeof(sbuf), format, varArgs); // Format the message
		va_end(varArgs); // End of using parameters
		if (DEBUG) // DEBUG on?
		{
		  Serial.print("Debug: "); // Yes, print prefix
		  Serial.println(sbuf); // and the info
		}
	   // return sbuf; // Return stored string
	  }
	}

	void DisplayPrint(char * text){
	 #ifdef HUB75
	   dma_display->fillRect(8, 8, kMatrixWidth-16, 11, dma_display->color444(0,0 , 0));
	   dma_display->setTextSize(1);
	   dma_display->setTextWrap(false);
	   dma_display->setCursor(10,10);
	   dma_display->print(text);
	   delay(1000);
	   dma_display->fillRect(8, 8, kMatrixWidth-16, 11, dma_display->color444(0,0 , 0));   
	 #endif 
	}

#endif // Win32