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

#define HSEM_ID_0 (0U) /* HW semaphore 0*/

volatile uint32_t Notif_Recieved;

static UART_HandleTypeDef xUARTHandle1;
static UART_HandleTypeDef xUARTHandle2;

static const uint8_t welcomeMessage1[] = "Hello There with UART 1!\n\r";
static const uint8_t welcomeMessage8[] = "Hello There with UART 8!\n\r";

static void WaitForCM7Wakeup(void);
static bool SyncedCoreInit(void);
static void UARTInit(void);

int main(void)
{

  /*
    The CM7 and CM4 both start on Power up.
    To ensure a correct Boot-Up, they both execute a synced
    Initialization.
  */
  bool init_successful = SyncedCoreInit();

  if (init_successful)
  {
    BSP_LED_On(LED3);
  }

  //HAL_UART_Transmit(&xUARTHandle1, (uint8_t *)welcomeMessage1, sizeof(welcomeMessage1), HAL_MAX_DELAY);
  //HAL_UART_Transmit(&xUARTHandle2, (uint8_t *)welcomeMessage8, sizeof(welcomeMessage8), HAL_MAX_DELAY);

  /* Now we start the scheduler */
  vTaskStartScheduler();

  /*
    Code from this point onwards should NEVER execute
  */
  configASSERT(0);
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

bool SyncedCoreInit(void)
{
  WaitForCM7Wakeup();

  __HAL_HSEM_CLEAR_FLAG(__HAL_HSEM_SEMID_TO_MASK(HSEM_ID_0));

  /* AIEC Common configuration: make CPU1 and CPU2 SWI line1 sensitive to
  rising edge. */
  HAL_EXTI_EdgeConfig(EXTI_LINE1, EXTI_RISING_EDGE);

  /* STM32 HAL Init */
  HAL_Init();

  //UARTInit();

  /* For now, CM4 uses the LED 3 on the board. */
  BSP_LED_Init(LED3);

  /* Interrupt used for M7 to M4 notifications. */
  ICMS::SetInterrupts(ICMS::Core::CM4);

  /* FreeRTOS needs disabled interrupts to do its init stuff */
  taskDISABLE_INTERRUPTS();

  /* FreeRTOS Initialization */
  xTaskCreate(Tasks::ProfilingTask, "Profiling Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

  /* Tasks are created, signal CM7 that we are ready */
  /* CM4 takes HW semaphore 0 to inform CM7 that he finished his job */
  HAL_HSEM_FastTake(HSEM_ID_0);
  /* Do not forget to release the HW semaphore 0 once needed */
  HAL_HSEM_Release(HSEM_ID_0, 0);

  return true;
}

void WaitForCM7Wakeup(void)
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
}

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, uint32_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
  /* If the buffers to be provided to the Idle task are declared inside this
  function then they must be declared static - otherwise they will be allocated on
  the stack and so not exists after this function exits. */
  static StaticTask_t xIdleTaskTCB;
  static uint32_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

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

void UARTInit(void)
{
  xUARTHandle1.Instance = USART3;
  xUARTHandle1.Init.BaudRate = 115200;
  xUARTHandle1.Init.WordLength = UART_WORDLENGTH_8B;
  xUARTHandle1.Init.StopBits = UART_STOPBITS_1;
  xUARTHandle1.Init.Parity = UART_PARITY_NONE;
  xUARTHandle1.Init.Mode = UART_MODE_TX_RX;
  xUARTHandle1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  xUARTHandle1.Init.OverSampling = UART_OVERSAMPLING_16;
  xUARTHandle1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  xUARTHandle1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  xUARTHandle1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&xUARTHandle1) != HAL_OK)
  {
    configASSERT(0);
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&xUARTHandle1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    configASSERT(0);
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&xUARTHandle1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    configASSERT(0);
  }
  if (HAL_UARTEx_DisableFifoMode(&xUARTHandle1) != HAL_OK)
  {
    configASSERT(0);
  }

  xUARTHandle2.Instance = UART8;
  xUARTHandle2.Init.BaudRate = 115200;
  xUARTHandle2.Init.WordLength = UART_WORDLENGTH_8B;
  xUARTHandle2.Init.StopBits = UART_STOPBITS_1;
  xUARTHandle2.Init.Parity = UART_PARITY_NONE;
  xUARTHandle2.Init.Mode = UART_MODE_TX_RX;
  xUARTHandle2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  xUARTHandle2.Init.OverSampling = UART_OVERSAMPLING_16;
  xUARTHandle2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  xUARTHandle2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  xUARTHandle2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&xUARTHandle2) != HAL_OK)
  {
    configASSERT(0);
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&xUARTHandle2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    configASSERT(0);
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&xUARTHandle2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    configASSERT(0);
  }
  if (HAL_UARTEx_DisableFifoMode(&xUARTHandle2) != HAL_OK)
  {
    configASSERT(0);
  }
}