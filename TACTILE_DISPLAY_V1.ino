//#include <Arduino_FreeRTOS.h>
//#include <semphr.h>

#include <WEBER_TACTILE_DISPLAY.h>

#include <Wire.h> //Include arduino Wire Library to enable to I2C
#include "WEBER_TACTILE_DISPLAY.h"

// waveform values and positions for a 6x6 matrix, uncomment if using the waveform for piezos
/*

const int waveformValues[6][6] = {
  {1,4,1,4,1,4},
  {2,5,2,5,2,5},
  {3,6,3,6,3,6},
  {1,4,1,4,1,4},
  {2,5,2,5,2,5},
  {3,6,3,6,3,6}
};

const int waveformPositions[6][6] = {
  {1,1,2,2,3,3},
  {1,1,2,2,3,3},
  {1,1,2,2,3,3},
  {4,4,5,5,6,6},
  {4,4,5,5,6,6},
  {4,4,5,5,6,6}
};

*/

/*
DESIGN PLAN:
- add basic functions and connection to scan serial port
- connect with functions to trigger a certain piezo to activate
- process string
*/



WEBER_TACTILE_DISPLAY TD; //Assign object for C++ class

unsigned long timestart,timeend,loadtime,playtime = 0; //used to store time

void setup() 
{

  delay(10);
  Serial.begin(9600);
  TD.begin(); 

  //TD.PLAY_MARIO();
  delay(20);
  TD.TEST_TCA0();
  //TD.i2cSCANNER();
  TD.TCA_SCANNER();
}

void loop() 
{  
  Serial.println("hi");
 // delay(1000);    // Delay allows waveform to play, and gives the user time to recognize the display
 // TD.TEST_TCA0(); // Test TCA1 - Outputs waveform onto 6 ports (0-5)
 // delay(1000);    // Delay allows waveform to play, and gives the user time to recognize the display



//  delay(1000);        // First delay gives user extra time to recognize the display from last repeated (main) loop
  TD.PLAY_A_proto(); // Play Braille Char. "A"- Outputs waveform onto designated Tactile Display ports

    //timestart = micros();
   // TD.LOAD_MAX36();
   // TD.PLAY_MAX36();

   // timeend = micros();
   // loadtime = timeend - timestart;
    //delay(100);
  //TD.TEST_TCA0();
  delay(3000);
  
   //TD.PLAY_C_proto();
  //TD.READ_WORD();
  //TD.PLAY_WORD();
/*
    timestart = micros();
    TD.PLAY_MAX36();
    timeend = micros();
    playtime = timeend - timestart;

    Serial.print("load time = ");
    Serial.println(loadtime);
    Serial.print("Play time = ");
    Serial.println(playtime);
    Serial.print("Total time = ");
    Serial.println(loadtime + playtime);
    Serial.print("Refresh rate for both load and play = ");
    Serial.println(1/(loadtime+playtime));
    */
    // delay(3000); // Delay allows waveform to play, and gives the user time to recognize the display
 
      

}
