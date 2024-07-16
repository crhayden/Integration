/**
 * @file SF_Audio.h
 * @brief Manages audio playback & monitors buttons 
 * 
 * @date 4/30/2024
 * 
 **/
#ifndef SRC_AUDIO_SF_AUDIO_H_
#define SRC_AUDIO_SF_AUDIO_H_
#include "main.h"
////////////////////////////////////////////////////////////////////////////////
///
///                              External Data
///
////////////////////////////////////////////////////////////////////////////////
///
/// Types of audio clips
///
typedef enum {
    NONE,
    WARNING,
    POWER_ON,
    SHOT,
    TONE,
} audio_clips_t;
///
/// Clip info
///
typedef struct {
    audio_clips_t   clip;		// clip type
    uint8_t         totalSlots;	// 1 slot = MAX_DMA_VAL
    uint8_t			curSlot;	// currrent chunk of audio clip to be played
    uint8_t			count;		// how many times the current clip has played
    uint8_t			dartsFired; // how many darts have been fired
    uint8_t			totalDarts; // total number of darts
} clip_info_t;

extern clip_info_t curClip;
////////////////////////////////////////////////////////////////////////////////
///
///                              External Functions
///
////////////////////////////////////////////////////////////////////////////////
///
/// Initializes the audio subsystem
///
/// @return     void
///
void SF_AudioInit();
#endif /* SRC_AUDIO_SF_AUDIO_H_ */
