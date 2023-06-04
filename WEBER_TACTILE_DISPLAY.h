/*

  WEBER_TACTILE_DISPLAY.h - Library for the Tactle Display - Senior Project
  Created by: Orlando Salas, Suny Ly, and Jacolby Griffin. April 2018
  Organization: Weber State University - Department of Electrical and Computer Engineering - Class of 2018

  Texas Instrument - DRV2667 Arduino Script Derived from Yuri Klenaov, January, 2016. (yurikleb.com)
  Texas Instrument - TCA9548A Arduino Script Derived from Tod E. Kurt  2009 (todbot.com/blog) and Lady Adra (adafruit.com/users/adafruit2)                       
                                                                            
*/

#ifndef WEBER_TACTILE_DISPLAY_h
#define WEBER_TACTILE_DISPLAY_h


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#include <Wire.h> //Wire Library to use I2C
#include <string.h> 


#define DRV2667_ADDR 0x59 //The DRV2667 Chip default I2C address.

#define TCAADDR0 0x70
#define TCAADDR1 0x71
#define TCAADDR2 0x72
#define TCAADDR3 0x73
#define TCAADDR4 0x74
#define TCAADDR5 0x75


class WEBER_TACTILE_DISPLAY
{
  public:
    WEBER_TACTILE_DISPLAY(void);
    void begin(void);

	void LOAD_WAVE(byte WaveForm[][4], byte WavesNumber);
	void writeRegisterBytes(byte reg, byte val);
	void i2c_Scan(void);

  void TCA_0(uint8_t i);
  void TCA_1(uint8_t i);
  void TCA_2(uint8_t i);
  void TCA_3(uint8_t i);
  void TCA_4(uint8_t i);
  void TCA_5(uint8_t i);
  void TCA_6(uint8_t i);
  void TCA_7(uint8_t i);

  void i2cSCANNER(void);
  void PLAY_A_proto(void);
  void PLAY_D_proto(void);
  void PLAY_C_proto(void);
  void PLAY_T_proto(void);
  void PLAY_MAX36(void);
  void PLAY_CHAR(char c, int pos);
  void PLAY_WORD(char* word);

  void ALLMUX_p1(void);
  void TEST_TCA0(void);
  void TEST_TCA1(void);

  void PLAY_MARIO(void);
  void PLAY_SIDE2SIDE(void);
  
  void TCA_SCANNER(void);

  void TCA_and_PORT(uint8_t m, uint8_t p);
  void LOAD_MAX36(void);
  void TCA_OFF(uint8_t m);

  private:

};

#endif
