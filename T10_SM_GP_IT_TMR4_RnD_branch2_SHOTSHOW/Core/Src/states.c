////////////////////////////////////////////////////////////////////////////////////////////
////                      states.c
////                      
////    Desc: State Machine mechanics 
////////////////////////////////////////////////////////////////////////////////////////////

/* Includes ------------------------------------------------------------------*/
#include "timing.h"
#include "states.h"
#include "SFTD_states.h"


// This small implementation of state.c provides state machine using a table
// instead of pointers
//extern void motor_stateEvent(int event, int parameter);

/* Private define ------------------------------------------------------------*/
/* States --------------------------------------------------------------------*/
/* Events --------------------------------------------------------------------*/
/* Private MACROS  -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
tSTATE state[NUM_STATE_MACHINES];

/* - Initialize State Machine -------------------------------------------------*/
void stateInit(int id, int first_state, int max_states)
{
    // set state and next state equal
    state[id].curr_state = first_state;
    state[id].next_state = first_state;
    state[id].timer = 0;
    // enter current state
    stateEvent(id, evENTER_STATE, 0);
}

/* - Execute an Event in State Machine and ENTER/EXIT states-------------------*/
void stateEventHandler(int id, int event, int parameter)
{
    if (event == evTIMER_TICK) state[id].timer += parameter;
    // call state machine
    stateEvent(id, event, parameter);

    // check for change of state
    if (state[id].next_state != state[id].curr_state)
    {
        // exit current state
        stateEvent(id, evEXIT_STATE, 0);
        // change to next state
        state[id].curr_state = state[id].next_state;
        // reset tick timer
        state[id].timer = 0;
        timerReset(TICK_TIMER);
        // enter current state
        stateEvent(id, evENTER_STATE, 0);
    }
}

/* - Conversion specific to State Machine -------------------*/
void stateEvent(int id, int event, int parameter)
{
    if (id == SFTD_STATE) sftdState_stateEvent(event, parameter);
}
