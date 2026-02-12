#ifndef _MAIN__HPP
#define _MAIN__HPP


// #define TEST

#define conFigUSE_16_BIT_TICKS 0 // Events_bits 32
#define ADC_ATTEN ADC_ATTEN_11db // measurment up to
#define PAI 3.141592
// #define PIN1 5  // POT chip1 CS
// #define PIN2 27 // POT chip2 CS
#define Task_Pot1_bit (1 << 0)
#define Task_Pot2_bit (1 << 1)
#define Task_Pot3_bit (1 << 2)
#define Task_Pot4_bit (1 << 3)
#define Task_Pot5_bit (1 << 4)
#define Task_Pot6_bit (1 << 5)
#define Task_Pot7_bit (1 << 6)
#define Task_Pot8_bit (1 << 7)
#define READ (1 << 8)
#define OPMODE (1 << 9)
#define HALT (1 << 10)
#define REP_BIT (1 << 11)
#define DIR 12 //Ô­±¾ÊÇ14
#define X_PIN 32
#define Y_PIN 33
#define U_PIN 26
#define Z_PIN 25
#define OP_PIN 27 
#define IC_PIN 14
#define SBIT0 5
#define SBIT1 17
#define SBIT2 16
#define HC138 4

#endif