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
#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

T_TaskTCB Tarea1;
T_TaskTCB Tarea2;
T_TaskTCB Tarea3;
T_TaskTCB Tarea4;
u32 StackTarea1[256];
u32 StackTarea2[256];
u32 StackTarea3[256];
u32 StackTarea4[256];

T_MutexHandler Mutex;

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
//#define GPIOB_BSRR 					(*( ( volatile u32 * ) 0x40011010 ) )
//#define GPIOB_BSRR_BS14				( 1UL << 14UL )
//#define GPIOB_BSRR_BR1				( 1UL << 30UL )

//#define GPIOB_ODR					(*( ( volatile u32 * ) 0x40010C0C ) )
#define GPIOB_ODR					(*( ( volatile u32 * ) 0x40010C08 ) )
#define B13_CLEAR_AND				( 1UL << 13 )
#define B14_CLEAR_AND				( 1UL << 14 )

#define GPIOC_ODR					(*( ( volatile u32 * ) 0x40011008 ) )



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void FunctionTarea1 (void);
void FunctionTarea2 (void);
void FunctionTarea3 (void);
void FunctionTarea4 (void);
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
	//Port_InitTickTimer();
	Task_Create(&Tarea1, 1, "Tarea1",1, (pu32)StackTarea1, 256, FunctionTarea1);
	Task_Create(&Tarea2, 2, "Tarea2",1, (pu32)StackTarea2, 256, FunctionTarea2);
	Task_Create(&Tarea3, 3, "Tarea3",1, (pu32)StackTarea3, 256, FunctionTarea3);
	Task_Create(&Tarea4, 4, "Tarea4",2, (pu32)StackTarea4, 256, FunctionTarea4);


	*RCC_APB2ENR |= RCC_APB2ENR_IOPC_EN |RCC_APB2ENR_IOPB_EN;
	GPIOC_CRH |= GPIOC_CRH_MODER13;// | GPIOC_CRH_CNF13; // Colector abierto revisar para entregable 1
	GPIOC_BSRR |= GPIOC_BSRR_BS13; // Encender LED

	//*RCC_APB2ENR |= RCC_APB2ENR_IOPC_EN;
	GPIOC_CRH |= GPIOC_CRH_MODER14;// | GPIOC_CRH_CNF13; // Colector abierto revisar para entregable 1
	GPIOC_BSRR |= GPIOC_BSRR_BR14; // Encender LED

	//*RCC_APB2ENR |= RCC_APB2ENR_IOPB_EN;
	GPIOB_CRH |= GPIOB_CRH_MODER13;// | GPIOB_CRH_CNF12; // Colector abierto revisar para entregable 1
	GPIOC_BSRR |= GPIOB_BSRR_BS13;	// Encender LED


	//Habilitar puerto como in para boton
	GPIOB_CRH &= GPIOB_CRH_MODER8;
	GPIOB_CRH &= ~(3UL<<2UL);
	GPIOB_CRH |= GPIOB_CRH_CNF8;

	GPIOB_CRH |= GPIOB_CRH_MODER12; //habilitar como salida B12



	RTOS_Start();

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
  /* USER CODE BEGIN 2 */

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

/* USER CODE BEGIN 4 */
void FunctionTarea1(void){
	for(;;)
	{
		if( ( GPIOC_ODR & B13_CLEAR_AND ) == 0 )
			GPIOC_BSRR |= GPIOC_BSRR_BS13;
		else
			GPIOC_BSRR |= GPIOC_BSRR_BR13;
		Task_Sleep(1000);
		// LED 1 - 0.5 HZ

		//Task_Sleep(500);
	}
}
void FunctionTarea2(void){
	for(;;)
	{
		if( ( GPIOC_ODR & B14_CLEAR_AND ) == 0 )
			GPIOC_BSRR |= GPIOC_BSRR_BS14;
		else
			GPIOC_BSRR |= GPIOC_BSRR_BR14;
		Task_Sleep(500);
		// LED 2 - 1 HZ
	}
}
void FunctionTarea3(void){
	for(;;)
	{
		if( ( GPIOB_ODR & B13_CLEAR_AND ) == 0 )
			GPIOB_BSRR |= GPIOB_BSRR_BS13;
		else
			GPIOB_BSRR |= GPIOB_BSRR_BR13;
		Task_Sleep(100);
		// LED 3 - 5HZ
	}
}
void FunctionTarea4(void){
	for(;;)
	{
		Task_Sleep(20);
		if((GPIOB_IDR & GPIOB_IDR_8) == 0){ // Preguntar si boton. Si se oprime no entra al if
			GPIOB_BSRR |= GPIOB_BSRR_BR12;
		}
		else
			GPIOB_BSRR |= GPIOB_BSRR_BS12;
		//Task_CheckTimeBlockedTask();
		//Leer entrada digital
		// Encender o apagar led
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
