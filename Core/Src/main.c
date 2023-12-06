/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdint.h>
#include "Rtosinclude.h"
#include "Queue.h"
#include "Mutex.h"
#include "Events.h"
#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

T_TaskTCB Buzzer;
T_TaskTCB Buzzer2;
T_TaskTCB Mov;
T_TaskTCB Display;
T_TaskTCB PreGame;

u32 StackBuzzer[256];
u32 StackBuzzer2[256];
u32 StackMov[256];
u32 StackDisplay[256];
u32 StackPreGame[256];

u16 x = 10;
u16 y = 10;
u16 R0y = 1;
u16 R1y = 10;
u16 L0y = 20;
u16 L1y = 31;
s8 MarcP1 	= 0;
s8 MarcP2	= 0;

T_MutexHandler_Ptr Mutex_Display;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define RCC_APB2ENR					(( ( volatile u32 * ) 0xe40021018 ) )
#define RCC_APB2ENR_IOPC_EN			( 1UL << 4UL )

#define RCC_APB2ENR_IOPB_EN			( 1UL << 3UL )
#define GPIOB_CRL					(*( ( volatile u32 * ) 0x40010C00 ) )
#define GPIOB_CRH					(*( ( volatile u32 * ) 0x40010C04 ) )
#define GPIOB_CRH_MODER12			( 3UL << 16UL )
#define GPIOB_CRH_CNF12				( 1UL << 18UL )
#define GPIOB_BSRR 					(*( ( volatile u32 * ) 0x40010C10 ) )
#define GPIOB_BSRR_BS12				( 1UL << 12UL )
#define GPIOB_BSRR_BR12				( 1UL << 28UL )

#define GPIOB_CRL					(*( ( volatile u32 * ) 0x40010C00 ) )
#define GPIOB_CRH					(*( ( volatile u32 * ) 0x40010C04 ) )
#define GPIOB_CRH_MODER13			( 3UL << 20UL )
#define GPIOB_CRH_CNF13				( 3UL << 22UL )
#define GPIOB_BSRR 					(*( ( volatile u32 * ) 0x40010C10 ) )
#define GPIOB_BSRR_BS13				( 1UL << 13UL )
#define GPIOB_BSRR_BR13				( 1UL << 29UL )

//
#define GPIOC_CRH_MODER14			( 3UL << 24UL )
#define GPIOC_CRH_CNF14				( 3UL << 26UL )
#define GPIOC_BSRR 					(*( ( volatile u32 * ) 0x40011010 ) )
#define GPIOC_BSRR_BS14				( 1UL << 14UL )
#define GPIOC_BSRR_BR14				( 1UL << 30UL )

#define GPIOB_CRH_MODER14			~( 3UL << 24UL )
#define GPIOB_CRH_CNF14				( 2UL << 26UL )
#define GPIOB_IDR					(*( ( volatile u32 * ) 0x40010C08 ) )
#define GPIOB_IDR_14				(1UL << 14UL )

#define GPIOB_CRH_MODER8			~( 3UL )
#define GPIOB_CRH_CNF8				( 2UL << 2UL )
#define GPIOB_IDR					(*( ( volatile u32 * ) 0x40010C08 ) )
#define GPIOB_IDR_8					(1UL << 8UL )
#define GPIOB_ODR					(*( ( volatile u32 * ) 0x40010C08 ) )
#define B13_CLEAR_AND				( 1UL << 13 )
#define B14_CLEAR_AND				( 1UL << 14 )

#define GPIOC_ODR					(*( ( volatile u32 * ) 0x40011008 ) )


#define TEST1						0x0001
#define START						0x1000

#define RightUp						GPIO_PIN_14
#define	RightDown					GPIO_PIN_8
#define LeftDown					GPIO_PIN_9
#define LeftUp						GPIO_PIN_15


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */

T_EventHanlder Evento;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
void FunctionPreGame(void);
void FunctionBuzzer (void);
void FunctionBuzzer2 (void);
void FunctionMov(void);
void FunctionTarea6 (void);
void FunctionDisplay (void);

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

	RTOS_Init();

	Task_Create(&Buzzer,	1, "Buzzer",	2, (pu32)StackBuzzer,	 256,	 FunctionBuzzer);
	Task_Create(&Buzzer2,	2, "Buzzer2",	2, (pu32)StackBuzzer2,	 256,	 FunctionBuzzer2);
	Task_Create(&Mov,		3, "Mov",		3, (pu32)StackMov,		 256,	 FunctionMov);
	Task_Create(&Display,	4, "Display",	3, (pu32)StackDisplay,	 256,	 FunctionDisplay);
	Task_Create(&PreGame,	5, "PreGame",	1, (pu32)StackPreGame,	 256,	 FunctionPreGame);


	Mutex_Init(Mutex_Display);



  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	ssd1306_Init();

	ssd1306_SetColor(White);

	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);


	RTOS_Start();
  /* USER CODE END 2 */

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
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB12 PB13 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB14 PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA9 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void FunctionPreGame(void)
{
	ssd1306_Clear();

	ssd1306_DrawLine(1, 1, SSD1306_WIDTH - 1, 1);
	ssd1306_DrawLine(1, SSD1306_HEIGHT - 1, 1, 1);
	ssd1306_DrawLine(SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, 1, SSD1306_HEIGHT - 1);
	ssd1306_DrawLine(SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, SSD1306_WIDTH - 1, 1);

	ssd1306_DrawLine(5,L0y,5,L1y);
	ssd1306_DrawLine(3,L0y,3,L1y);
	ssd1306_DrawLine(4,L0y,4,L1y);

	ssd1306_DrawLine(SSD1306_WIDTH - 5,R0y,SSD1306_WIDTH - 5,R1y);
	ssd1306_DrawLine(SSD1306_WIDTH - 3,R0y,SSD1306_WIDTH - 3,R1y);
	ssd1306_DrawLine(SSD1306_WIDTH - 4,R0y,SSD1306_WIDTH - 4,R1y);

	ssd1306_SetCursor(15, 15);
	ssd1306_WriteChar(*"Pulse el boton",Font_7x10);
	ssd1306_SetCursor(15, 25);
	ssd1306_WriteChar(*"Para iniciar",Font_7x10);
	ssd1306_UpdateScreen();

	while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 0);

	Events_Set(&Evento, START);


}

void FunctionBuzzer(void){
	Events_WaitAll(&Evento, START);
	for(;;)
	{
		Events_WaitAny(&Evento, 0x001);
		Events_Clear(&Evento, 0x001);
		Events_Clear(&Evento, 0x004);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
		Task_Sleep(200);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
		Task_Sleep(200);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
		Task_Sleep(200);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
		Task_Sleep(200);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
		Events_Set(&Evento, 0x004);
		// LED 3 - 5HZ
	}
}
void FunctionBuzzer2(void){
	Events_WaitAll(&Evento, START);
	for(;;)
	{
		Events_WaitAny(&Evento, 0x002);
		Events_Clear(&Evento, 0x002);
		//Events_Clear(&Evento, 0x004);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
		Task_Sleep(500);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
		//Events_Set(&Evento, 0x004);

		//Task_CheckTimeBlockedTask();
		//Leer entrada digital
		// Encender o apagar led
	}
}

void FunctionMov(void){
	Events_WaitAll(&Evento, START);
	u8 dirID	= 0;
	u8 dirUPD 	= 0;


	dirID 	= 0; //izquierda
	dirUPD 	= 0; //UP

	Events_Set(&Evento, 0x004);

	for(;;)
	{
		if(dirID == 0)x++;
		else x--;
		if(SSD1306_WIDTH - 1 == x)
		{
			dirID = 1;
			x = SSD1306_WIDTH/2;
			y = SSD1306_HEIGHT/2;
			MarcP1++;
			if(MarcP1 == 10)
			{
				MarcP2 = 0;
				MarcP1 =0;
			}
			Events_Set(&Evento,0x001);
			Task_Sleep(600);
		}
		if(x == 1)
		{
			dirID = 0;
			x = SSD1306_WIDTH/2;
			y = SSD1306_HEIGHT/2;
			MarcP2++;
			if(MarcP2 == 10){
				MarcP2 = 0;
				MarcP1 =0;
			}
			Events_Set(&Evento,0x001);
			Task_Sleep(600);
		}
		if(x == 5 && y >= L0y && y <=L1y){
			dirID = 0;
			Events_Set(&Evento,0x002);
		}
		if(x == SSD1306_WIDTH - 5 && y >= R0y && y <=R1y)
		{
			dirID = 1;
			Events_Set(&Evento,0x002);
		}

		// Esperar por mensaje
		//Dormir tarea
		if((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_14) == 1) && (R1y != SSD1306_HEIGHT - 1) ){R0y++;R1y++;}
		if((HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_8) == 1) && (R0y != 1) ){R0y--;R1y--;}

		if((HAL_GPIO_ReadPin(GPIOA, LeftDown) == 1) && (L1y != SSD1306_HEIGHT - 1)) {L0y++; L1y++;}
		if((HAL_GPIO_ReadPin(GPIOA, LeftUp) == 1) && (L0y != 1) ){L0y--;L1y--;}

		//Luego de recibir mensaje, continuar con codigo

		if(dirUPD == 0)y++;
		else y--;
		if(SSD1306_HEIGHT - 1 == y) dirUPD = 1;
		if(y == 1 ) dirUPD = 0;

		Task_Sleep(1);
		ssd1306_UpdateScreen();


		// Fin mutex de display
		Mutex_Give(Mutex_Display);


		Events_WaitAny(&Evento, 0x004);
		Task_Sleep(1);

		//Mutex de display
		Mutex_Take(Mutex_Display);
	}
}

void FunctionTarea6(void)
{
	Events_WaitAll(&Evento, START);
	while(1)
	{
		if((HAL_GPIO_ReadPin(GPIOB, RightUp) == 1) && (R1y != SSD1306_HEIGHT - 1) ){R0y++;R1y++;}
		if((HAL_GPIO_ReadPin(GPIOB, RightDown) == 1) && (R0y != 1) ){R0y--;R1y--;}

		if((HAL_GPIO_ReadPin(GPIOA, LeftUp) == 1) && (L1y != SSD1306_HEIGHT - 1)) {L0y++; L1y++;}
		if((HAL_GPIO_ReadPin(GPIOA, LeftDown) == 1) && (L0y != 1) ){L0y--;L1y--;}

		// mandar informacion por un mensaje a la tarea 4
		Task_Sleep(1);

	}
}

void FunctionDisplay(void)
{
	Events_WaitAll(&Evento, START);
	while(1)
	{

		//Mutex de display
		Mutex_Take(Mutex_Display);

		ssd1306_Clear();

		ssd1306_DrawLine(1, 1, SSD1306_WIDTH - 1, 1);
		ssd1306_DrawLine(1, SSD1306_HEIGHT - 1, 1, 1);
		ssd1306_DrawLine(SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, 1, SSD1306_HEIGHT - 1);
		ssd1306_DrawLine(SSD1306_WIDTH - 1, SSD1306_HEIGHT - 1, SSD1306_WIDTH - 1, 1);

		ssd1306_DrawLine(5,L0y,5,L1y);
		ssd1306_DrawLine(3,L0y,3,L1y);
		ssd1306_DrawLine(4,L0y,4,L1y);

		ssd1306_DrawLine(SSD1306_WIDTH - 5,R0y,SSD1306_WIDTH - 5,R1y);
		ssd1306_DrawLine(SSD1306_WIDTH - 3,R0y,SSD1306_WIDTH - 3,R1y);
		ssd1306_DrawLine(SSD1306_WIDTH - 4,R0y,SSD1306_WIDTH - 4,R1y);

		ssd1306_SetCursor(50, 15);
		ssd1306_WriteChar(MarcP1+'0',Font_7x10);
		ssd1306_SetCursor(SSD1306_WIDTH - 50, 15);
		ssd1306_WriteChar(MarcP2+'0',Font_7x10);

		ssd1306_DrawPixel(x, y);

		Mutex_Give(Mutex_Display);

		Task_Sleep(10);

		//Fin  mutex de display

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
