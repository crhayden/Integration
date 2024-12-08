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
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "timing.h" 
#include "states.h" 
#include "SFTD_states.h" 
#include "system.h" 
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
ADC_HandleTypeDef hadc1;

I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_tx;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;

/* Definitions for stateMonitorTas */
osThreadId_t stateMonitorTasHandle;
const osThreadAttr_t stateMonitorTas_attributes = {
  .name = "stateMonitorTas",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE BEGIN PV */
TIM_HandleTypeDef htim6;															//NEW CODE ADDED...1/6/24 FROM T10_x1-TIMER_EX3_COPY1
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim5;
uint32_t pulse1_value = 21000;//500Hz
uint32_t ccr_content;
uint32_t pulse_p = 10; 
uint32_t counter = 0; 

uint16_t laserPulse = 0;
#if MILO_ENABLED
volatile uint16_t laser_pulses[10] = {0,1306,4571,7837,14367,0,0,0,0,0,0};//MILO Pulses
#elif VIRTRA_ENABLED
volatile uint32_t laser_pulses[10] = {0,12931,18384,23804,29257,34612,40163,45714,50939,56490};//VIRTRA Pulses
#elif TI_ENABLED
volatile uint16_t laser_pulses[10] = {0,2612,14367,24261,35265,47020,58776,0,0,0};//TI Pulses
#elif LASER_AMO
#define LAZER_TARGET_PRESCALER	24
#define LAZER_TARGET_38KHZ_ARR	6
#define LAZER_TARGET_38KHZ_TOGGLE_COUNT 152
#define LAZER_TARGET_LOW_ARR 275
#define LAZER_TARGET_HIGH_ARR 224
#define LAZER_TARGET_LOW_HIGH_TOGGLE_COUNT 18
volatile  int irToggleCount = 0;
#endif
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2S3_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM6_Init(void);
void StateMonitorTask(void *argument);

/* USER CODE BEGIN PFP */
void tim5_init(void); 
void FIRE_LASER();

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
  MX_DMA_Init();
  MX_I2S3_Init();
  MX_ADC1_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  SF_AudioInit();
  SF_BatteryInit();
  //MX_TIM5_Init();
  sftdStateInit();


  if((HAL_TIM_PWM_Start(&htim5, TIM_CHANNEL_2)!= HAL_OK))
  {
	  Error_Handler();
  }

  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of stateMonitorTas */
  stateMonitorTasHandle = osThreadNew(StateMonitorTask, NULL, &stateMonitorTas_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Macro to configure the PLL multiplication factor
  */
  __HAL_RCC_PLL_PLLM_CONFIG(16);

  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSI);

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
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
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
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_3;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2S3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2S3_Init(void)
{

  /* USER CODE BEGIN I2S3_Init 0 */

  /* USER CODE END I2S3_Init 0 */

  /* USER CODE BEGIN I2S3_Init 1 */

  /* USER CODE END I2S3_Init 1 */
  hi2s3.Instance = SPI3;
  hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B_EXTENDED;
  hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_DISABLE;
  hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_44K;
  hi2s3.Init.CPOL = I2S_CPOL_LOW;
  hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
  hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
  if (HAL_I2S_Init(&hi2s3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2S3_Init 2 */

  /* USER CODE END I2S3_Init 2 */

}
/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
#if MILO_ENABLED
#elif VIRTRA_ENABLED
#elif TI_ENABLED
  htim6.Init.Prescaler = 48;
#elif LASER_AMO
  htim6.Init.Prescaler = 24;
#endif
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 65535;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */
  HAL_TIM_RegisterCallback(&htim6, HAL_TIM_PERIOD_ELAPSED_CB_ID, HAL_TIM_PeriodElapsedCallback);
  //HAL_TIM_Base_Start_IT(&htim6);
  /* USER CODE END TIM6_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

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
  HAL_GPIO_WritePin(GPIOA, FLASH_Pin|DISP_RED_Pin|DISP_GRN_Pin|DISP_BLU_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(IRLASER_GPIO_Port, IRLASER_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GREEN_LASER_GPIO_Port, GREEN_LASER_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GRN_Pin|BLU_Pin|RED_Pin|DISP_LED5_Pin
                          |DISP_LED4_Pin|DISP_LED3_Pin|DISP_LED2_Pin|DISP_LED1_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|DISP_LED13_Pin|DISP_LED12_Pin|DISP_LED11_Pin
                          |RF_PWR_CT_Pin|KEEPON_Pin|BATT_MSR_EN_Pin|DISP_LED9_Pin
                          |DISP_LED8_Pin|DISP_LED7_Pin|DISP_LED6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(DISP_LED10_GPIO_Port, DISP_LED10_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SWC_Pin SWD_Pin PWR_MON_Pin SW5_Pin
                           SW4_Pin SWA_Pin SWB_Pin */
  GPIO_InitStruct.Pin = SWC_Pin|SWD_Pin|PWR_MON_Pin|SW5_Pin
                          |SW4_Pin|SWA_Pin|SWB_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : GRN_CHARGE_Pin */
  GPIO_InitStruct.Pin = GRN_CHARGE_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GRN_CHARGE_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TRIGGER_Pin */
  GPIO_InitStruct.Pin = TRIGGER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(TRIGGER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : FLASH_Pin DISP_RED_Pin IRLASER_Pin DISP_GRN_Pin
                           DISP_BLU_Pin */
  GPIO_InitStruct.Pin = FLASH_Pin|DISP_RED_Pin|IRLASER_Pin|DISP_GRN_Pin
                          |DISP_BLU_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : FINGER_Pin */
  GPIO_InitStruct.Pin = FINGER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(FINGER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : GREEN_LASER_Pin */
  GPIO_InitStruct.Pin = GREEN_LASER_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GREEN_LASER_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : GRN_Pin BLU_Pin RED_Pin DISP_LED5_Pin
                           DISP_LED4_Pin DISP_LED3_Pin DISP_LED2_Pin DISP_LED1_Pin */
  GPIO_InitStruct.Pin = GRN_Pin|BLU_Pin|RED_Pin|DISP_LED5_Pin
                          |DISP_LED4_Pin|DISP_LED3_Pin|DISP_LED2_Pin|DISP_LED1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin DISP_LED13_Pin DISP_LED12_Pin DISP_LED11_Pin
                           RF_PWR_CT_Pin KEEPON_Pin BATT_MSR_EN_Pin DISP_LED9_Pin
                           DISP_LED8_Pin DISP_LED7_Pin DISP_LED6_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|DISP_LED13_Pin|DISP_LED12_Pin|DISP_LED11_Pin
                          |RF_PWR_CT_Pin|KEEPON_Pin|BATT_MSR_EN_Pin|DISP_LED9_Pin
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
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
  HAL_NVIC_SetPriorityGrouping( NVIC_PRIORITYGROUP_4 );
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/***************************************************************************************************************************************/
//			BEGINNING OF										FROM T10_x1-TIMER_EX3_COPYkl1 FOR TIMER6 CONFIG 1/6/24				   //

//			END       OF										FROM T10_x1-TIMER_EX3_COPY1 FOR TIMER6 CONFIG  						   //
/***************************************************************************************************************************************/
//			BEGINNING OF										FROM T10_x1-TIMER_EX3_COPY1 FOR TIMER6 CALLBACK 1/6/24				   //
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) 
{ 
#if MILO_ENABLED
#elif VIRTRA_ENABLED
#elif TI_ENABLED
    HAL_GPIO_WritePin(IRLASER_GPIO_Port, IRLASER_Pin, SET);
    HAL_TIM_Base_Stop_IT(&htim6);
#elif LASER_AMO
	irToggleCount++;

    if( irToggleCount < LAZER_TARGET_38KHZ_TOGGLE_COUNT) {
    	// Laser is in 38kHz pulse period
        HAL_GPIO_TogglePin(IRLASER_GPIO_Port, IRLASER_Pin);
    } else if (irToggleCount < (LAZER_TARGET_38KHZ_TOGGLE_COUNT + LAZER_TARGET_LOW_HIGH_TOGGLE_COUNT)){
    	// laser is high/low period
    	int high_low_count = irToggleCount - LAZER_TARGET_38KHZ_TOGGLE_COUNT;
    	if(high_low_count % 2){
    		HAL_GPIO_WritePin(IRLASER_GPIO_Port, IRLASER_Pin, GPIO_PIN_RESET);
    		laserPulse = LAZER_TARGET_LOW_ARR;
    		__HAL_TIM_CLEAR_FLAG(&htim6, TIM_FLAG_UPDATE);
    		__HAL_TIM_SET_AUTORELOAD(&htim6, laserPulse);
    	} else {
    		HAL_GPIO_WritePin(IRLASER_GPIO_Port, IRLASER_Pin, GPIO_PIN_SET);
    		laserPulse = LAZER_TARGET_HIGH_ARR;
    		__HAL_TIM_CLEAR_FLAG(&htim6, TIM_FLAG_UPDATE);
			__HAL_TIM_SET_AUTORELOAD(&htim6, laserPulse);
    	}
    } else {
    	// laser sequence is complete
    	HAL_GPIO_WritePin(IRLASER_GPIO_Port, IRLASER_Pin, GPIO_PIN_SET);
    	HAL_TIM_Base_Stop_IT(&htim6);
    }
#endif


} 
//			END       OF										FROM T10_x1-TIMER_EX3_COPY1 FOR TIMER6 CALLBACK						   //
/***************************************************************************************************************************************/

void FIRE_LASER()
{ 

#if MILO_ENABLED
#elif VIRTRA_ENABLED
#elif TI_ENABLED
	laserPulse = laser_pulses[getSwitch()];
#elif LASER_AMO
	irToggleCount = 0;
	laserPulse = LAZER_TARGET_38KHZ_ARR;
#endif
    //
    // Turn on the laser active low
    //          
    HAL_GPIO_WritePin(IRLASER_GPIO_Port, IRLASER_Pin, RESET);
    //
    // Load the new timer value
    //
    __HAL_TIM_CLEAR_FLAG(&htim6, TIM_FLAG_UPDATE);
    __HAL_TIM_SET_AUTORELOAD(&htim6, laserPulse);
    //
    // Start the timer
    //
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

/* USER CODE BEGIN Header_StateMonitorTask */
/**
* @brief Function implementing the stateMonitorTas thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StateMonitorTask */
void StateMonitorTask(void *argument)
{
  /* USER CODE BEGIN 5 */
    /* Infinite loop */
    for(;;) {

#if SIMULATED_ENABLED
        if (HAL_I2S_GetState(&hi2s3) == HAL_I2S_STATE_READY) {
#endif //SIMULATED_ENABLED
        	 sftdStateMonitor();
#if SIMULATED_ENABLED
        }
#endif //SIMULATED_ENABLED
        osDelay(10);
    }
  /* USER CODE END 5 */
}

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
