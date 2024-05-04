/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "timing.h"
#include "states.h"
#include "SFTD_states.h"
#include "system.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
uint32_t pulse_p = 10;
uint32_t counter = 0;

/* USER CODE BEGIN PV */
TIM_HandleTypeDef htim6;															//NEW CODE ADDED...1/6/24 FROM T10_x1-TIMER_EX3_COPY1
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim5;
uint32_t pulse1_value = 21000;//500Hz
uint32_t ccr_content;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM5_Init(void);
void tim6_init(void);																//NEW CODE ADDED...1/6/24 FROM T10_x1-TIMER_EX3_COPY1
void tim2_init(void);
void tim5_init(void);
void FIRE_LASER(uint32_t pulse_length);
uint16_t test(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  //tim6_init();
  //tim2_init();
  tim5_init();

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  /* USER CODE BEGIN 2 */
  MX_TIM2_Init();
  //MX_TIM5_Init();
  sftdStateInit();


  if((HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2)!= HAL_OK))
  {
	  Error_Handler();
  }

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  //uint16_t brightness = 0;
  keepOnTest();
  while (1)
  {
    /* USER CODE END WHILE */
	 sftdStateMonitor();

    /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
//			BEGINNING OF												THE ORIGINAL CODE FOR CLOCK CONFIG								//
		  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
		  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

		  /** Configure the main internal regulator output voltage
		  */
		  __HAL_RCC_PWR_CLK_ENABLE();
		  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

		  /** Initializes the RCC Oscillators according to the specified parameters
		  * in the RCC_OscInitTypeDef structure.
		  */
		  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
		  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
		  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
		  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
		  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		  {
			Error_Handler();
		  }

		  /** Initializes the CPU, AHB and APB buses clocks
		  */
		  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
									  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
		  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
		  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
		  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
		  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

		  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
		  {
			Error_Handler();
		  }
}

		/**
		  * @brief TIM2 Initialization Function
		  * @param None
		  * @retval None
		  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 4294967295;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
  {
	Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
	Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
	Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);
//			END OF												THE ORIGINAL CODE FOR CLOCK CONFIG										//
/***************************************************************************************************************************************/
//			BEGINNING OF										FROM T10_x1-TIMER_EX3_COPY1 FOR CLOCK CONFIG  						   //
/*		  	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
		    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

		    // Configure the main internal regulator output voltage

		    __HAL_RCC_PWR_CLK_ENABLE();
		    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

		    //Initializes the RCC Oscillators according to the specified parameters
		    // in the RCC_OscInitTypeDef structure.

		    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
		    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
		    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
		    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
		    RCC_OscInitStruct.PLL.PLLM = 8;
		    RCC_OscInitStruct.PLL.PLLN = 336;
		    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
		    RCC_OscInitStruct.PLL.PLLQ = 7;
		    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
		    {
		      Error_Handler();
		    }

		    // Initializes the CPU, AHB and APB buses clocks

		    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
		                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
		    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;//RCC_SYSCLKSOURCE_PLLCLK;
		    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
		    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
		    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

		    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
		    {
		      Error_Handler();
		    }
		    */
//			END OF												FROM T10_x1-TIMER_EX3_COPY1 FOR CLOCK CONFIG  						   //
/***************************************************************************************************************************************/

}

/**
  * @brief TIM5 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM5_Init(void)
{

  /* USER CODE BEGIN TIM5_Init 0 */

  /* USER CODE END TIM5_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM5_Init 1 */

  /* USER CODE END TIM5_Init 1 */
  htim5.Instance = TIM5;
  htim5.Init.Prescaler = 0;
  htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim5.Init.Period = 4294967295;
  htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim5.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_OC_Init(&htim5) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim5, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim5, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM5_Init 2 */

  /* USER CODE END TIM5_Init 2 */
  HAL_TIM_MspPostInit(&htim5);

}



/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  //HAL_GPIO_WritePin(GPIOA, DISP_RED_Pin|IRLASER_Pin|DISP_GRN_Pin|DISP_BLU_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, FLASH_Pin|DISP_RED_Pin|IRLASER_Pin|DISP_GRN_Pin
                          |DISP_BLU_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, BLU_Pin|RED_Pin|DISP_LED5_Pin
                          |DISP_LED4_Pin|DISP_LED3_Pin|DISP_LED2_Pin|DISP_LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, DISP_LED13_Pin|DISP_LED12_Pin|DISP_LED11_Pin|GREEN_LASER_Pin
                          |KEEPON_Pin|BATT_MSR_EN_Pin|DISP_LED9_Pin|LD4_Pin
                          |DISP_LED8_Pin|DISP_LED7_Pin|DISP_LED6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DISP_LED10_GPIO_Port, DISP_LED10_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SWC_Pin SWD_Pin PWR_MON_Pin MODE_Pin
                           SW5_Pin SW6_Pin SWA_Pin SWB_Pin */
  GPIO_InitStruct.Pin = SWC_Pin|SWD_Pin|PWR_MON_Pin|MODE_Pin
                          |SW5_Pin|SW6_Pin|SWA_Pin|SWB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GRN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = RF_PWR_CT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);


  /*Configure GPIO pin : TRIGGER_Pin */
  GPIO_InitStruct.Pin = TRIGGER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TRIGGER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : FLASH_Pin DISP_RED_Pin IRLASER_Pin DISP_GRN_Pin
                           DISP_BLU_Pin */
  //GPIO_InitStruct.Pin = FLASH_Pin|DISP_RED_Pin|IRLASER_Pin|DISP_GRN_Pin
  //                        |DISP_BLU_Pin;
  GPIO_InitStruct.Pin = DISP_RED_Pin|IRLASER_Pin|DISP_GRN_Pin|DISP_BLU_Pin;//removed |FLASH 1/10/24 to test PWM function on A1
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : FINGER_Pin */
  GPIO_InitStruct.Pin = FINGER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(FINGER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : GRN_Pin BLU_Pin RED_Pin DISP_LED5_Pin
                           DISP_LED4_Pin DISP_LED3_Pin DISP_LED2_Pin DISP_LED1_Pin */
  GPIO_InitStruct.Pin = BLU_Pin|RED_Pin|DISP_LED5_Pin
                          |DISP_LED4_Pin|DISP_LED3_Pin|DISP_LED2_Pin|DISP_LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : DISP_LED13_Pin DISP_LED12_Pin DISP_LED11_Pin GREEN_LASER_Pin
                           RF_PWR_CT_Pin KEEPON_Pin BATT_MSR_EN_Pin DISP_LED9_Pin
                           DISP_LED8_Pin DISP_LED7_Pin DISP_LED6_Pin */
  GPIO_InitStruct.Pin = DISP_LED13_Pin|DISP_LED12_Pin|DISP_LED11_Pin|GREEN_LASER_Pin
                          |KEEPON_Pin|BATT_MSR_EN_Pin|DISP_LED9_Pin|LD4_Pin
                          |DISP_LED8_Pin|DISP_LED7_Pin|DISP_LED6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : LOW_BATT_Pin */
  GPIO_InitStruct.Pin = LOW_BATT_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(LOW_BATT_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : DISP_LED10_Pin */
  GPIO_InitStruct.Pin = DISP_LED10_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(DISP_LED10_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
/***************************************************************************************************************************************/
//			BEGINNING OF										FROM T10_x1-TIMER_EX3_COPY1 FOR TIMER6 CONFIG 1/6/24				   //
void tim6_init(void)
{
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 24;
	//htim6.Init.Period = (2560-1);//2560 = 4mS
	//htim6.Init.Period = (8960-1);//8960 = 14mS
	//htim6.Init.Period = (15360-1);//15360 = 24mS
	htim6.Init.Period = (28160-1);//28160 = 44mS
	if(HAL_TIM_Base_Init(&htim6) != HAL_OK)
	{
		Error_Handler();
	}

}
//			END       OF										FROM T10_x1-TIMER_EX3_COPY1 FOR TIMER6 CONFIG  						   //
/***************************************************************************************************************************************/
//			BEGINNING OF										FROM T10_x1-TIMER_EX3_COPY1 FOR TIMER6 CALLBACK 1/6/24				   //
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	pulse_p = 0;
	if(counter++> pulse_p)
	{
		HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, RESET);
		HAL_GPIO_WritePin(IRLASER_GPIO_Port, IRLASER_Pin, RESET);
		HAL_TIM_Base_Stop(&htim6);
		pulse_p = 0;
		counter = 0;
	}
}
//			END       OF										FROM T10_x1-TIMER_EX3_COPY1 FOR TIMER6 CALLBACK						   //
/***************************************************************************************************************************************/

void FIRE_LASER(uint32_t pulse_length)
{
	//1. turn on laser
	HAL_GPIO_WritePin(IRLASER_GPIO_Port, IRLASER_Pin, SET);
	//HAL_GPIO_WritePin(LD4_GPIO_Port, LD4_Pin, SET);

	//2. start timer
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 24;
	htim6.Init.Period = (pulse_length-1);
	if(HAL_TIM_Base_Init(&htim6) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_TIM_Base_Start_IT(&htim6);
}


void tim2_init(void)
{
	TIM_OC_InitTypeDef tim2PWM_Config;

	htim2.Instance = TIM2;
	htim2.Init.Period = 7000;
	htim2.Init.Prescaler = 5;
	if(HAL_TIM_PWM_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}

	tim2PWM_Config.OCMode = TIM_OCMODE_PWM1;
	tim2PWM_Config.OCPolarity = TIM_OCPOLARITY_LOW;

	//tim2PWM_Config.Pulse = (htim2.Init.Period*5)/100;
	tim2PWM_Config.Pulse = 0;
	if((HAL_TIM_PWM_ConfigChannel(&htim2, &tim2PWM_Config, TIM_CHANNEL_1) != HAL_OK))
	{
		Error_Handler();
	}
	//HAL_TIM_PWM_MspInit(&htim5);
}

void tim5_init(void)
{
	TIM_OC_InitTypeDef tim5PWM_Config;

	htim5.Instance = TIM5;
	htim5.Init.Period = 7000;
	htim5.Init.Prescaler = 5;
	if(HAL_TIM_PWM_Init(&htim5) != HAL_OK)
	{
		Error_Handler();
	}

	tim5PWM_Config.OCMode = TIM_OCMODE_PWM1;
	tim5PWM_Config.OCPolarity = TIM_OCPOLARITY_LOW;

	tim5PWM_Config.Pulse = 0;
	if((HAL_TIM_PWM_ConfigChannel(&htim5, &tim5PWM_Config, TIM_CHANNEL_2) != HAL_OK))
	{
		Error_Handler();
	}
}

void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim)
{
	/*
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
		ccr_content = HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_1,ccr_content+pulse1_value);
	}*/
	if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)
	{
		ccr_content = HAL_TIM_ReadCapturedValue(&htim5, TIM_CHANNEL_2);
		__HAL_TIM_SET_COMPARE(htim, TIM_CHANNEL_2,ccr_content+pulse1_value);
	}
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
