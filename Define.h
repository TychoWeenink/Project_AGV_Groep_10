// Define.h
#ifndef DEFINE_H
#define DEFINE_H

#define START_knop    PK6 //pin A14
#define stop_knop     PK5 //pin A13
#define toestand_knop PK7 //pin A15
#define IR_L          PF0 //pin A0
#define IR_R          PF1 //pin A1
#define IR_V          PF2 //pin A2

//#define ultra_V_echo  PA0 //pin D22 Gr
//#define ultra_V_trigg PA2 //pin D24 Gl
//#define ultra_R_echo  PA1 //pin D23 Bl
//#define ultra_R_trigg PA3 //pin D25 Pr
//#define ultra_L_echo  PA5 //pin D27 Bl
//#define ultra_L_trigg PA4 //pin D26 Pr

#define Ultra_triggers PE3 //pin D5

#define Ultra_R_echo PK2 //pin A10
#define Ultra_L_echo PK1 //pin A9
#define Ultra_V_echo PK4 //pin A12

#define LM_switch     PK3  //pin A11
#define RM_switch     PK0  //pin A8
#define LED_L_GEEL    PL0  //pin D49
#define LED_R_GEEL    PL2  //pin D47
#define LED_TEST      PH6  //pin D9
#define LED_TEST2     PH5  //pin D8

//motoren

#define IN1           PB7  //pin D13 motorRP
#define IN2           PB6  //pin D12 motorRN
#define IN3           PB5  //pin D11 motorLP
#define IN4           PB4  //pin D10 motorLN
#define ENA           PH4  //pin D7 L
#define ENB           PH3  //pin D6 R

#define snelheidrechtdoor    50
#define snelheidhard    40
#define snelheidbijsturen   60
#define snelheiduit  99

// 60 rij snelheid

// 5 klik halve omwenteling wiel
// 10 klik 1 omwenteling wiel
//561 betekent te dichtbij


#endif // DEFINE_H
