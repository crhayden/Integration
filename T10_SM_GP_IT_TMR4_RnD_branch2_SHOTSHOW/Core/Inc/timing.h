/*******************************************************************************
* File Name      : timing.h
* Description    : timing routines using the systick timer
* Microcontroller: dsPIC33
*
*     $Archive:  /chip/ $
*     $Author:   Brian$
*     $Date:     2/19/17 $
*     $Revision: 1.1 $
*     $History:  Modifed for dsPIC33 $
*
*******************************************************************************/ 
   

#ifndef SW_TIMING_H_
#define	SW_TIMING_H_

enum {TICK_TIMER,SFTD_STATE_TIMER,NUM_TIMERS};

#define TIMER_EXPIRED   0

//#define ONE_SECOND      1000
#define HUNDRED_MD      (ONE_SECOND / 10)
#define TEN_MS          (ONE_SECOND / 100)


typedef struct
{
    unsigned long period;
    unsigned long reference;
}tSW_TIMER;

// function prototypes
void timerInit(int id, unsigned long period);
unsigned char  timerExpired(int id);
long timerGetTime(int id);
void timerReload(int id);
void timerReset(int id);
void timerDelay(unsigned long period);

#endif

