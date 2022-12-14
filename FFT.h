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
 
#pragma once
// Depending on how many bands have been defined, one of these tables will contain the frequency
// cutoffs for that "size" of a spectrum display. 
// Only one of the following should be 1, rest has to be 0 
// WARNING amke sure your math add up.. if you select 24 bands on a 32 wide matrix....you'll need to adjust code 
#define bands8  0
#define bands16 0
#define bands24 0
#define bands32 1
#define bands64 0  
int CalibrationType=0;  //0=none, 1=White, 2=Pink     
char Filtername[4][6]={
  "None",  // see documentation on this one.
  "Pink",
  "White",
  "Brown"
};
#if bands8
const int numBands = 8;
  static int BandCutoffTable[8] =
  {
   20, 150, 400, 550, 675, 780, 900, 1200
  };

  static double BandCalibration_Pink[8]=
  { 4.08, 1.56, 1.00, 2.80, 3.23, 3.63, 6.19, 1.65 };
  static double BandCalibration_White[8]=
  { 25.84, 3.43, 1.10, 2.95, 3.37, 3.39, 5.46, 1.00 };
  static double BandCalibration_Brown[8]=
  { 2.72, 1.17, 1.00, 3.34, 4.83, 5.57, 11.18, 3.08 };
#endif

#if bands16
  const int numBands = 16;
  static int BandCutoffTable[16] =
  {
   40, 60, 100, 150, 250, 400, 650, 1000, 1600, 2500, 4000, 6250, 12000, 14000, 16000, 17000
  };

  static double BandCalibration_Pink[16]=
{ 4.52, 5.48, 5.54, 6.06, 2.98, 1.72, 1.49, 1.36, 1.00, 1.49, 2.04, 1.71, 2.19, 2.68, 1.85, 5.69 };

  static double BandCalibration_White[16]=
{ 169.55, 148.58, 141.29, 124.24, 35.59, 10.76, 6.97, 5.18, 2.89, 4.19, 4.24, 1.99, 1.00, 1.60, 1.53, 5.48 }; 
    static double BandCalibration_Brown[16]=
{ 1.81, 2.17, 2.49, 2.89, 1.57, 1.00, 1.10, 1.30, 1.22, 3.74, 113.96, 774.90, 7.76, 645.75, 2583.00, 7749.00 };

#endif

#if bands24
  const int numBands = 24;
  static int BandCutoffTable[24] =
  {
    40, 80, 150, 220, 270, 320, 380, 440, 540, 630,  800, 1000, 1250, 1600, 2000, 2500, 3150, 
    3800, 4200, 4800, 5400, 6200, 7400, 12500 
  };

  static double BandCalibration_Pink[24]=
  { 1.08, 1.19, 1.06, 1.21, 1.16, 1.09, 1.10, 1.15, 1.14, 1.31, 1.11, 1.05, 1.01, 1.00, 1.13, 1.34, 1.65, 1.68, 1.81, 1.78, 1.83, 2.02, 1.77, 1.18 };
  static double BandCalibration_White[24]=
  { 152.55, 134.78, 38.14, 28.98, 103.60, 111.06, 102.83, 28.74, 29.98, 33.78, 12.07, 8.28, 6.79, 5.76, 7.29, 11.03, 13.72, 10.98, 14.86, 8.20, 9.17, 6.91, 3.95, 1.00 };
  static double BandCalibration_Brown[24]=
  { 1.41, 1.61, 1.00, 1.30, 3.72, 4.18, 4.66, 2.20, 3.00, 3.46, 1.93, 1.93, 2.04, 2.33, 4.30, 16.74, 113.82, 626.00, 1043.33, 1565.00, 2086.67, 2086, 122.75, 2.86 };
#endif

int loopcounter=0;
double bndcounter[64];
#if bands32
  const int numBands = 32;
  
  static int BandCutoffTable[32] = 
  {
    45, 90, 130, 180, 220, 260, 310, 350, 
    390, 440, 480, 525, 650, 825, 1000, 1300, 
    1600, 2050, 2500, 3000, 4000, 5125, 6250, 9125, 
    12000, 13000, 14000, 15000, 16000, 16500, 17000, 17500
  };
  /*
  
  static int BandCutoffTable[32] = 
  {
    45, 90, 130, 180, 220, 260, 310, 350, 
    390, 440, 480, 525, 650, 825, 1000, 1300, 
    1600, 2050, 2500, 3000, 4000, 5500, 6000, 6500, 
    7000, 7500, 8000, 8500, 9000, 10000, 11000, 12000
  };
	*/


  static double BandCalibration_Pink[32]=
  { 1.17, 3.09, 3.07, 3.53, 3.95, 4.22, 4.43, 5.02, 5.59, 5.63, 6.27, 7.32, 1.90, 1.51, 1.66, 1.00, 1.31, 1.31, 2.58, 4.25, 1.78, 1.50, 2.75, 2.95, 2.34, 4.81, 2.41, 2.16, 2.08, 7.30, 9.01, 10.48 };
  // PSRK
  //{ 1.88, 1.39, 1.76, 1.62, 1.26, 1.52, 1.52, 1.36, 1.53, 1.61, 1.44, 1.26, 2.59, 3.10, 4.01, 5.47, 4.80, 4.72, 2.04, 1.51, 3.36, 5.00, 2.70, 1.21, 1.00, 1.81, 4.24, 4.64, 4.96, 1.29, 1.95, 1.30 };

  static double BandCalibration_White[32]=
  { 22.21, 65.51, 62.36, 60.53, 57.51, 52.28, 48.14, 49.53, 49.06, 50.49, 58.15, 60.89, 9.73, 6.36, 6.01, 3.17, 3.67, 3.59, 6.76, 9.03, 2.86, 1.91, 2.09, 1.16, 1.00, 2.08, 1.64, 1.62, 1.73, 4.89, 6.98, 9.08 };
  
  static double BandCalibration_Brown[32]=
  { 1.00, 2.82, 3.26, 3.68, 4.21, 5.15, 5.57, 5.98, 7.06, 7.53, 9.94, 11.47, 3.22, 2.88, 3.53, 2.48, 3.88, 5.37, 43.13, 312.68, 224.49, 729.58, 2188.75, 2188.75, 12.35, 8755, 2188.75, 1250.71, 2918.33, 8755, 8755.00, 8755.00 };
  
#endif

#if bands64
  const int numBands = 64;
  
  static int BandCutoffTable[64] = 
  {
       45, 90, 130, 180, 220, 260, 310, 350
    , 390, 440, 480, 525, 565, 610, 650, 690
    , 735, 780, 820, 875, 920, 950, 1000, 1050
    , 1080, 1120, 1170, 1210, 1250, 1300, 1340, 1380
    , 1430, 1470, 1510, 1560, 1616, 1767, 1932, 2113
    , 2310, 2526, 2762, 3019, 3301, 3610, 3947, 4315
    , 4718, 5159, 5640, 6167, 6743, 7372, 8061, 8813
    , 9636, 10536, 11520, 12595, 13771, 15057, 16463, 18000
    
  };


   static double BandCalibration_Pink[64]=
   { 1.00, 3.16, 3.12, 3.46, 3.33, 3.56, 4.34, 4.29, 4.58, 5.75, 5.74, 6.64, 7.02, 7.22, 8.25, 8.69, 9.28, 9.65, 9.80, 10.24, 9.01, 10.62, 11.05, 10.71, 10.55, 11.31, 12.66, 11.77, 14.08, 
     16.30, 15.82, 15.28, 16.75, 20.22, 21.95, 19.97, 23.89, 2.97, 6.48, 4.28, 9.37, 9.71, 11.49, 12.89, 9.65, 10.46, 8.03, 5.54, 7.16, 7.42, 7.92, 8.76, 17.34, 8.32, 58.71, 194.92, 20.05, 168.03, 3.97, 2.78, 2.80, 1.65, 1.67, 2.69 };

  static double BandCalibration_White[64]=
  { 20.92, 56.64, 53.90, 45.14, 40.24, 50.82, 42.75, 37.52, 41.75, 43.59, 46.32, 47.56, 49.68, 52.93, 51.11, 49.68, 57.37, 58.12, 52.31, 50.53, 49.96, 46.32, 45.84, 48.33, 49.41, 46.56, 
    47.56, 49.96, 53.90, 51.40, 61.76, 56.64, 58.51, 70.02, 71.72, 61.76, 77.33, 8.35, 17.20, 10.62, 25.41, 23.40, 19.63, 20.63, 14.87, 12.07, 8.42, 5.57, 5.71, 5.42, 5.28, 4.31, 4.19, 3.56, 5.19, 6.12, 4.32, 4.21, 2.07, 1.49, 1.24, 1.00, 1.08, 2.01 };

   static double BandCalibration_Brown[64]=
   { 1.00, 2.71, 3.14, 3.83, 4.34, 4.98, 5.41, 6.29, 8.09, 9.41, 10.43, 12.42, 14.84, 15.60, 17.89, 20.46, 20.68, 24.28, 23.46, 27.31, 30.45, 33.95, 31.58, 34.10, 36.02, 39.55, 40.60, 
   48.56, 59.02, 67.90, 69.13, 71.71, 80.77, 91.35, 123.76, 134.61, 130.05, 18.44, 57.26, 62.38, 274.04, 426.28, 767.30, 1918.25, 306.92, 2557.67, 2557.67, 3836.50, 3000, 3000, 3836.50, 3000, 3000, 
   85.26, 3000, 3000, 99.65, 3000, 11.84, 13.46, 142.09, 2557.67, 7673.00, 2557.67 };

#endif
