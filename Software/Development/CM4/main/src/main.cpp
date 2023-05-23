/**
 ******************************************************************************
 * @file    HSEM/HSEM_CoreSync/CM4/Src/main.c
 * @author  MCD Application Team
 * @brief   This is the main program for Cortex-M4
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

#include "FreeRTOS.h"
#include "task.h"
#include "message_buffer.h"

#include "ProfilingTask.h"
#include "ICMS.h"

/** @addtogroup STM32H7xx_HAL_Examples
 * @{
 */

/** @addtogroup Templates
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
volatile uint32_t Notif_Recieved;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Main program
 * @param  None
 * @retval None
 */
int main(void)
{

  /*HW semaphore Clock enable*/
  __HAL_RCC_HSEM_CLK_ENABLE();

  /* Activate HSEM notification for Cortex-M4*/
  HAL_HSEM_ActivateNotification(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));

  /*
    Domain D2 goes to STOP mode (Cortex-M4 in deep-sleep) waiting for Cortex-M7 to
    perform system initialization (system clock config, external memory configuration.. )
  */
  HAL_PWREx_ClearPendingEvent();
  HAL_PWREx_EnterSTOPMode(PWR_MAINREGULATOR_ON, PWR_STOPENTRY_WFE, PWR_D2_DOMAIN);

  /* From here, CM7 has woken up the CM4 core */

  taskDISABLE_INTERRUPTS();

  /* Clear HSEM flag */
  __HAL_HSEM_CLEAR_FLAG(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));

  /* STM32H7xx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
         can eventually implement his proper time base source (a general purpose
         timer for example or other time source), keeping in mind that Time base
         duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
         handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
  */
  HAL_Init();

  /* Initialize LED 3 */
  BSP_LED_Init(LED3);

  while(1)
  {
    
  }

  ICMS::Init();

  /*Add CM4 Job here */

  /* AIEC Common configuration: make CPU1 and CPU2 SWI line1 sensitive to
  rising edge. */
  HAL_EXTI_EdgeConfig(EXTI_LINE1, EXTI_RISING_EDGE);

  /* ICMS Module has to be initialized by now */
  if(!ICMS::IsInitialized())
  {
    BSP_LED_On(LED3);
  }
  

  /* Interrupt used for M7 to M4 notifications. */
  ICMS::SetInterrupts(ICMS::Core::CM4);

  /* FreeRTOS Initialization */
  xTaskCreate(Tasks::ProfilingTask, "Profiling Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
  

  /* Tasks are created, signal CM7 that we are ready */
  /* CM4 takes HW semaphore 0 to inform CM7 that he finished his job */
  HAL_HSEM_FastTake(HSEM_ID_0);
  /* Do not forget to release the HW semaphore 0 once needed */
  HAL_HSEM_Release(HSEM_ID_0, 0);

  

  /* Now we start the scheduler */
  vTaskStartScheduler();

  /* Infinite loop */
  while (1)
  {
  }
}

#ifdef USE_FULL_ASSERT

/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, uint32_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
/* If the buffers to be provided to the Idle task are declared inside this
function then they must be declared static - otherwise they will be allocated on
the stack and so not exists after this function exits. */
static StaticTask_t xIdleTaskTCB;
static uint32_t uxIdleTaskStack[ configMINIMAL_STACK_SIZE ];

	/* configUSE_STATIC_ALLOCATION is set to 1, so the application must provide
	an implementation of vApplicationGetIdleTaskMemory() to provide the memory
	that is used by the Idle task.
	https://www.freertos.org/a00110.html#configSUPPORT_STATIC_ALLOCATION */

	/* Pass out a pointer to the StaticTask_t structure in which the Idle task's
	state will be stored. */
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

	/* Pass out the array that will be used as the Idle task's stack. */
	*ppxIdleTaskStackBuffer = uxIdleTaskStack;

	/* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
	Note that, as the array is necessarily of type StackType_t,
	configMINIMAL_STACK_SIZE is specified in words, not bytes. */
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}
