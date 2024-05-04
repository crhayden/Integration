////////////////////////////////////////////////////////////////////////////////////////////
////                      states.h
////                     
////    Desc: State Machine mechanics 
////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _STATES_H
#define _STATES_H

/* Private define ------------------------------------------------------------*/
#define TICK_TIMER_MS    100

/* States --------------------------------------------------------------------*/
enum {SFTD_STATE,NUM_STATE_MACHINES};

/* Events --------------------------------------------------------------------*/
enum {evENTER_STATE, evBUTTON_PRESSED, evBUTTON_RELEASED, evSELECT_BUTTON_PRESSED, evTIMER_TICK, evEXIT_STATE, evUSER=100};

/* Private MACROS  -----------------------------------------------------------*/
void stateInit(int id, int first_state, int max_states);
void stateEventHandler(int id, int event, int parameter);
void stateEvent(int id, int event, int parameter);

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
    int curr_state;
    int next_state;
    long timer;
	int call_state;
	int temp_state;
} tSTATE;

/* Public variables ----------------------------------------------------------*/
extern tSTATE state[NUM_STATE_MACHINES];

#endif
