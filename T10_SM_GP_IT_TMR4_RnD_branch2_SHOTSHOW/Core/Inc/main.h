/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <AudioClips/SF_Shot.h>
#include <AudioClips/SF_PwrOnConcise.h>
#include <AudioClips/SF_Tone.h>
#include "SF_Audio.h"
#include "SF_Battery.h"
#include "states.h"
#include "stdbool.h"
#include "cmsis_os.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern DMA_HandleTypeDef hdma_spi3_tx;
extern I2S_HandleTypeDef hi2s3;
extern ADC_HandleTypeDef hadc1;
extern state_t systemState;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);
void FIRE_LASER();

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

// Target Manufacturer
#define VIRTRA_ENABLED 		0
#define MILO_ENABLED		0
#define TI_ENABLED			0
#define LASER_AMO			0
#define SET_CAN_ENABLED		1

// Shot Counter LEDs
#define CONFIG_SHOT_COUNTER_LED_RANDOM        0
#define CONFIG_SHOT_COUNTER_LED_RIGHT_TO_LEFT 1
#if CONFIG_SHOT_COUNTER_LED_RANDOM && CONFIG_SHOT_COUNTER_LED_RIGHT_TO_LEFT
#error "Choose only one option: CONFIG_SHOT_COUNTER_LED_RANDOM or CONFIG_SHOT_COUNTER_LED_RIGHT_TO_LEFT"
#endif

#define CONFIG_SHOT_COUNTER_CHASE_SPEED_MS 40

#define SIMULATED_ENABLED 	0
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SWC_Pin GPIO_PIN_2
#define SWC_GPIO_Port GPIOE
#define SWD_Pin GPIO_PIN_3
#define SWD_GPIO_Port GPIOE
#define PWR_MON_Pin GPIO_PIN_4
#define PWR_MON_GPIO_Port GPIOE
#define GRN_CHARGE_Pin GPIO_PIN_5
#define GRN_CHARGE_GPIO_Port GPIOE
#define SW5_Pin GPIO_PIN_6
#define SW5_GPIO_Port GPIOE
#define TRIGGER_Pin GPIO_PIN_0
#define TRIGGER_GPIO_Port GPIOA
#define TRIGGER_EXTI_IRQn EXTI0_IRQn
#define FLASH_Pin GPIO_PIN_1
#define FLASH_GPIO_Port GPIOA
#define DISP_RED_Pin GPIO_PIN_2
#define DISP_RED_GPIO_Port GPIOA
#define IRLASER_Pin GPIO_PIN_5
#define IRLASER_GPIO_Port GPIOA
#define DISP_GRN_Pin GPIO_PIN_6
#define DISP_GRN_GPIO_Port GPIOA
#define DISP_BLU_Pin GPIO_PIN_7
#define DISP_BLU_GPIO_Port GPIOA
#define FINGER_Pin GPIO_PIN_0
#define FINGER_GPIO_Port GPIOB
#define SW4_Pin GPIO_PIN_7
#define SW4_GPIO_Port GPIOE
#define GREEN_LASER_Pin GPIO_PIN_9
#define GREEN_LASER_GPIO_Port GPIOE
#define GRN_Pin GPIO_PIN_12
#define GRN_GPIO_Port GPIOB
#define BLU_Pin GPIO_PIN_13
#define BLU_GPIO_Port GPIOB
#define RED_Pin GPIO_PIN_14
#define RED_GPIO_Port GPIOB
#define LD4_Pin GPIO_PIN_12
#define LD4_GPIO_Port GPIOD
#define DISP_LED13_Pin GPIO_PIN_13
#define DISP_LED13_GPIO_Port GPIOD
#define DISP_LED12_Pin GPIO_PIN_14
#define DISP_LED12_GPIO_Port GPIOD
#define DISP_LED11_Pin GPIO_PIN_15
#define DISP_LED11_GPIO_Port GPIOD
#define LOW_BATT_Pin GPIO_PIN_10
#define LOW_BATT_GPIO_Port GPIOA
#define DISP_LED10_Pin GPIO_PIN_11
#define DISP_LED10_GPIO_Port GPIOC
#define RF_PWR_CT_Pin GPIO_PIN_1
#define RF_PWR_CT_GPIO_Port GPIOD
#define KEEPON_Pin GPIO_PIN_2
#define KEEPON_GPIO_Port GPIOD
#define BATT_MSR_EN_Pin GPIO_PIN_3
#define BATT_MSR_EN_GPIO_Port GPIOD
#define DISP_LED9_Pin GPIO_PIN_4
#define DISP_LED9_GPIO_Port GPIOD
#define DISP_LED8_Pin GPIO_PIN_5
#define DISP_LED8_GPIO_Port GPIOD
#define DISP_LED7_Pin GPIO_PIN_6
#define DISP_LED7_GPIO_Port GPIOD
#define DISP_LED6_Pin GPIO_PIN_7
#define DISP_LED6_GPIO_Port GPIOD
#define DISP_LED5_Pin GPIO_PIN_3
#define DISP_LED5_GPIO_Port GPIOB
#define DISP_LED4_Pin GPIO_PIN_4
#define DISP_LED4_GPIO_Port GPIOB
#define DISP_LED3_Pin GPIO_PIN_5
#define DISP_LED3_GPIO_Port GPIOB
#define DISP_LED2_Pin GPIO_PIN_7
#define DISP_LED2_GPIO_Port GPIOB
#define DISP_LED1_Pin GPIO_PIN_8
#define DISP_LED1_GPIO_Port GPIOB
#define SWA_Pin GPIO_PIN_0
#define SWA_GPIO_Port GPIOE
#define SWB_Pin GPIO_PIN_1
#define SWB_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
