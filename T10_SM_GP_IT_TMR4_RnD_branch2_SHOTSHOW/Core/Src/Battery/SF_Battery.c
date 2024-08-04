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
#define NUM_BAT_SAMPLES 		10
#define HYSTERESIS				0.01f
#define GOOD_THRESHOLD			3.6f
#define LOW_THRESHOLD 			3.26f
#define DEAD_THRESHOLD			2.75f
#define BATTERY_TASK_SLEEP_TIME	500   
#define KEEPON_PWR_DURATION		3000/BATTERY_TASK_SLEEP_TIME
#define IRLASER_ON_INTERVAL_CNT	9000/BATTERY_TASK_SLEEP_TIME   // hold trigger & down on selector for 9 seconds to turn on IRLaser
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
		HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, RESET);
		HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, SET);
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
    GPIO_PinState isCharging = GPIO_PIN_SET;
	uint8_t 	selectorDownPinVal 		=	0;
	uint8_t 	selectorDownCount  		=	0;
	uint8_t 	trigPinVal 				=	0;
	uint8_t 	trigDownCount  			=	0;
	uint8_t 	timeInMancturingMode	= 	0;	
    for (;;) {
    	//
    	// if we are charging set the LED green, otherwise monitor battery as usual.
    	//
		uint8_t warningPinVal	=  HAL_GPIO_ReadPin(GPIOE, SW5_Pin);
    	if (systemState != CHARGE && !warningPinVal) {
			if (!HAL_GPIO_ReadPin(GRN_CHARGE_GPIO_Port, GRN_CHARGE_Pin)) {
				HAL_GPIO_WritePin(GRN_GPIO_Port, GRN_Pin, SET);
				HAL_GPIO_WritePin(BLU_GPIO_Port, BLU_Pin, RESET);
				HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, RESET);
			} else {
				_setLED();
	    		HAL_ADC_Start_IT(&hadc1);
			}
		}
		trigPinVal			=HAL_GPIO_ReadPin(GPIOA, TRIGGER_Pin);
	 	if (!trigPinVal) {
	 		trigDownCount++;
	 	} else {
	 		trigDownCount = 0;
	 	}
	 	selectorDownPinVal 	= HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin);
	 	if (selectorDownPinVal) {
	 		selectorDownCount++;
	 	} else {
	 		selectorDownCount = 0;
	 	}
		if (trigDownCount >= IRLASER_ON_INTERVAL_CNT && selectorDownCount >= IRLASER_ON_INTERVAL_CNT) {	
			batteryState = LOW;
			_setLED();
			HAL_GPIO_WritePin(KEEPON_GPIO_Port, KEEPON_Pin, SET);
  			HAL_GPIO_WritePin(IRLASER_GPIO_Port, IRLASER_Pin, RESET); 
  			manufacturingMode	= true;
  			trigDownCount 		= 0;
  			selectorDownCount 	= 0;
		}
		if (manufacturingMode) {
			timeInMancturingMode++;
			if (timeInMancturingMode >= KEEPON_PWR_DURATION) {
				HAL_GPIO_WritePin(KEEPON_GPIO_Port, KEEPON_Pin, RESET);
				timeInMancturingMode = 0;
			}
		}
        osDelay(BATTERY_TASK_SLEEP_TIME);
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
bool 		manufacturingMode		=	false;

