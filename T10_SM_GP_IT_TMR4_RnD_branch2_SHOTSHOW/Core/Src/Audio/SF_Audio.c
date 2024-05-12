/**
 * @file SF_Audio.h
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
////////////////////////////////////////////////////////////////////////////////
///
///                           Internal Data
///
////////////////////////////////////////////////////////////////////////////////
///
/// task and queue handles
///
osThreadId 		buttonTaskHandle;
osThreadId 		audioTaskHandle;
osMessageQId	audioQueueHandle;
///
/// Holds information about the current clip that is playing
///
clip_info_t curClip = {.clip = NONE, .slot = 0, .count = 0};
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
static void _Play(uint16_t* pBuf, uint32_t len) {
  uint8_t   k = 0;
  uint8_t   i = len/MAX_DMA_VAL;
  //
  // assume audio clip is larger than max size (2^16)
  //
  do {
    //
    // Advance to start of next chunk
    //
    uint16_t* pData = &pBuf[k * MAX_DMA_VAL];
    //
    // Get the length of current chunk
    //
    uint32_t l        = len - MAX_DMA_VAL * k;
    uint32_t clipSize = MIN(l, MAX_DMA_VAL);
    //
    // make sure we are ready before transmitting
    //
    while (HAL_I2S_GetState(&hi2s3) != HAL_I2S_STATE_READY);
    HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)pData, clipSize);
    k++;
  } while (i-->0);
  return;
}
///
/// @brief  Supplies the DMA with appropriate audio clip data 
///
/// @param  audio_clips_t: WARNING | POWER_ON | SHOT | TONE 
///
/// @return void
///
static void PlayClip(audio_clips_t clip) {
    curClip.clip = clip;
    switch (clip) {
        case WARNING:
          _Play((uint16_t*)&PwrOnConcise[0], sizePwrOnConcise);
          break;
        case POWER_ON:
          _Play((uint16_t*)&PwrOnConcise[0], sizePwrOnConcise);
          break;
        case SHOT:
          _Play((uint16_t*)&Shot[0], sizeShot);
          break;
        case TONE:
          _Play((uint16_t*)&Tone[0], sizeTone);
          break;
    default:
      break;
    }
    curClip.slot++;
}
///
/// @brief  Read pins from GPI pins
///
/// @return uint32_t - pin value
///
static uint32_t _ReadPins() {
  uint32_t  res =   0;
  res       =   HAL_GPIO_ReadPin(GPIOE, SW5_Pin)  <<  2;
  res       |=  HAL_GPIO_ReadPin(GPIOE, SW6_Pin)  <<  1;
  res       |=  HAL_GPIO_ReadPin(GPIOA, TRIGGER_Pin);
  return  res;
}
///
/// @brief  Function implementing the audio task.
///
/// @param  argument: Hardcoded to 0.
///
/// @return void
///
static void AudioTask(void const * argument) {
    osEvent 		event;
    audio_clips_t 	clip = 0;
    PlayClip(POWER_ON);
    for (;;) {
        event = osMessageGet(audioQueueHandle, osWaitForever);
        if (event.status == osEventMessage) {
            event.def.message_id    = audioQueueHandle;
            clip                    = (audio_clips_t)event.value.v;
            PlayClip(clip);
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
static void ButtonTask(void const * argument) {
	uint32_t	curPinVal	=  0;
	uint32_t 	oldPinVal   =  -1;
	for (;;) {
	curPinVal	=	_ReadPins();
	//
	// take action on button release only with exception of SW5 (warning clip)
	//
	if (curPinVal != oldPinVal || curPinVal == 3) {
		oldPinVal	=	curPinVal;
		//
		// Only attempt to play clip if there isn't one already playing
		//
		if (HAL_I2S_GetState(&hi2s3) == HAL_I2S_STATE_READY) {
			switch (curPinVal) {
					case 3:
						osMessagePut (audioQueueHandle, WARNING, 100);
						break;
					case 6:
						osMessagePut (audioQueueHandle, SHOT, 100);
						break;
	 				// case 6:
	 				// 	osMessagePut (audioQueueHandle, TONE, 100);
	 				// 	break;
					default:
						break;
			}
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
	if (curClip.clip == SHOT) {
		curClip.count++;
		if (curClip.count >=2) {
			osMessagePut (audioQueueHandle, TONE, 0);
			curClip.count = 0;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
///
///                           External Functions
///
////////////////////////////////////////////////////////////////////////////////
void SF_AudioInit() {
	//
	// Register for the DMA complete callback
	//
  	HAL_I2S_RegisterCallback(&hi2s3, HAL_DMA_XFER_CPLT_CB_ID, HAL_I2S_TxCpltCallback);
	//
	// Create the audio queue
	//
	osMessageQDef(audioQueue, 16, uint16_t);
	audioQueueHandle = osMessageCreate(osMessageQ(audioQueue), NULL);
	//
	// Create the audio task
	//
	osThreadDef(audioTask, AudioTask, osPriorityIdle, 0, 128);
	audioTaskHandle = osThreadCreate(osThread(audioTask), NULL);
	//
	// Create the button task
	//
	osThreadDef(buttonTask, ButtonTask, osPriorityNormal, 0, 128);
	buttonTaskHandle = osThreadCreate(osThread(buttonTask), NULL);

}
////////////////////////////////////////////////////////////////////////////////
///
///                              Global Data
///
////////////////////////////////////////////////////////////////////////////////
