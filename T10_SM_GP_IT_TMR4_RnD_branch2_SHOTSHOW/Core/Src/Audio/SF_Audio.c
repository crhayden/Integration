/**
 * @file SF_Audio.c
 * @brief Manages audio playback & monitors buttons 
 * 
 * @date 4/30/2024
 * 
 **/
#include "SF_Audio.h"
#include <math.h>
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
///
/// Standard mimnimum comparison
///
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
///
/// Max length that can be played by DMA controller 
///
#define MAX_DMA_VAL (uint32_t)(pow(2,16)-1)
////////////////////////////////////////////////////////////////////////////////
////
///
///                           Internal Types
///
////////////////////////////////////////////////////////////////////////////////
///
/// Trigger info
///
typedef struct {
    uint8_t         pinVal;		
    bool			didRelease;	
} trig_data_t;
////////////////////////////////////////////////////////////////////////////////
///
///                           Internal Data
///
////////////////////////////////////////////////////////////////////////////////

const osThreadAttr_t audioTask_attributes = {
  .name = "audioMonitorTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

const osThreadAttr_t buttonTask_attributes = {
  .name = "buttonMonitorTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
///
/// task and queue handles
///
osThreadId_t 		buttonTaskHandle;
osThreadId_t 		audioTaskHandle;
osMessageQueueId_t	audioQueueHandle;

/////////
///
/// Holds information about the current clip that is playing
///
clip_info_t curClip		= {.clip 	= NONE, .totalSlots = 0, .curSlot = 0, .count = 0, .dartsFired = 0, .totalDarts = 10};
////////////////////////////////////////////////////////////////////////////////
///
///                           Internal Functions
///
////////////////////////////////////////////////////////////////////////////////
///
/// @brief  Supplies the DMA with appropriate audio clip data 
///
/// @param  pBuf*: pointer to the audio clip data
/// @param  len: 	length of audio clip
///
/// @return void
///
static void _StartDMA(uint16_t* pBuf, uint32_t len) {
	//
	// assume audio clip is larger than max size (2^16)
	//
	uint32_t clipSize = MIN(len, MAX_DMA_VAL);
	HAL_I2S_Transmit_DMA(&hi2s3, pBuf, clipSize);
	return;
}
///
/// @brief  Supplies the DMA with appropriate audio clip data 
///
/// @param  audio_clips_t: WARNING | POWER_ON | SHOT | TONE 
///
/// @return void
///
static void _SelectAudioClip(audio_clips_t clip) {
    curClip.clip = clip;
    switch (clip) {
		case WARNING:
			_StartDMA((uint16_t*)&PwrOnConcise[0], sizePwrOnConcise);
			break;
		case POWER_ON:
		  	_StartDMA((uint16_t*)&PwrOnConcise[0], sizePwrOnConcise);
		  	break;
		case SHOT:
			if(!manufacturingMode) {
        		FIRE_LASER(laserPulse);
    		}
			_StartDMA((uint16_t*)&Shot[0], sizeShot);
			break;
		case TONE:
			curClip.totalSlots = sizeTone/MAX_DMA_VAL;
		    _StartDMA((uint16_t*)&Tone[0], sizeTone);
		  	break;
    default:
      break;
    }
}
///
/// @brief  Read trigger pin
///
/// @return uint8_t - pin value
///
static uint8_t _readTrigger() {
	uint8_t res	=  HAL_GPIO_ReadPin(GPIOA, TRIGGER_Pin);
	return	res;
}
///
/// @brief  Read warning pin
///
/// @return uint8_t - pin value
///
static uint8_t _readWarning() {
	uint8_t res	=  HAL_GPIO_ReadPin(GPIOE, SW5_Pin);
	return	res;
}
///
/// @brief  Read stealth pin (push down)
///
/// @return uint8_t - pin value
///
static uint8_t _readStealth() {
	uint8_t res	=  HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin);
	return	res;
}
///
/// @brief  Function implementing the audio task.
///
/// @param  argument: Hardcoded to 0.
///
/// @return void
///
static void AudioTask(void * argument) {
    //osEvent 		evt;
	audio_clips_t 	qMsg			=  	0;
    audio_clips_t 	clip = 0;
    //
    // Hack for IRLaser 4mS issue hardware workaround. Remove this on next rev of hardware.
    // Turn on the 5Volts to the IR Laser.
    //
    HAL_GPIO_WritePin(GPIOD, RF_PWR_CT_Pin, SET);
	uint8_t pinVal = _readStealth();
	if (!pinVal) {
	    _SelectAudioClip(POWER_ON);
	}
    for (;;) {
		if (osMessageQueueGet(audioQueueHandle, &qMsg, 0, osWaitForever) ==  osOK){
			//evt.def.message_id    = audioQueueHandle;
			clip                    = qMsg;
			osDelay(35);
			_SelectAudioClip(clip);
		}
        osDelay(100);
    }
}
///
/// @brief  Function implementing the button task.
///
/// @param  argument: Hardcoded to 0.
///
/// @return void
///
static void ButtonTask(void * argument) {
	uint8_t 	warnPinVal			=  	0;
	audio_clips_t 	qMsg			=  	0;

	///
	/// Holds information about the trigger data
	///
	trig_data_t		trigData	= 	{.pinVal	= 1,	.didRelease = true};
	for (;;) {

		trigData.pinVal	=	_readTrigger();
		if (trigData.pinVal) {
			trigData.didRelease = true;
		}
		warnPinVal	=	_readWarning();
		if (HAL_I2S_GetState(&hi2s3) == HAL_I2S_STATE_READY) { 
			if (curClip.dartsFired < curClip.totalDarts){
				if (!trigData.pinVal && trigData.didRelease) {
					curClip.dartsFired++; 
					trigData.didRelease = false;
					qMsg = SHOT;
					osMessageQueuePut (audioQueueHandle, &qMsg, 0, 100);
				}
			} 
			if (warnPinVal) {
				qMsg = WARNING;
				osMessageQueuePut (audioQueueHandle, &qMsg, 0, 100);
			}
		}
		osDelay(50);
	}
}
//------------------------------------------------------------------------------
//
//                           Callback Handlers
//
//------------------------------------------------------------------------------

void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s) {
	audio_clips_t 	qMsg			=  	0;
	if (curClip.clip == TONE) {
		//
		// if we've reached the end, reset
		//
		if (++curClip.curSlot == curClip.totalSlots){
			curClip.curSlot = 0;
			return;
		} else {
			//
			// Advance to start of next slot
			//
			uint16_t* pData = (uint16_t*)&Tone[curClip.curSlot * MAX_DMA_VAL];
			//
			// Get the length of current slot
			//
			uint32_t l        = sizeTone - MAX_DMA_VAL * curClip.curSlot;
			uint32_t clipSize = MIN(l, MAX_DMA_VAL);
			//
			// restart the DMA
			//
			HAL_I2S_Transmit_DMA(&hi2s3, pData, clipSize);
		}
	} else if (curClip.clip == SHOT) {
		//
		// If this is the 2nd consecutive tigger pull - start the tone.
		//
		if (++curClip.count == 2) {
			qMsg = TONE;
			osMessageQueuePut (audioQueueHandle, &qMsg, 0, 0);
			curClip.count = 0;
		}
	} else {
		curClip.count = 0;
	}
}
////////////////////////////////////////////////////////////////////////////////
///
///                           External Functions
///
////////////////////////////////////////////////////////////////////////////////
const osMessageQueueAttr_t audioQueue_attributes = { 
  .name = "audioQueue"
}; 
void SF_AudioInit() {
	//
	// Register for the DMA complete callback
	//
  	HAL_I2S_RegisterCallback(&hi2s3, HAL_DMA_XFER_CPLT_CB_ID, HAL_I2S_TxCpltCallback);
	//
	// Create the audio queue
	//
	audioQueueHandle = osMessageQueueNew(16, sizeof(uint32_t), &audioQueue_attributes);
	//
	// Create the audio task
	//
	audioTaskHandle = osThreadNew(AudioTask, NULL, &audioTask_attributes);
	//
	// Create the button task
	//
	buttonTaskHandle = osThreadNew(ButtonTask, NULL, &buttonTask_attributes);

}
////////////////////////////////////////////////////////////////////////////////
///
///                              Global Data
///
////////////////////////////////////////////////////////////////////////////////
