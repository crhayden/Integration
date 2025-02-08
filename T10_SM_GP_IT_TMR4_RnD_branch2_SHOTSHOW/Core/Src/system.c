/*
 * system.c
 *
 *  Created on: Jan 8, 2024
 *      Author: brian
 */

#include "system.h"
#include "main.h"


//SYSTEM VARIABLES
extern TIM_HandleTypeDef htim5;
typedef enum
{
	// LED Arrays are 1 indexed. 0 is unsued.

	// Orange LEDs on back of gun that represent shots remaining.
	SHOTCOUNTER_LED_B1 = 1,
	SHOTCOUNTER_LED_B2,
	SHOTCOUNTER_LED_B3,
	SHOTCOUNTER_LED_B4,
	SHOTCOUNTER_LED_B5,
	SHOTCOUNTER_LED_B6,
	SHOTCOUNTER_LED_B7,
	SHOTCOUNTER_LED_B8,
	SHOTCOUNTER_LED_B9,
	SHOTCOUNTER_LED_B10,

	// Status RGB
	SHOTCOUNTER_LED_BLUE,
	SHOTCOUNTER_LED_RED,
	SHOTCOUNTER_LED_GREEN,

	SHOTCOUNTER_NUMBER_OF_LEDS,
} SHOT_COUNTER_LEDS;

long int LED_STATE[SHOTCOUNTER_NUMBER_OF_LEDS] = {1,1,1,1,1,1,1,1,1,1,1,1,1,1};//LED0, LED1....LED10, BLUE,RED,GREEN

uint8_t randNums[10];
void _Shuffle(uint8_t *pArr) {
    for (int i = 0; i < 9; i++) {
        int j = i + rand() / (RAND_MAX / (10 - i) + 1);
        int t = pArr[j];
        pArr[j] = pArr[i];
        pArr[i] = t;
    }
}


//FUNCTION DEFINITIONS

short unsigned getSwitch(void)
{
	int SWA, SWB, SWC, SWD, SWSTATE;
	SWA = !HAL_GPIO_ReadPin(SWA_GPIO_Port, SWA_Pin);
	SWB = !HAL_GPIO_ReadPin(SWB_GPIO_Port, SWB_Pin);
	SWC = !HAL_GPIO_ReadPin(SWC_GPIO_Port, SWC_Pin);
	SWD = !HAL_GPIO_ReadPin(SWD_GPIO_Port, SWD_Pin);

	SWSTATE = (SWD*8)+(SWC*4)+(SWB*2)+(SWA*1);
	return SWSTATE;
}

void initGPIOStates(void)
{
	HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET);
	HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET);
	HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET);
	HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, SET);
	HAL_GPIO_WritePin(FLASH_GPIO_Port, FLASH_Pin, SET);
}

short unsigned getMode(void)
{
	int SW5, SW4, MODE_STATE;
	SW5 = HAL_GPIO_ReadPin(SW5_GPIO_Port, SW5_Pin);
	SW4 = HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin);

	MODE_STATE = (SW5*2)+(SW4*1);
	return MODE_STATE;

}

void updateMode(long int mode) 
{ 
	if(mode == 0) 
	{//SW4 & SW5 NO FLAG --- NORMAL MODE 
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, SET); 
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET); 
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET); 
		if (!startStealth || state[SFTD_STATE].next_state == CHARGE) {
			HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, SET);
			HAL_GPIO_WritePin(FLASH_GPIO_Port, FLASH_Pin, SET);
		}
		//__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,7000); 
	} 
	if(mode == 1) 
	{//SW4 FLAG SW5 NO FLAG -- STEALTH 
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET); 
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET); 
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET); 
		HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, RESET); 
		HAL_GPIO_WritePin(FLASH_GPIO_Port, FLASH_Pin, RESET); 
		HAL_GPIO_WritePin(KEEPON_GPIO_Port, KEEPON_Pin, SET);

		//__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,7000); 
	} 
	if(mode == 2) 
	{//SW5 FLAG, SW4 NO FLAG WARN_REENG, RED LED 
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET); 
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET); 
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, SET); 
		HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, SET); 
		HAL_GPIO_WritePin(FLASH_GPIO_Port, FLASH_Pin, SET); 
	} 
	if(mode == 3) 
	{//IN ARM STATE NO SWITCHES PRESSED 
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, SET); 
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET); 
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET); 
		HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, SET); 
		HAL_GPIO_WritePin(FLASH_GPIO_Port, FLASH_Pin, RESET); 
		//__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,1000); 
	} 
 
} 

void keepOnTest(void)
{
	if(HAL_GPIO_ReadPin(PWR_MON_GPIO_Port, PWR_MON_Pin))
	{
		HAL_GPIO_WritePin(KEEPON_GPIO_Port, KEEPON_Pin, SET);
	}
	else HAL_GPIO_WritePin(KEEPON_GPIO_Port, KEEPON_Pin, RESET);
}

short unsigned turnAllOff(void)
{
	HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET);
	HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET);
	HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET);
	HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, RESET);
	HAL_GPIO_WritePin(FLASH_GPIO_Port, FLASH_Pin, RESET);
	//__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,7100);
	HAL_Delay(1);

	//TURN OFF ALL DISPLAY LEDS...
	/*HAL_GPIO_WritePin(DISP_LED1_GPIO_Port, DISP_LED1_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED2_GPIO_Port, DISP_LED2_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED3_GPIO_Port, DISP_LED3_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED4_GPIO_Port, DISP_LED4_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED5_GPIO_Port, DISP_LED5_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED6_GPIO_Port, DISP_LED6_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED7_GPIO_Port, DISP_LED7_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED8_GPIO_Port, DISP_LED8_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED9_GPIO_Port, DISP_LED9_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED10_GPIO_Port, DISP_LED10_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED11_GPIO_Port, DISP_LED11_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED12_GPIO_Port, DISP_LED12_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED13_GPIO_Port, DISP_LED13_Pin, SET);*/
return 0;

}

void updateDisp(short int led_num, short int on_off)
{//use bitwise AND function to use mode, bit by bit to determine DISP LED states.
	/*B1,B2,B3,B4,B5,B6,B7,B8,B9,B10,B11,B12,B13,*/

#if CONFIG_SHOT_COUNTER_LED_RANDOM
	LED_STATE[randNums[led_num-1]] = on_off;
#else //default is defined(CONFIG_SHOT_COUNTER_LED_SEQUENTIAL)
	LED_STATE[(SHOTCOUNTER_LED_B10+1) - led_num] = on_off; 
#endif

	HAL_GPIO_WritePin(DISP_LED1_GPIO_Port, DISP_LED1_Pin, LED_STATE[SHOTCOUNTER_LED_B1]);//dart1
	HAL_GPIO_WritePin(DISP_LED2_GPIO_Port, DISP_LED2_Pin, LED_STATE[SHOTCOUNTER_LED_B2]);//dart2
	HAL_GPIO_WritePin(DISP_LED3_GPIO_Port, DISP_LED3_Pin, LED_STATE[SHOTCOUNTER_LED_B3]);
	HAL_GPIO_WritePin(DISP_LED4_GPIO_Port, DISP_LED4_Pin, LED_STATE[SHOTCOUNTER_LED_B4]);
	HAL_GPIO_WritePin(DISP_LED5_GPIO_Port, DISP_LED5_Pin, LED_STATE[SHOTCOUNTER_LED_B5]);
	HAL_GPIO_WritePin(DISP_LED6_GPIO_Port, DISP_LED6_Pin, LED_STATE[SHOTCOUNTER_LED_B6]);
	HAL_GPIO_WritePin(DISP_LED7_GPIO_Port, DISP_LED7_Pin, LED_STATE[SHOTCOUNTER_LED_B7]);
	HAL_GPIO_WritePin(DISP_LED8_GPIO_Port, DISP_LED8_Pin, LED_STATE[SHOTCOUNTER_LED_B8]);
	HAL_GPIO_WritePin(DISP_LED9_GPIO_Port, DISP_LED9_Pin, LED_STATE[SHOTCOUNTER_LED_B9]);
	HAL_GPIO_WritePin(DISP_LED10_GPIO_Port, DISP_LED10_Pin, LED_STATE[SHOTCOUNTER_LED_B10]);//dart10
}

void turnOffDisp(void)
{
	//TURN OFF ALL DISPLAY LEDS...
	HAL_GPIO_WritePin(DISP_LED1_GPIO_Port, DISP_LED1_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED2_GPIO_Port, DISP_LED2_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED3_GPIO_Port, DISP_LED3_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED4_GPIO_Port, DISP_LED4_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED5_GPIO_Port, DISP_LED5_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED6_GPIO_Port, DISP_LED6_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED7_GPIO_Port, DISP_LED7_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED8_GPIO_Port, DISP_LED8_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED9_GPIO_Port, DISP_LED9_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED10_GPIO_Port, DISP_LED10_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED11_GPIO_Port, DISP_LED11_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED12_GPIO_Port, DISP_LED12_Pin, SET);
	HAL_GPIO_WritePin(DISP_LED13_GPIO_Port, DISP_LED13_Pin, SET);

}

void initDisp(void)
{    
    // Initialize the array with numbers from 1 to 10
    for (int i = 0; i < 10; i++) {
        randNums[i] = i + 1;
    }
    // Shuffle the array
    _Shuffle(&randNums[0]);

	for (int i = 0; i<=12; i ++) {
		LED_STATE[i] = 0;
	}
	LED_STATE[SHOTCOUNTER_LED_GREEN] = 0;
	HAL_GPIO_WritePin(DISP_LED1_GPIO_Port, DISP_LED1_Pin, LED_STATE[SHOTCOUNTER_LED_B1]);//dart1
	HAL_GPIO_WritePin(DISP_LED2_GPIO_Port, DISP_LED2_Pin, LED_STATE[SHOTCOUNTER_LED_B2]);//dart2
	HAL_GPIO_WritePin(DISP_LED3_GPIO_Port, DISP_LED3_Pin, LED_STATE[SHOTCOUNTER_LED_B3]);
	HAL_GPIO_WritePin(DISP_LED4_GPIO_Port, DISP_LED4_Pin, LED_STATE[SHOTCOUNTER_LED_B4]);
	HAL_GPIO_WritePin(DISP_LED5_GPIO_Port, DISP_LED5_Pin, LED_STATE[SHOTCOUNTER_LED_B5]);
	HAL_GPIO_WritePin(DISP_LED6_GPIO_Port, DISP_LED6_Pin, LED_STATE[SHOTCOUNTER_LED_B6]);
	HAL_GPIO_WritePin(DISP_LED7_GPIO_Port, DISP_LED7_Pin, LED_STATE[SHOTCOUNTER_LED_B7]);
	HAL_GPIO_WritePin(DISP_LED8_GPIO_Port, DISP_LED8_Pin, LED_STATE[SHOTCOUNTER_LED_B8]);
	HAL_GPIO_WritePin(DISP_LED9_GPIO_Port, DISP_LED9_Pin, LED_STATE[SHOTCOUNTER_LED_B9]);
	HAL_GPIO_WritePin(DISP_LED10_GPIO_Port, DISP_LED10_Pin, LED_STATE[SHOTCOUNTER_LED_B10]);//dart10
	HAL_GPIO_WritePin(DISP_LED11_GPIO_Port, DISP_LED11_Pin, 1);//blu
	HAL_GPIO_WritePin(DISP_LED12_GPIO_Port, DISP_LED12_Pin, 1);//red
	HAL_GPIO_WritePin(DISP_LED13_GPIO_Port, DISP_LED13_Pin, 0);//grn
}

void recallDisp(void)
{
	HAL_GPIO_WritePin(DISP_LED1_GPIO_Port, DISP_LED1_Pin, LED_STATE[SHOTCOUNTER_LED_B1]);//dart1
	HAL_GPIO_WritePin(DISP_LED2_GPIO_Port, DISP_LED2_Pin, LED_STATE[SHOTCOUNTER_LED_B2]);//dart2
	HAL_GPIO_WritePin(DISP_LED3_GPIO_Port, DISP_LED3_Pin, LED_STATE[SHOTCOUNTER_LED_B3]);
	HAL_GPIO_WritePin(DISP_LED4_GPIO_Port, DISP_LED4_Pin, LED_STATE[SHOTCOUNTER_LED_B4]);
	HAL_GPIO_WritePin(DISP_LED5_GPIO_Port, DISP_LED5_Pin, LED_STATE[SHOTCOUNTER_LED_B5]);
	HAL_GPIO_WritePin(DISP_LED6_GPIO_Port, DISP_LED6_Pin, LED_STATE[SHOTCOUNTER_LED_B6]);
	HAL_GPIO_WritePin(DISP_LED7_GPIO_Port, DISP_LED7_Pin, LED_STATE[SHOTCOUNTER_LED_B7]);
	HAL_GPIO_WritePin(DISP_LED8_GPIO_Port, DISP_LED8_Pin, LED_STATE[SHOTCOUNTER_LED_B8]);
	HAL_GPIO_WritePin(DISP_LED9_GPIO_Port, DISP_LED9_Pin, LED_STATE[SHOTCOUNTER_LED_B9]);
	HAL_GPIO_WritePin(DISP_LED10_GPIO_Port, DISP_LED10_Pin, LED_STATE[SHOTCOUNTER_LED_B10]);//dart10
	HAL_GPIO_WritePin(DISP_LED11_GPIO_Port, DISP_LED11_Pin, LED_STATE[SHOTCOUNTER_LED_BLUE]);//blu
	HAL_GPIO_WritePin(DISP_LED12_GPIO_Port, DISP_LED12_Pin, LED_STATE[SHOTCOUNTER_LED_RED]);//red
	HAL_GPIO_WritePin(DISP_LED13_GPIO_Port, DISP_LED13_Pin, LED_STATE[SHOTCOUNTER_LED_GREEN]);//grn

}

void keepOnState(short int keepon)
{

	HAL_GPIO_WritePin(KEEPON_GPIO_Port, KEEPON_Pin, keepon);
}

short unsigned battTest(void)
{
	return 0;
}
/*
void testSW1(void)
{
	short int mode = getSwitch();

	if(mode == 0)
	{
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, SET);
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET);
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET);
	}
	if(mode == 1)
	{
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET);
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, SET);
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET);
	}
	if(mode == 2)
	{
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET);
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET);
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, SET);
	}
	if(mode == 3)
	{
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET);
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, SET);
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, SET);
	}
}*/
