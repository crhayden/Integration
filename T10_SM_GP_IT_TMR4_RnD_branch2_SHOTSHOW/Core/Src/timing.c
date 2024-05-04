/*******************************************************************************
* File Name      : timing.c
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
/* Includes ------------------------------------------------------------------*/
#include "timing.h"
#include "stm32f4xx_it.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Public variables ---------------------------------------------------------*/
tSW_TIMER tmr[NUM_TIMERS];

/*******************************************************************************
* Function Name  : timerInit()
* Description    : initialize software timer
* Input          : pointer to software timer structure, timing period
* Output         : None
* Return         : None
*******************************************************************************/
void timerInit( int id, unsigned long period )
{
    tmr[id].period = period;
    tmr[id].reference  = getSystickTime();
}
/*******************************************************************************
* Function Name  : timerExpired()
* Description    : checks to see if timer expired
* Input          : pointer to software timer structure
* Output         : None
* Return         : TRUE if expired, FALSE if not expired
*******************************************************************************/
unsigned char timerExpired( int id )
{
    unsigned long time_diff;

    time_diff = getSystickTime() - tmr[id].reference;
    if (( time_diff !=0 ) && ( time_diff >= tmr[id].period ))
        return ( 1 );
    else
        return ( 0 );
}
/*******************************************************************************
* Function Name  : timerGetTime()
* Description    : gets current timer value
* Input          : pointer to software timer structure
* Output         : None
* Return         : current timer value
*******************************************************************************/
long timerGetTime( int id )
{
    return ( getSystickTime() - tmr[id].reference );
}
/*******************************************************************************
* Function Name  : timerReload()
* Description    : adds timer period to reference value
* Input          : pointer to software timer structure
* Output         : None
* Return         : None
*******************************************************************************/
void timerReload( int id )
{
    tmr[id].reference += tmr[id].period;
}
/*******************************************************************************
* Function Name  : timerReset()
* Description    : resets timer reference value to current time
* Input          : pointer to software timer structure
* Output         : None
* Return         : None
*******************************************************************************/
void timerReset( int id )
{
    tmr[id].reference = getSystickTime();
}
/*******************************************************************************
* Function Name  : timerDelay()
* Description    : blocked delay
* Input          : period
* Output         : None
* Return         : None
*******************************************************************************/
void timerDelay( unsigned long period )
{

    timerInit( TICK_TIMER, period );
#ifdef WATCHDOG_IMPLEMENT
    while ( !timerExpired( DELAY_TIMER ) )
    IWDG_ReloadCounter(); /* Reload IWDG counter */
#else
    while ( !timerExpired( TICK_TIMER ) );
#endif
}
/*******************************************************************************
* Function Name  : delay_ms()
* Description    : millisecond delay timer from CCS...do not use, replace
* Input          : period
* Output         : None
* Return         : None
*******************************************************************************/
/*
void delay_ms(int delay)
{
    //__delay_ms(delay);

}
*/
