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
#include<string.h>
//#include <Arduino_FreeRTOS.h>
//#include <semaphr.h>
#include <Wire.h> //Wire Library to use I2C


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
  void PLAY_B_proto(void);
  void PLAY_C_proto(void);
  void PLAY_D_proto(void);
  void PLAY_E_proto(void);
  void PLAY_F_proto(void);
  void PLAY_G_proto(void);
  void PLAY_H_proto(void);
  void PLAY_I_proto(void);
  void PLAY_J_proto(void);
  void PLAY_K_proto(void);
  void PLAY_M_proto(void);
  void PLAY_N_proto(void);
  void PLAY_L_proto(void);
  void PLAY_O_proto(void);
  void PLAY_P_proto(void);
  void PLAY_Q_proto(void);
  void PLAY_R_proto(void);
  void PLAY_S_proto(void);
  void PLAY_T_proto(void);
  void PLAY_U_proto(void);
  void PLAY_V_proto(void);
  void PLAY_W_proto(void);
  void PLAY_X_proto(void);
  void PLAY_Y_proto(void);
  void PLAY_Z_proto(void);
  void PLAY_NUMF_proto(void);
  void PLAY_period_proto(void);
  void PLAY_comma_proto(void);
  void PLAY_Qmark_proto(void);
  void PLAY_Exalmark_proto(void);
  void PLAY_colon_proto(void);
  void PLAY_Semicolon_proto(void);
  void PLAY_dash_proto(void);
  void PLAY_CAPF_proto(void);
  void PLAY_APO_proto(void);
  void PLAY_MAX36(void);
  void PLAY_CHAR(char c, int p);
  void POSITION(void);
  void READ_WORD();
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

