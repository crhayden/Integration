/**
 * @file SF_Battery.h
 * @brief Polls battery voltage and updates LED
 * 
 * @date 5/25/2024
 * 
 **/
#include "SF_Battery.h"
////////////////////////////////////////////////////////////////////////////////
///
///                           Internal Constants
///
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
///
///                           Internal Macros
///
////////////////////////////////////////////////////////////////////////////////
#define NUM_BAT_SAMPLES 10
#define HYSTERESIS		0.01f
#define GOOD_THRESHOLD	3.6f
#define LOW_THRESHOLD 	3.26f
#define DEAD_THRESHOLD	2.75f
////////////////////////////////////////////////////////////////////////////////
////
///
///                           Internal Types
///
////////////////////////////////////////////////////////////////////////////////
///
/// Battery states
///
typedef enum {
	GOOD,
	LOW,
	DEAD,
} bat_state_t;
////////////////////////////////////////////////////////////////////////////////
///
///                           Internal Data
///
////////////////////////////////////////////////////////////////////////////////
///
/// task and  handles
///
osThreadId 		batteryTaskHandle;
///
/// current battery state
///
bat_state_t 	batteryState = GOOD;
////////////////////////////////////////////////////////////////////////////////
///
///                           Internal Functions
///
////////////////////////////////////////////////////////////////////////////////
static void _setLED() {
	switch (batteryState){
	case GOOD:
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, SET);
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET);
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET);	
		break;
	case LOW:
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, SET);
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET);
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, SET);
		break;
	case DEAD:
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET);
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET);
		HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, SET);
		break;
	}
}
///
/// @brief  Function implementing the battery task.
///
/// @param  argument: Hardcoded to 0.
///
/// @return void
///
static void BatteryTask(void const * argument) {
    for (;;) {
    	HAL_ADC_Start_IT(&hadc1);
        osDelay(500);
    }
}
//------------------------------------------------------------------------------
//
//                           Callback Handlers
//
//------------------------------------------------------------------------------

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc1) {
	//
	// When playing audio - the rail is temporality pulled down and gives a false battery/ADC reading
	//
	if (HAL_I2S_GetState(&hi2s3) == HAL_I2S_STATE_READY) {
		//
		// Don't make updated when we are in stealth mode
		//
		if (systemState != CHARGE) {
			const uint32_t maxADCVal = 4095;

			float voltage = 0;
			for (int i = 0; i<=NUM_BAT_SAMPLES; i++) {
				uint32_t adcVal = HAL_ADC_GetValue (hadc1);
				voltage += (((float)adcVal/(float)maxADCVal) * 3.3) * 1.6f;
			}
			voltage = voltage/NUM_BAT_SAMPLES;
			switch (batteryState) {
				case GOOD:
					if (voltage < DEAD_THRESHOLD - HYSTERESIS) {
						batteryState = DEAD;
						_setLED();
					} else if (voltage < GOOD_THRESHOLD - HYSTERESIS) {		
						batteryState = LOW;
						_setLED();
					} else if (voltage > GOOD_THRESHOLD + HYSTERESIS) {
						batteryState = GOOD;
						_setLED();	
					}
					break;
				case LOW:
					if (voltage < DEAD_THRESHOLD - HYSTERESIS) {
						batteryState = DEAD;
						_setLED();
					} else if (voltage > GOOD_THRESHOLD + HYSTERESIS) {
						batteryState = GOOD;
						_setLED();
					}
					break;
				case DEAD:
					if (voltage > LOW_THRESHOLD + HYSTERESIS) {
						batteryState = LOW;
						_setLED();
					}
					break;
				default:
					break;
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
///
///                           External Functions
///
////////////////////////////////////////////////////////////////////////////////
void SF_BatteryInit() {  
	HAL_GPIO_WritePin(GPIOD, BATT_MSR_EN_Pin, GPIO_PIN_SET);
	//
	// Register for the DMA complete callback
	//
  	HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID, HAL_ADC_ConvCpltCallback);
	//
	// Create the battery task
	//
	osThreadDef(batteryTask, BatteryTask, osPriorityIdle, 0, 128);
	batteryTaskHandle = osThreadCreate(osThread(batteryTask), NULL);

}
////////////////////////////////////////////////////////////////////////////////
///
///                              Global Data
///
////////////////////////////////////////////////////////////////////////////////

