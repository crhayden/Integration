/*******************************************************************************
 * File Name      : SFTD_states.c
 * Description    : routines to handle gun function
 *******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "timing.h"
#include "states.h"
#include "main.h"
#include "system.h"

/* Private defines -----------------------------------------------------------*/
extern TIM_HandleTypeDef htim5;
// states

enum
{
    PWR_OFF,WAKE_UP,TRIG_PULL,SHOT_ONLY,SHOT_SOUND,MOD_LASER,ARC_BUTTON,TSR_EMPTY,TURN_ON,MODE_SLCT,TEST_MODE,CHARGE,NUM_STATES
};

enum
{
	OFF,ARM_RDY,WARN_REENG,STEALTH,NUM_MODES
};

enum
{
	LED0,LED1,LED2,LED3,LED4,LED5,LED6,LED7,LED8,LED9,LED10,BLU,RED,GRN,NUM_LED
};

enum
{
	LED_ON,LED_OFF
};

//VARIABLES
static int TEST_RATE_MS = 10;//ADJUST FOR BLINK RATE/DURATION ADJUSTMENT
uint16_t laserPulse = 0;
//volatile uint16_t laser_pulse[10] = {0,2560,8960,15360,28160,0,0,0,0,0};//MILO Pulses
volatile uint32_t laser_pulse[10] = {0,25344,36032,46656,57344,57344,57344,57344,57344,57344};//VIRTRA Pulses
//volatile uint32_t laser_pulse[10] = {0,5120,28160,47552,47552,47552,47552,47552,47552,47552};//TI Pulses
//volatile uint16_t mode_state[NUM_MODES] = {STEALTH,OFF,ARM_RDY,WARN_REENG,NUM_MODES};
volatile uint16_t mode = 9;
volatile uint16_t next_mode = 9;
short int TRIGGER = 0;
short int TRIG_HELD = 0;
volatile uint16_t roundsShot = 0;
volatile uint16_t numRounds = 9;
volatile uint16_t quarterBright = (1725);
volatile uint16_t halfBright = (3500);
volatile uint16_t fullBright = (6999);
volatile uint16_t flashRate = 2;//ADJUST FOR BLINK RATE/DURATION ADJUSTMENT
uint16_t SM_Counter = 0;
uint16_t State_Counter = 0;
uint32_t powerOffTimer = 50;/* 1 minute = 60 seconds  10 seconds = 5000, 20 = 10000, 30 = 15000, 40 = 20000, 50 = 25000, 60 = 30000, 600 = 300000*/
uint32_t offTimer = 30000;//5000 = 10 SECONDS = 2mS PER TICK/COUNT...MAX OF 65535 FOR 10 MINUTE: 10*60 = 600 SECONDS SO, 600/.002 = 300,000...TOO BIG FOR 16BIT
volatile short int SM_Flag = 0;
short int charging = 0;
short int KEEPON_STATE = 0;
short int CHRG_PLUGIN = 0;

#define STEALTH_ENTER_COUNT 3
typedef struct {
	uint8_t buttonRelCnt;
	uint8_t pinVal;
	bool 	isPressed;
} stealth_info_t;

stealth_info_t stealthInfo = {};

//STATE SPECIFIC EVENTS
void state_PWR_OFF(int event, int parameter);
void state_WAKE_UP(int event, int parameter);
void state_TSR_EMPTY(int event, int parameter);
void state_TRIG_PULL(int event, int parameter);
void state_SHOT_ONLY(int event, int parameter);
void state_SHOT_SOUND(int event, int parameter);
void state_MOD_LASER(int event, int parameter);
void state_ARC_BUTTON(int event, int parameter);
void state_TURN_ON(int event, int parameter);
void state_MODE_SLCT(int event, int parameter);
void state_TEST_MODE(int event, int parameter);
void state_CHARGE(int event, int parameter);

void sftdState_stateEvent(int event, int parameter)
{
    if (state[SFTD_STATE].curr_state == PWR_OFF) 
    	state_PWR_OFF(event, parameter);
    else if (state[SFTD_STATE].curr_state == WAKE_UP) 
    	state_WAKE_UP(event, parameter);
    else if (state[SFTD_STATE].curr_state == TRIG_PULL) 
    	state_TRIG_PULL(event, parameter);
    else if (state[SFTD_STATE].curr_state == SHOT_ONLY) 
    	state_SHOT_ONLY(event, parameter);
    else if (state[SFTD_STATE].curr_state == SHOT_SOUND) 
    	state_SHOT_SOUND(event, parameter);
	else if (state[SFTD_STATE].curr_state == MOD_LASER) 
		state_MOD_LASER(event, parameter);
	else if (state[SFTD_STATE].curr_state == ARC_BUTTON) 
		state_ARC_BUTTON(event, parameter);
	else if (state[SFTD_STATE].curr_state == TSR_EMPTY) 
		state_TSR_EMPTY(event, parameter);
	else if (state[SFTD_STATE].curr_state == TURN_ON) 
		state_TURN_ON(event, parameter);
	else if (state[SFTD_STATE].curr_state == MODE_SLCT) 
		state_MODE_SLCT(event, parameter);
	else if (state[SFTD_STATE].curr_state == TEST_MODE) 
		state_TEST_MODE(event, parameter);
	else if (state[SFTD_STATE].curr_state == CHARGE) 
		state_CHARGE(event, parameter);
}
void sftdStateInit(void)
{
	//get charge status
	/*
	 * #define PWR_MON_Pin GPIO_PIN_4
	   #define PWR_MON_GPIO_Port GPIOE
	 */
	turnAllOff();
	//INITIALIZE GPIO STATES
	initGPIOStates();

	//GET MODE_STATE
	mode = getMode();
	updateMode(mode);//update laser, flash, LED states.

	//Get pulse setting
	laserPulse = laser_pulse[getSwitch()];
	//Initialize state machine timer
	timerInit(SFTD_STATE_TIMER, TEST_RATE_MS);
	//check for charging, and power button off..go into CHARGE State if charging
	charging = HAL_GPIO_ReadPin(GRN_GPIO_Port, GRN_Pin);
	KEEPON_STATE = HAL_GPIO_ReadPin(PWR_MON_GPIO_Port, PWR_MON_Pin);
	if(KEEPON_STATE && !charging)
	{
		stateInit(SFTD_STATE, TURN_ON, NUM_STATES);
	}
	else if(!KEEPON_STATE && charging)
	{
		stateInit(SFTD_STATE, CHARGE, NUM_STATES);
	}
}
 
void sftdStateMonitor(void)
{
    stateEventHandler(SFTD_STATE, evTIMER_TICK, TEST_RATE_MS);
}
 
/*******************************************************************************
 * Function Name  : state_PWR_OFF()
 * State          :
 * Description    :
 * Input          : event, event parameter
 * Next States    : ...
 *******************************************************************************/
void state_PWR_OFF(int event, int parameter)
{
     switch (event)
    {
        case evENTER_STATE:
        	SM_Counter = 0;
        	State_Counter = 0;
        	turnAllOff();
            break;

        case evTIMER_TICK:
			while ((SM_Counter < 7000)&&(!KEEPON_STATE))
			  {
				  //State_Counter++;
				  SM_Counter += 10;
				  __HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,SM_Counter);
				  HAL_Delay(1);
				  KEEPON_STATE = HAL_GPIO_ReadPin(PWR_MON_GPIO_Port, PWR_MON_Pin);
				  CHRG_PLUGIN = HAL_GPIO_ReadPin(GRN_GPIO_Port, GRN_Pin);
				  if(!KEEPON_STATE&&(State_Counter++ > offTimer))
					{
						//TURN DEVICE OFF...
						HAL_GPIO_WritePin(KEEPON_GPIO_Port, KEEPON_Pin, RESET);

					}
				  else if(KEEPON_STATE&&(State_Counter < offTimer))
					{
						//MOVE BACK TO OPERATION...
					  	mode = getMode();//retrieve rotary switch position, and assign to mode.
						updateMode(mode);//update laser, flash, LED states.
						laserPulse = laser_pulse[getSwitch()];
						state[SFTD_STATE].next_state = SHOT_ONLY;

					}
				  else if(CHRG_PLUGIN)
				  {
					  state[SFTD_STATE].next_state = CHARGE;

				  }
			  }

			  while ((SM_Counter > 0)&&(!KEEPON_STATE))
			  {
				  //State_Counter++;
				  SM_Counter -= 10;
				  __HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,SM_Counter);
				  HAL_Delay(1);
				  KEEPON_STATE = HAL_GPIO_ReadPin(PWR_MON_GPIO_Port, PWR_MON_Pin);
				  if(!KEEPON_STATE&&(State_Counter++ > offTimer))
					{
						//TURN DEVICE OFF...
						HAL_GPIO_WritePin(KEEPON_GPIO_Port, KEEPON_Pin, RESET);

					}
				  else if(KEEPON_STATE&&(State_Counter < offTimer))
					{
						//MOVE BACK TO OPERATION...
						mode = getMode();//retrieve rotary switch position, and assign to mode.
						updateMode(mode);//update laser, flash, LED states.
						laserPulse = laser_pulse[getSwitch()];
						state[SFTD_STATE].next_state = SHOT_ONLY;

					}
				  else if(CHRG_PLUGIN)
				  {
					  state[SFTD_STATE].next_state = CHARGE;

				  }
			  }
            break;

        case evEXIT_STATE:
            break;
    }
}

/*******************************************************************************
 * Function Name  : state_CHARGE()
 * State          :
 * Description    : Stay in this state, until USB cable unplugged
 * Input          : event, event parameter
 * Next States    : ...
 *******************************************************************************/
void state_CHARGE(int event, int parameter)
{
     switch (event)
    {
        case evENTER_STATE:
        	turnAllOff();
        	turnOffDisp();
            break;

        case evTIMER_TICK:
        	charging = HAL_GPIO_ReadPin(GRN_GPIO_Port, GRN_Pin);
			KEEPON_STATE = HAL_GPIO_ReadPin(PWR_MON_GPIO_Port, PWR_MON_Pin);
			uint8_t trigPin = HAL_GPIO_ReadPin(TRIGGER_GPIO_Port, TRIGGER_Pin);
			if (!trigPin) {
				mode = getMode();
				updateMode(mode);
				state[SFTD_STATE].next_state = SHOT_ONLY;
			}
			if(KEEPON_STATE && !charging)
			{
				//mode = getMode();//retrieve rotary switch position, and assign to mode.
				//updateMode(mode);//update laser, flash, LED states.
				//laserPulse = laser_pulse[getSwitch()];
				//state[SFTD_STATE].next_state = SHOT_ONLY;
			}
			else if(!KEEPON_STATE)
			{
				keepOnState(0);//turn off
			}
            break;

        case evEXIT_STATE:
        	recallDisp();
            break;
    }
}

/*******************************************************************************
 * Function Name  : state_WAKE_UP()
 * State          :
 * Description    : 
 * Input          : event, event parameter
 * Next States    : ...
 *******************************************************************************/
void state_WAKE_UP(int event, int parameter)
{
    switch (event)
    {
        case evENTER_STATE:          
            break;

        case evSELECT_BUTTON_PRESSED:
            break;

        case evTIMER_TICK:
        	HAL_GPIO_TogglePin(DISP_LED11_GPIO_Port, DISP_LED11_Pin);
        	if(HAL_GPIO_ReadPin(TRIGGER_GPIO_Port, TRIGGER_Pin))
        	{
        		state[SFTD_STATE].next_state = PWR_OFF;
        	}
            break;

        case evEXIT_STATE:             
            break;
    }
}

/*******************************************************************************
 * Function Name  : state_TURN_ON()
 * State          :
 * Description    :
 * Input          : event, event parameter
 * Next States    : ...
 *******************************************************************************/
void state_TURN_ON(int event, int parameter)
{
     switch (event)
    {
        case evENTER_STATE:
        	//START TURN ON SOUND...
        	initDisp();
            break;

        case evTIMER_TICK:
        	//MOVE TO SHOT ONLY
        	state[SFTD_STATE].next_state = SHOT_ONLY;
            break;

        case evEXIT_STATE:
            break;
    }
}

/*******************************************************************************
 * Function Name  : state_MODE_SLCT()
 * State          :
 * Description    :
 * Input          : event, event parameter
 * Next States    : ...
 *******************************************************************************/
void state_MODE_SLCT(int event, int parameter)
{
     switch (event)
    {
        case evENTER_STATE:
			/*red_laser_en = !red_laser_en;
			red_lasers(red_laser_en);
			mode_sound(1);
			myTimer = 0; */
            break;

        case evTIMER_TICK:
			/*if(myTimer++ < 10);			//minimum zap time
			else
			{
				mode_sound(0);
				state[SFTD_STATE].next_state = state[SFTD_STATE].call_state;
			}*/
            break;

        case evEXIT_STATE:
            break;
    }
}

/*******************************************************************************
 * Function Name  : state_SHOT_ONLY()
 * State          :
 * Description    :
 * Input          : event, event parameter
 * Next States    : ...
 *******************************************************************************/
void state_SHOT_ONLY(int event, int parameter)
{
     switch (event)
    {
        case evENTER_STATE:
			//TRIG_HELD = 0;
        	SM_Counter = 0;
        	State_Counter = 0;
            break;

        case evTIMER_TICK:
        	SM_Counter++;
        	next_mode = getMode();
        	KEEPON_STATE = HAL_GPIO_ReadPin(PWR_MON_GPIO_Port, PWR_MON_Pin);
        	stealthInfo.pinVal = HAL_GPIO_ReadPin(GRN_GPIO_Port, GRN_Pin);
        	if (stealthInfo.pinVal) {
        		stealthInfo.isPressed = true;
        	} 
        	if (stealthInfo.isPressed && !stealthInfo.pinVal) {
        		stealthInfo.buttonRelCnt++;
        		stealthInfo.isPressed = false;
        	}
			if(TRIGGER)
        	{
				while(!HAL_GPIO_ReadPin(TRIGGER_GPIO_Port, TRIGGER_Pin));
        		state[SFTD_STATE].next_state = TRIG_PULL;
        	}
        	else if(mode != next_mode)
        	{//DEAL WITH USER CHANGING MODES HERE...
        		//state[SFTD_STATE].next_state = TRIG_PULL;
        		mode = getMode();//retrieve rotary switch position, and assign to mode.
        		next_mode = mode;
        		updateMode(mode);//update laser, flash, LED states.
        		laserPulse = laser_pulse[getSwitch()];
        	}
        	//else if((SM_Counter > flashRate)&&!SM_Flag)
        	else if((SM_Counter > flashRate)&&(!SM_Flag)&&(mode == WARN_REENG))
			{
				SM_Flag = 1;
				SM_Counter = 0;
				__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,6950);//does not work here in code for some reason...1/10/24 bw
				HAL_Delay(1);
			}
			//else if((SM_Counter > flashRate)&&SM_Flag)
			else if((SM_Counter > flashRate)&&(SM_Flag)&&(mode == WARN_REENG))
			{
				SM_Flag = 0;
				SM_Counter = 0;
				__HAL_TIM_SET_COMPARE(&htim5,TIM_CHANNEL_2,950);//does not work here in code for some reason...1/10/24 bw
				HAL_Delay(1);

			}

			State_Counter++;
			if(!KEEPON_STATE&&(State_Counter > powerOffTimer))
			//if(!KEEPON_STATE)
			{
				state[SFTD_STATE].next_state = PWR_OFF;

			}
			else if(stealthInfo.buttonRelCnt == STEALTH_ENTER_COUNT &&KEEPON_STATE)
			{
				state[SFTD_STATE].next_state = CHARGE;
				stealthInfo.buttonRelCnt = 0;
			}
            break;

        case evEXIT_STATE:
        	state[SFTD_STATE].call_state = SHOT_ONLY;
			/*ten_second_tmr_exp = 0;
			state[SFTD_STATE].call_state = SHOT_ONLY;
			myCounter = 0;
			setBit(PORTC,FAULT_LED);//turn off RED LED  */
            break;
    }
}

/*******************************************************************************
 * Function Name  : state_TRIG_PULL()
 * State          :
 * Description    : Primary state for device
 * Input          : event, event parameter
 * Next States    : ...

 numRounds = 2;
 volatile uint8_t roundsShot = 0;
 *******************************************************************************/
void state_TRIG_PULL(int event, int parameter)
{
    switch (event)
    {
        case evENTER_STATE:
			//red_lasers(red_laser_en);
            break;

        case evTIMER_TICK:
			if((roundsShot <= numRounds) && !TRIG_HELD)
			{//come here if loaded, and need to fire sound & laser
				roundsShot++;
				updateDisp(roundsShot,LED_OFF);
				state[SFTD_STATE].next_state = SHOT_SOUND;
			}
			else if((roundsShot > numRounds) && !TRIG_HELD)
			{//comes here when magazine is empty, plays the empty click sound the routes back to the calling state.
				state[SFTD_STATE].next_state = TSR_EMPTY;
			}
			else
			{
				state[SFTD_STATE].next_state = state[SFTD_STATE].call_state;
			}
			/*
			while(((!(PINB & (1<<TRIGGER))) && !done) && TRIG_HELD)//looking for trigger held...
			{//this is constantly polling, and will keep state machine from advancing to other states if trigger is still held.
				if(ten_second_tmr_exp)//if trigger held longer than 10 seconds, change modes of operation
				{
					//state[SFTD_STATE].next_state = ADVC_PRGM;
					play_sound(TONE_SELECT);
					flash_leds('G', 1, 100, 0);
					done = 1;					//gets us out of this while loop so we can advance to the next state
					roundsShot = 0;				//reset round counter for going into sensor mode.
					myCounter = 0;
				}
			}
			if(!ten_second_tmr_exp && TRIG_HELD)
			{
				state[SFTD_STATE].next_state = state[SFTD_STATE].call_state;
			}*/
            break;

        case evEXIT_STATE:
			/*ten_second_tmr_exp = 0;
			done = 0;
			//while_not = 0;
			myCounter = 0;*/
            break;
    }
}

/*******************************************************************************
 * Function Name  : state_MOD_LASER()
 * State          :
 * Description    :
 * Input          : event, event parameter
 * Next States    : ...
 *******************************************************************************/
void state_MOD_LASER(int event, int parameter)
{
     switch (event)
    {
        case evENTER_STATE:
        	//
        	FIRE_LASER(laserPulse);
			/*pulse_lsr1();
			myTimer = 0;
			while_not = 1; */
            break;

        case evTIMER_TICK:
        	if(roundsShot >

        	numRounds)
        	{
        		state[SFTD_STATE].next_state = TSR_EMPTY;
        	}
        	else
        	{
        		state[SFTD_STATE].next_state = SHOT_ONLY;
        	}
            break;

        case evEXIT_STATE:
        	TRIGGER = 0;
			/*TRIG_HELD = 1;
			zap_sound(0);
			myTimer = 0;
			setBit(PORTE,TAZE_LED);
			red_lasers(red_laser_en);  */
            break;
    }
}

/*******************************************************************************
 * Function Name  : state_SHOT_SOUND()
 * State          :
 * Description    :
 * Input          : event, event parameter
 * Next States    : ...
 *******************************************************************************/
void state_SHOT_SOUND(int event, int parameter)
{
     switch (event)
    {
        case evENTER_STATE:
			//if(enStatus.ss_en)zap_sound(1);
			//if(enStatus.rfd_en)rfd_tmr_req = true;
            break;

        case evTIMER_TICK:
			state[SFTD_STATE].next_state = MOD_LASER;
            break;

        case evBUTTON_RELEASED:
            break;

        case evEXIT_STATE:
            break;
    }
}

/*******************************************************************************
 * Function Name  : state_ARC_BUTTON()
 * State          :
 * Description    :
 * Input          : event, event parameter
 * Next States    : ...
 *******************************************************************************/
void state_ARC_BUTTON(int event, int parameter)
{
     switch (event)
    {
        case evENTER_STATE:
			//zap_sound(1);
			//myTimer = 0;
            break;

        case evTIMER_TICK:
			/*if(myTimer++ < arcMin);			//minimum zap time
			else if(!(PIND & (1<<ARC_BUT)));			//Stay here
			else state[SFTD_STATE].next_state = SHOT_ONLY;*/
            break;

        case evBUTTON_RELEASED:
            break;

        case evEXIT_STATE:
			//zap_sound(0);
			//myTimer = 0;
            break;
    }
}

/*******************************************************************************
 * Function Name  : state_TSR_EMPTY()
 * State          :
 * Description    :
 * Input          : event, event parameter
 * Next States    : ...
 *******************************************************************************/
void state_TSR_EMPTY(int event, int parameter)
{
     switch (event)
    {
        case evENTER_STATE:
			/*timerInit(SFTD_STATE_TIMER, 500);
			clearBit(PORTC,FAULT_LED);
			while_not = 1; */
        	turnAllOff();
        	HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, SET);
            break;

        case evTIMER_TICK:
        	keepOnTest();
            break;

        case evBUTTON_RELEASED:
            break;

        case evEXIT_STATE:
            break;
    }
}

/*******************************************************************************
 * Function Name  : state_TEST_MODE()
 * State          :
 * Description    :
 * Input          : event, event parameter
 * Next States    : ...
 *******************************************************************************/
void state_TEST_MODE(int event, int parameter)
{
     switch (event)
    {
		 case evENTER_STATE:
		/* myTimer = 0;
		 myCounter = 0;
		 END = 0;
		 irtimer = 0;*/

		 break;

		 case evTIMER_TICK:
		/* if((!(PINB & (1<<TRIGGER)))&&(!END))
		 {
			 myTimer = 0;
			 myCounter = 0;
			 END = 1;
			 irtimer = 0;
			 clearBit(PORTC,FAULT_LED);//turn off RED LED
			 clearBit(PORTC,READY_LED);
			 red_lasers(0);
			 RUN = 1;
		 }
		 else if((myTimer++ >= 10) && RUN)
		 {
			 myCounter++;
			 if(myCounter > 3)
			 {
				 END = 0;
				 myCounter = 1;
			 }
			 else
			 {
				 turnon_sound(myCounter);
				 myTimer = 0;
				 irtimer++;
			 }
		 }
		 else if(irtimer == 8)
		 {
			 setBit(PORTB,LSR1_PWM);
		 }
		 else if(irtimer == 18)
		 {
			 clearBit(PORTB,LSR1_PWM);
			 setBit(PORTD,LSR2_PWM);
		 }
		 else if(irtimer == 28)
		 {
			 turnon_sound(0);
			 clearBit(PORTB,LSR1_PWM);
			 clearBit(PORTD,LSR2_PWM);
		 }*/
		 break;

		 case evEXIT_STATE:
		 break;
    }
}













