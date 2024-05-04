/*
 * system.h
 *
 *  Created on: Jan 8, 2024
 *      Author: brian
 */

#ifndef INC_SYSTEM_H_
#define INC_SYSTEM_H_


//FUNCTION PROTOTYPES
short unsigned getSwitch(void);
void initGPIOStates(void);
short unsigned getMode(void);
void updateMode(long int mode);
void keepOnTest(void);
void keepOnState(short int keepon);
short unsigned turnAllOff(void);
void updateDisp(short int led_num, short int on_off);
void turnOffDisp(void);
void initDisp(void);
void recallDisp(void);
short unsigned battTest(void);
//void testSW1(void);



#endif /* INC_SYSTEM_H_ */
