/* 

  WEBER_TACTILE_DISPLAY.h - Library for the Tactle Display - Senior Project
  Created by: Orlando Salas, Suny Ly, and Jacolby Griffin. April 2018
  Organization: Weber State University - Department of Electrical and Computer Engineering - Class of 2018

  Texas Instrument - DRV2667 Arduino Script Derived from Yuri Klenaov, January, 2016. (yurikleb.com)
  Texas Instrument - TCA9548A Arduino Script Derived from Tod E. Kurt  2009 (todbot.com/blog) and Lady Adra (adafruit.com/users/adafruit2)                       
                                                                            
*/

#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif
#include <Wire.h> //Wire Library to use I2C
//#include <string>
extern "C" { 
#include "utility/twi.h"  // from Wire library, so we can do bus scanning
}

#include "WEBER_TACTILE_DISPLAY.h"

byte error = 0;

byte WaveForm_MAIN[1][4] = 
{ 
          
    // WaveForm Array: [Amplitude, Freq, Cycles, Envelope] 
    // Amplitude    --  min:0=50v max: 255=100v 
    // Frequerncy   --  0-255 or 0x00-0xFF
    // Duration     --  Cycles 0-255
    // Envelope     --  (Ramp up + down)
    // Max 60 waves per array !!
   {255, 10, 100, 0}  // {AMP., FREQ., DUR., ENV.} = { [A] , [F] , [D] , [E - "Optimum at 0"] }  
                          
};



WEBER_TACTILE_DISPLAY::WEBER_TACTILE_DISPLAY()
{
  // required for C++ classes. Creates constructor for class
}



// ------------- Yurikleb's wtd2665 init. and i2c Scan Procedure ----------------------
// Description: Scans for i2c devices on data bus
void WEBER_TACTILE_DISPLAY::begin() 
{
  Wire.begin(); // initialize i2c CLK and DATA bus
  Wire.setClock(400000);
  Serial.begin (9600);
  // 2c_Scan(); // Scan for i2c devices to make sure we have the wtd2667 attached;
  delay(300);
}


// ------------------------------- Yurikleb's wtd2665 Initialization Procedure ----------------------
// Description: Takes piezo driver out of standby, and runs an initialization setup
//  Note: Setting the GO bit has been disabled for our use. Therefore, it is ONLY loading Waveform (not playing)
//        The GO bit is written/played in custom "play" functions.
void WEBER_TACTILE_DISPLAY::LOAD_WAVE(byte WaveForm[][4], byte WavesNumber) // 
{
          // Uncomment below to display waveform characteristics on serial monitor
    //  Serial.println("Playing WaveForm:");
    //  Serial.print("Containing: "); 
    //  Serial.print(WavesNumber / 4); 
    //  Serial.println(" waves:");
    //  for (int j=0; j < WavesNumber/4; j++){
    //    Serial.print("Ampl: "); 
    //    Serial.print(WaveForm[j][0]);  
    //    Serial.print("  |  Freq: "); 
    //    Serial.print(WaveForm[j][1]);    
    //    Serial.print("  |  Duration: "); 
    //    Serial.print(1000 * (WaveForm[j][2] / (7.8125 * WaveForm[j][1]))); 
    //    Serial.print("ms");  
    //    Serial.print("  |  Env: 0x"); 
    //    Serial.println(WaveForm[j][3], HEX);
    //  }
    //  Serial.println("");

    
  //control ---------------------------------------
  //desc: Adjust register values to change waveform characteristics
  writeRegisterBytes(0x02, 0x00); //Take device out of standby mode
  writeRegisterBytes(0x01, 0x03); //Set Gain 0-3 (0x00-0x03 25v-100v) [0x02 = 75V GAIN]
  writeRegisterBytes(0x03, 0x01); //Set sequencer to play WaveForm ID #1
  writeRegisterBytes(0x04, 0x00); //End of sequence
  
  //header ------------------------------------------
  
  //desc: The follwing
  writeRegisterBytes(0xFF, 0x01); //Set memory to page 1
  writeRegisterBytes(0x00, 0x05); //Header size –1
  writeRegisterBytes(0x01, 0x80); //Start address upper byte (page), also indicates Mode 3
  writeRegisterBytes(0x02, 0x06); //Start address lower byte (in page address)
  writeRegisterBytes(0x03, 0x00); //Stop address upper byte
  writeRegisterBytes(0x04, 0x06+WavesNumber-1); //Stop address Lower byte
  writeRegisterBytes(0x05, 0x01); //Repeat count, play WaveForm once
  
  //WaveForm Data From the array
  for(byte i = 0; i < WavesNumber; i++){
    writeRegisterBytes(0x06+i*4+0, WaveForm[i][0]); 
    writeRegisterBytes(0x06+i*4+1, WaveForm[i][1]); 
    writeRegisterBytes(0x06+i*4+2, WaveForm[i][2]); 
    writeRegisterBytes(0x06+i*4+3, WaveForm[i][3]);
  }
  
  //Control ----------------------------------------
  
  writeRegisterBytes(0xFF, 0x00); //Set page register to control space
  //writeRegisterBytes(0x02, 0x01); //Set GO bit (execute WaveForm sequence)ss [moved to main code]
                                    //  Because we will control multiple wtd2667s, it was is
                                    //  not good to execute (or play) waveform at this point 
  
  //delay( 1000 * (cycles / (7.8125 * frequency)) ); //keep delay in mind for waveform design
                                                     //add delay into main loop, or into seperate
                                                     //"character/shape" function to be created

}



//Write Bytes via I2c register and gives DATA to register (Using Wire Library)
void WEBER_TACTILE_DISPLAY::writeRegisterBytes(byte reg, byte val) 
{
    Wire.beginTransmission(DRV2667_ADDR);
    Wire.write((byte)reg);
    Wire.write((byte)val);
    Wire.endTransmission();
}




// ------------------ Yurikleb's i2c Scanner  ----------------------
// Description: Scans for i2c devices on data bus, and reports results
void WEBER_TACTILE_DISPLAY::i2c_Scan()
{
    // Leonardo: wait for serial port to connect
    while (!Serial)
    {
    }
  
    Serial.println ();
    Serial.println ("I2C scanner. Scanning ...");
    byte count = 0;
  
    Wire.begin();
    for (byte i = 8; i < 120; i++)
    {
      Wire.beginTransmission (i);
      if (Wire.endTransmission () == 0)
      {
        Serial.print ("Found address: ");
        Serial.print (i, DEC);
        Serial.print (" (0x");
        Serial.print (i, HEX);
        Serial.println (")");
        count++;
        delay (1);
      }
    }
    Serial.println ("Done.");
    Serial.print ("Found ");
    Serial.print (count, DEC);
    Serial.println (" device(s).");
    Serial.println ("***********");  
    Serial.println (" ");
    
}

// --------------------------- TI - TCA9548A functions -------------------------------------------------
// Description: Uses Wire.h to communicate with TI's TCA9548A I2C switch
//              Each MUX will have its own address and function
//              Code was derived from Adrafruit's Lady Adra website
void WEBER_TACTILE_DISPLAY::TCA_0(uint8_t i) // MUX0 w/ port-select (enter 0 - 7 here for port on MUX)
{
  if (i > 7) return;
  Wire.beginTransmission(TCAADDR0); // Writes to TCA0 Address (note: User is to designate TCA i2c address by hardwire)
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void WEBER_TACTILE_DISPLAY::TCA_1(uint8_t i) // MUX0 w/ port-select (enter 0 - 7 here for port on MUX)
{
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR1); // Writes to TCA1 Address (note: User is to designate TCA i2c address by hardwire)
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void WEBER_TACTILE_DISPLAY::TCA_2(uint8_t i) // MUX0 w/ port-select (enter 0 - 7 here for port on MUX)
{
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR2); // Writes to TCA2 Address (note: User is to designate TCA i2c address by hardwire)
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void WEBER_TACTILE_DISPLAY::TCA_3(uint8_t i) // MUX0 w/ port-select (enter 0 - 7 here for port on MUX)
{
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR3); // Writes to TCA3 Address (note: User is to designate TCA i2c address by hardwire)
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void WEBER_TACTILE_DISPLAY::TCA_4(uint8_t i) // MUX0 w/ port-select (enter 0 - 7 here for port on MUX)
{
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR4); // Writes to TCA4 Address (note: User is to designate TCA i2c address by hardwire)
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void WEBER_TACTILE_DISPLAY::TCA_5(uint8_t i) // MUX0 w/ port-select (enter 0 - 7 here for port on MUX)
{
  if (i > 7) return;
 
  Wire.beginTransmission(TCAADDR5); // Writes to TCA5 Address (note: User is to designate TCA i2c address by hardwire)
  Wire.write(1 << i);
  Wire.endTransmission();  
}

void WEBER_TACTILE_DISPLAY::TCA_and_PORT(uint8_t m, uint8_t p) // selects mux and port (enter 0 - 7 here for port on MUX)
{
  
  if (p > 7) return;
 
  Wire.beginTransmission(TCAADDR0+m); // Writes to TCA(0 + M) Address and also writes to port (P) (note: User is to designate TCA i2c address by hardwire)
  
 
  Wire.write(1 << p);
  error=Wire.endTransmission(); 
     
}



void WEBER_TACTILE_DISPLAY::TCA_OFF(uint8_t m)  //zero's out mux register
                                        //Important function if you are using multiple TCAs for a full 6x6 wtd array
{
  Wire.beginTransmission(TCAADDR0+m);
  
 
  Wire.write(0x00); // Writes to TCA(M) Address and writes 00 to tell TCA to ignore all following commands
                    // until it is initiated once again.
  error=Wire.endTransmission(); 
}

// ---- end ALL MUX/i2cSwitch Functions ----






// --------------------------- LOAD MAX 36 function ---------------------------
// Description: LOAD MAX 36 function will load waveform into all 36 wtd drivers.
//              All 36 wtds are individually loaded

void WEBER_TACTILE_DISPLAY::LOAD_MAX36(void)
{

  // --------------      SETUP (LOAD WAVEFORM into RAM)   ----------------

  for (uint8_t m=0; m<6; m++) {
         
          
          for (uint8_t p=0; p<6; p++) {
              TCA_and_PORT(m, p); // Loop designed to loop and load WF_MAIN into all 36 DRVS
              LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));  //load each DRV
              
          }
          TCA_OFF(m); // Tells TCA(M) to stop listening to I2C bus to avoid interference & loss of communication
  }

}


// --------------------------- PLAY MAX 36 function ---------------------------
// Description: After all 36 wtds are individually loaded, we THEN play all wtds simultaneously
//              (or in a rapid consecutive execution) to minimize delay between each actuator.
void WEBER_TACTILE_DISPLAY::PLAY_MAX36(void){

      // ------   EXECUTE (Write to "GO"/play register on wtd)  ------
    for (uint8_t m=0; m<6; m++) // Loop designed to loop and PLAY stored WF_MAIN for all 36 DRVS
    {
           
            
            for (uint8_t p=0; p<6; p++) {
                TCA_and_PORT(m, p);
                writeRegisterBytes(0xFF, 0x00); //Set page register to control space
                writeRegisterBytes(0x02, 0x01); //play each wtd
                
            }
            TCA_OFF(m); // Designed to minimize i2c databus interference
    }
     
}





// --------------------------- i2cSCANNER functions -------------------------------------------------
// Description: Uses Wire.h to communicate with PRIMARY i2c BUS
//              The purpose of this function is to scan for all immediate i2c switches/demuxes
//              It will display all individual i2c switches with unique addresses
void WEBER_TACTILE_DISPLAY::i2cSCANNER(void)
{

    Serial.println("\nI2C Scanner");
    byte error, address;
    int nDevices;
  
    Serial.println("Scanning...");
  
    nDevices = 0;
    for(address = 1; address < 127; address++ ) 
    {
      // The i2c_scanner uses the return value of
      // the Write.endTransmisstion to see if
      // a device did acknowledge to the address.
      Wire.beginTransmission(address);
      error = Wire.endTransmission();
  
      if (error == 0)
      {
        Serial.print("I2C device found at address 0x");
        if (address<16) 
          Serial.print("0");
        Serial.print(address,HEX);
        Serial.println("  !");
  
        nDevices++;
      }
      else if (error==4) 
      {
        Serial.print("Unknown error at address 0x");
        if (address<16) 
          Serial.print("0");
        Serial.println(address,HEX);
      }    
    }
    if (nDevices == 0)
      Serial.println("No I2C devices found\n");
    else
      Serial.println("done\n");
  
    delay(5000);           // wait 5 seconds for next scan

}

// --------------------------- TCA_SCANNER functions -------------------------------------------------
// Description: Uses Wire.h to communicate with PRIMARY i2c BUS
//              The purpose of this function is to scan for all immediate i2c switches/demuxes
//              It will display all individual i2c switches with unique addresses.
//              Code derived from Tod E. Kurt. 2009
void WEBER_TACTILE_DISPLAY::TCA_SCANNER(void)
{
    for (uint8_t m=0; m<6; m++) {
        Serial.print("TCAScanner ready for Mux#");
        Serial.println(m);
        
        for (uint8_t p=0; p<8; p++) {
          //TCA_0(t);
          TCA_and_PORT(m, p);
          Serial.print("TCA Port #"); Serial.println(p);
    
          for (uint8_t addr = 0; addr<=127; addr++) {
            if (addr == TCAADDR0) continue;
            if (addr == TCAADDR1) continue;
            if (addr == TCAADDR2) continue;
            if (addr == TCAADDR3) continue;
            if (addr == TCAADDR4) continue;
            if (addr == TCAADDR5) continue;
          
            uint8_t data;
            if (! twi_writeTo(addr, &data, 0, 1, 1)) {
               Serial.print("Found I2C 0x");  Serial.println(addr,HEX);
            }
          }
        }
        TCA_OFF(m);
        Serial.println("\ndone");
    }
}



void WEBER_TACTILE_DISPLAY::ALLMUX_p1(void)
{


  // --------------      SETUP (LOAD WAVEFORM into RAM)   ----------------
  
  // Array 0

      TCA_0(1); //Select port 1 on TCA0
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); //Writing/storing Waveform to DRV2667


  // Array 1

      TCA_1(1); //Select port 1 on TCA1
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); //Writing/storing Waveform to DRV2667


  // Array 2

      TCA_2(1); //Select port 1 on TCA2
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); //Writing/storing Waveform to DRV2667


  // Array 3

      TCA_3(1); //Select port 1 on TCA3
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); //Writing/storing Waveform to DRV2667


  // Array 4

      TCA_4(1); //Select port 1 on TCA4
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); //Writing/storing Waveform to DRV2667

  // Array 5

      TCA_5(1); //Select port 1 on TCA5
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); //Writing/storing Waveform to DRV2667

 
  // ------   EXECUTE (Write to "GO"/play register on wtd)  ------
  
  // Array 0

      TCA_0(1); //Select port 1 on TCA0
      writeRegisterBytes(0x02, 0x01); // Writing GO bit to GO register on DRV2667


  // Array 1

      TCA_1(1); //Select port 1 on TCA1
      writeRegisterBytes(0x02, 0x01); // Writing GO bit to GO register on DRV2667


  // Array 2

      TCA_2(1); //Select port 1 on TCA2
      writeRegisterBytes(0x02, 0x01); // Writing GO bit to GO register on DRV2667


  // Array 3

      TCA_3(1); //Select port 1 on TCA3
      writeRegisterBytes(0x02, 0x01); // Writing GO bit to GO register on DRV2667


  // Array 4

      TCA_4(1); //Select port 1 on TCA4
      writeRegisterBytes(0x02, 0x01); // Writing GO bit to GO register on DRV2667


  // Array 5

      TCA_5(1); //Select port 1 on TCA5
      writeRegisterBytes(0x02, 0x01); // Writing GO bit to GO register on DRV2667

  
}



// -------------- Test TCA0    ------------
// Description: LOAD --  Setup procedure + LOAD waveform onto TCA0 ports 0-7
//              EXECUTE --  PLAY written waveform by writing GO bit to GO register on DRV2667      
void WEBER_TACTILE_DISPLAY::TEST_TCA0(void)
{


  //LOAD WAVEFORM
  TCA_0(0); 
  LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
  TCA_0(1); 
  LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
  TCA_0(2); 
  LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
  TCA_0(3); 
  LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
  TCA_0(4); 
  LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
  TCA_0(5); 
  LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
  
  //EXCECUTE
  TCA_0(0); 
  writeRegisterBytes(0x02, 0x01);
  TCA_0(1); 
  writeRegisterBytes(0x02, 0x01);
  TCA_0(2); 
  writeRegisterBytes(0x02, 0x01);
  TCA_0(3); 
  writeRegisterBytes(0x02, 0x01);
  TCA_0(4); 
  writeRegisterBytes(0x02, 0x01);
  TCA_0(5); 
  writeRegisterBytes(0x02, 0x01);
}




// -------------- Test TCA1    ------------
// Description: LOAD --  Setup procedure + LOAD waveform onto TCA0 ports 0-7
//              EXECUTE --  PLAY written waveform by writing GO bit to GO register on DRV2667  
void WEBER_TACTILE_DISPLAY::TEST_TCA1(void)
{


    //LOAD WAVEFORM
    TCA_1(0); 
    LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
    TCA_1(1); 
    LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
    TCA_1(2); 
    LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
    TCA_1(3); 
    LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
    TCA_1(4); 
    LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
    TCA_1(5); 
    LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
    
    //EXCECUTE
    TCA_1(0); 
    writeRegisterBytes(0x02, 0x01);
    TCA_1(1); 
    writeRegisterBytes(0x02, 0x01);
    TCA_1(2); 
    writeRegisterBytes(0x02, 0x01);
    TCA_1(3); 
    writeRegisterBytes(0x02, 0x01);
    TCA_1(4); 
    writeRegisterBytes(0x02, 0x01);
    TCA_1(5); 
    writeRegisterBytes(0x02, 0x01);
}



// -------------- PLAY&LOAD Braille Char. "A" ------------
// Description: Load -- Setup procedure + Load waveform
//              Play -- Write to and PLAY written waveform
// NOTE!!: This character was created on the hand-soldered
//         prototype circuit, and will not output the correct pattern
//         on the final prototype
void WEBER_TACTILE_DISPLAY::PLAY_A_proto(void)
{
      //LOAD Waveform into TCA0 port 0
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
    
      //PLAY Play Stored Waveform
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01); // GO bit to control reg.
      
}

// -------------- Play Mario Soundbit  ------------
// Desc: After testing the Piezos with the wtd driver, we noticed that Piezos emit
//         sounds at high frequencies. We tuned multiple waveforms for proper sound frequencies.
//         The follwing function will play resonate piezos found on TCA0 ports: 2, 4, and 5.
void WEBER_TACTILE_DISPLAY::PLAY_MARIO(void)
{
   //Creating 4 waveforms containing 4 specific frequencies for the Mario Bros. Soundbit.
   byte Wave660_MAIN[1][4] = {  //660hz
                          {255, 85, 66, 0}  // {AMP., FREQ., DUR., ENV.} = { [A] , [F] , [D] , Optimum at 0}                     
                          };
   byte Wave510_MAIN[1][4] = { 
                          {255,66, 51, 0}  // {AMP., FREQ., DUR., ENV.} = { [A] , [F] , [D] , Optimum at 0}                     
                          };
   byte Wave770_MAIN[1][4] = { 
                          {255, 100, 78, 0}  // {AMP., FREQ., DUR., ENV.} = { [A] , [F] , [D] , Optimum at 0}                     
                          };
   byte Wave380_MAIN[1][4] = { 
                          {255, 49, 38, 0}  // {AMP., FREQ., DUR., ENV.} = { [A] , [F] , [D] , Optimum at 0}                     
                          };


    ///////load startup music to these pins//////////////
   TCA_0(1); 
   LOAD_WAVE(Wave660_MAIN, sizeof(Wave660_MAIN)); 

   TCA_0(2); 
   LOAD_WAVE(Wave510_MAIN, sizeof(Wave510_MAIN)); 

   TCA_0(4); 
   LOAD_WAVE(Wave770_MAIN, sizeof(Wave770_MAIN)); 

   TCA_0(5); 
   LOAD_WAVE(Wave380_MAIN, sizeof(Wave380_MAIN)); 

  //////play quick tune/////////////////////////
  
   TCA_0(1); 
   writeRegisterBytes(0x02, 0x01);
   delay(150);
   TCA_0(1); 
   writeRegisterBytes(0x02, 0x01);
   delay(300);
   TCA_0(1); 
   writeRegisterBytes(0x02, 0x01);
   delay(300);
   TCA_0(2); 
   writeRegisterBytes(0x02, 0x01);
   delay(100);
   TCA_0(1); 
   writeRegisterBytes(0x02, 0x01);
   delay(300);
   TCA_0(4); 
   writeRegisterBytes(0x02, 0x01);
   delay(550);
   TCA_0(5); 
   writeRegisterBytes(0x02, 0x01);
   delay(575);
   TCA_OFF(0);
}


// -------------- Play Side to Side  ------------
// Desc: The test was designed for our initial prototype that had 6 working piezo pins.
//       The function will play 6 working piezos that are connected to TCA0 on ports 0-5.
//       The unique sequence will alternate piezo actuation between TCA0's ports 0-2 and 3-5
void WEBER_TACTILE_DISPLAY::PLAY_SIDE2SIDE(void)
{

    // --------SETUP
    TCA_0(0); 
    LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
    TCA_0(1); 
    LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
    TCA_0(2); 
    LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
    TCA_0(3); 
    LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
    TCA_0(4); 
    LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
    TCA_0(5); 
    LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 


    // --------EXECUTE
    delay(1000);
    TCA_0(0); 
    writeRegisterBytes(0x02, 0x01);
    TCA_0(1); 
    writeRegisterBytes(0x02, 0x01);
    TCA_0(2); 
    writeRegisterBytes(0x02, 0x01);

    delay(1500);
    TCA_0(3); 
    writeRegisterBytes(0x02, 0x01);
    TCA_0(4); 
    writeRegisterBytes(0x02, 0x01);
    TCA_0(5); 
    writeRegisterBytes(0x02, 0x01);


}

void WEBER_TACTILE_DISPLAY::PLAY_CHAR(char c, int pos) 
{
  switch (c) {
    case 'A':
      PLAY_A_proto();
      Serial.println("A");
      break;
    case 'B':
      PLAY_B_proto();
      Serial.println("B");
      break;
    case 'C':
      PLAY_C_proto();
      Serial.println("C");
      break;
    case 'D':
      PLAY_D_proto();
      Serial.println("D");
      break;
    case 'E':
      break;
    case 'F':
      break;
    case 'G':
      break;
    case 'H':
      break;
    case 'I':
      break;
    case 'J':
      break;
    case 'K':
      break;
    case 'M':
      break;
    case 'N':
      break;
    case 'L':
      break;
    case 'O':
      break;
    case 'P':
      break;
    case 'Q':
      break;
    case 'R':
      break;
    case 'S':
      break;
    case 'T':
      PLAY_T_proto();
      Serial.println("T");
      break;
    case 'U':
      break;
    case 'V':
      break;
    case 'W':
      break;
    case 'X':
      break;
    case 'Y':
      break;
    case 'Z':
      break;
    default:
      Serial.println("Letter not found");
  }
}

void WEBER_TACTILE_DISPLAY::PLAY_WORD(char* word) 
{
  int length = strlen(word);
  for (int i = 0; i < length; i++) {
    PLAY_CHAR(word[i], 0);
    delay(4000);
  }
}

void WEBER_TACTILE_DISPLAY::PLAY_C_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);

}
// -------------- PLAY&LOAD Braille Char. "D" ------------
// Description: Load -- Setup procedure + Load waveform
//              Play -- Write to and PLAY written waveform
// NOTE!!: This character was created on the hand-soldered
//         prototype circuit, and will not output the correct pattern
//         on the final prototype

void WEBER_TACTILE_DISPLAY::PLAY_D_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_1(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(1); 
      writeRegisterBytes(0x02, 0x01);

}

//code for playing the letter E on the first segement of display
void WEBER_TACTILE_DISPLAY::PLAY_E_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_1(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(1); 
      writeRegisterBytes(0x02, 0x01);

}

void WEBER_TACTILE_DISPLAY::PLAY_F_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_0(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);
}

//plays letter G for the first segment
void WEBER_TACTILE_DISPLAY::PLAY_G_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_0(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(1); 
      writeRegisterBytes(0x02, 0x01);
}

// plays letter H 
void WEBER_TACTILE_DISPLAY::PLAY_H_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_0(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(1); 
      writeRegisterBytes(0x02, 0x01);
}
//plays I in first segemtn
void WEBER_TACTILE_DISPLAY::PLAY_I_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);
}

//plays the letter J on the first segment
void WEBER_TACTILE_DISPLAY::PLAY_J_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(1); 
      writeRegisterBytes(0x02, 0x01);
}

void WEBER_TACTILE_DISPLAY::PLAY_K_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
}

void WEBER_TACTILE_DISPLAY::PLAY_L_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_0(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
}

void WEBER_TACTILE_DISPLAY::PLAY_M_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);
}


void WEBER_TACTILE_DISPLAY::PLAY_N_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(1); 
      writeRegisterBytes(0x02, 0x01);
}

void WEBER_TACTILE_DISPLAY::PLAY_O_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(1); 
      writeRegisterBytes(0x02, 0x01);
}

void WEBER_TACTILE_DISPLAY::PLAY_P_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_0(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);
}

void WEBER_TACTILE_DISPLAY::PLAY_Q_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_0(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(1); 
      writeRegisterBytes(0x02, 0x01);
}

void WEBER_TACTILE_DISPLAY::PLAY_R_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_0(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(1); 
      writeRegisterBytes(0x02, 0x01);
}

void WEBER_TACTILE_DISPLAY::PLAY_S_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);
}

void WEBER_TACTILE_DISPLAY::PLAY_T_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_1(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(1); 
      writeRegisterBytes(0x02, 0x01);
}

void WEBER_TACTILE_DISPLAY::PLAY_U_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_1(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(2); 
      writeRegisterBytes(0x02, 0x01);
}

void WEBER_TACTILE_DISPLAY::PLAY_V_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_0(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_1(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(2); 
      writeRegisterBytes(0x02, 0x01);
}


void WEBER_TACTILE_DISPLAY::PLAY_W_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_1(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_1(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(2); 
      writeRegisterBytes(0x02, 0x01);
}


void WEBER_TACTILE_DISPLAY::PLAY_X_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_1(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_1(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(2); 
      writeRegisterBytes(0x02, 0x01);
}

void WEBER_TACTILE_DISPLAY::PLAY_Y_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_1(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_1(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(2); 
      writeRegisterBytes(0x02, 0x01);
}


void WEBER_TACTILE_DISPLAY::PLAY_Z_proto(void) {
  ////LOAD Waveform into TCA0 port 0,1, and 2
      TCA_0(0); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_0(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 
      TCA_1(1); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN));
      TCA_1(2); 
      LOAD_WAVE(WaveForm_MAIN, sizeof(WaveForm_MAIN)); 

      //PLAY Stored Stored Waveform on same loaded DRVs
      TCA_0(0); 
      writeRegisterBytes(0x02, 0x01);
      TCA_0(2); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(1); 
      writeRegisterBytes(0x02, 0x01);
      TCA_1(2); 
      writeRegisterBytes(0x02, 0x01);
}
