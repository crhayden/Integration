/**
 * @file SF_Battery.h
 * @brief Polls battery voltage and updates LED
 * 
 * @date 5/25/2024
 * 
 **/
#ifndef SRC_BATTERY_SF_BATTERY_H_
#define SRC_BATTERY_SF_BATTERY_H_
#include "main.h"
#include "states.h"
#include "stdbool.h"
////////////////////////////////////////////////////////////////////////////////
///
///                              External Data
///
////////////////////////////////////////////////////////////////////////////////
extern bool manufacturingMode;
////////////////////////////////////////////////////////////////////////////////
///
///                              External Functions
///
////////////////////////////////////////////////////////////////////////////////
///
/// Initializes the Battery moinitoring task
///
/// @return     void
///
void SF_BatteryInit();
#endif /* SRC_BATTERY_SF_BATTERY_H_ */
