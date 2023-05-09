/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"
#include "usart.h"
#include "IMU_CAN.h"

/* USER CODE BEGIN 0 */
#include <stdio.h>
#include <string.h>


CAN_HandleTypeDef     hcan1;
CAN_TxHeaderTypeDef   TxHeader;
CAN_RxHeaderTypeDef   RxHeader;
uint8_t               TxData[8];
uint8_t               RxData[8];
uint32_t              TxMailbox;
/* USER CODE END 0 */


/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */
	CAN_FilterTypeDef  sFilterConfig;
  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 4;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_2TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = ENABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */
  /*##-2- Configure the CAN Filter ###########################################*/
  sFilterConfig.FilterBank = 0;
  sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterActivation = ENABLE;
  sFilterConfig.SlaveStartFilterBank = 14;

  if(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK)
  {
    /* Filter configuration Error */
    Error_Handler();
  }

  /*##-3- Start the CAN peripheral ###########################################*/
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    /* Start Error */
    Error_Handler();
  }

  /*##-4- Activate CAN RX notification #######################################*/
  if (HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) {
	  /* Notification Error */
	  Error_Handler();
  }

  /* USER CODE END CAN1_Init 2 */

}

void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN CAN1_MspInit 1 */
//    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 1, 0);
//    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
  * @brief  Configures the CAN, transmit and receive by polling
  * @param  None
  * @retval PASSED if the reception is well done, FAILED in other case
  */
HAL_StatusTypeDef CAN_Polling(void)
{
//  /*##-4- Start the Transmission process #####################################*/
//  TxHeader.StdId = 0x22;
//  TxHeader.RTR = CAN_RTR_DATA;
//  TxHeader.IDE = CAN_ID_STD;
//  TxHeader.DLC = 2;
//  TxHeader.TransmitGlobalTime = DISABLE;
//  TxData[0] = 0xF0;
//  TxData[1] = 0x12;

  /* Request transmission */
//  if(HAL_CAN_AddTxMessage(&hcan1, &TxHeader, TxData, &TxMailbox) != HAL_OK)
//  {
//    /* Transmission request Error */
//	HAL_CAN_AbortTxRequest(&hcan1, CAN_TX_MAILBOX0 | CAN_TX_MAILBOX1 | CAN_TX_MAILBOX2);
//    return HAL_ERROR;
//  }
//
////  /* Wait transmission complete */
//  while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) != 3) {
//	  //return HAL_ERROR;
//  }
//
//  HAL_USART_Transmit(&husart2, (uint8_t *) "1", 1, 10);

  /*##-5- Start the Reception process ########################################*/
  uint32_t fifoFill = HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0);
//  int msg_size = sizeof("ExtId: %lx, Data: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n") + 1;
//  volatile char msg[msg_size];
//  HAL_USART_Transmit(&husart2, (uint8_t *) msg, strlen(msg), 10);
  if(fifoFill < 1)
  {
    /* Reception Missing */
	  return HAL_ERROR;
  }

  if(HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK)
  {
    /* Reception Error */
//	  HAL_USART_Transmit(&husart2, (uint8_t *) "3", 1, 10);
	  return HAL_ERROR;
  }

//  if((RxHeader.StdId != 0x33)                     ||
//     (RxHeader.RTR != CAN_RTR_DATA)               ||
//     (RxHeader.IDE != CAN_ID_STD)                 ||
//     (RxHeader.DLC != 2))						  //||
////     ((RxData[0]<<8 | RxData[1]) != 0xCAFE))
//  {
//    /* Rx message Error */
//    return HAL_ERROR;
//  }

  if(RxHeader.IDE == CAN_ID_STD)
    {
        if(!(RxHeader.StdId != 0x0F0 || RxHeader.StdId != 0x4F0) ||
           (RxHeader.DLC != 8))
        {
              return HAL_ERROR;
        }
    }
    else if(RxHeader.IDE == CAN_ID_EXT)
    {
        if(!(RxHeader.ExtId != 0x8F02D80 || RxHeader.ExtId != 0xCF02A80) ||
           (RxHeader.DLC != 8))
        {
              return HAL_ERROR;
        }
        else{
//        	snprintf(msg, msg_size, "ExtId: %lx, Data: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
//        			RxHeader.ExtId, RxData[0], RxData[1], RxData[2], RxData[3], RxData[4], RxData[5], RxData[6], RxData[7]);
//        	HAL_USART_Transmit(&husart2, (uint8_t *) msg, msg_size, 10);
        }
    }

//  snprintf(msg, 16, "%lx\n", RxHeader.ExtId);
//  HAL_USART_Transmit(&husart2, (uint8_t *) msg, 16, 10);

  return HAL_OK; /* Test Passed */
}

//void printIMUtoUSART(){
//	volatile char msg2[26];
//	sprintf(msg2, "Acceleration x: %7.4f\n", state.x_acceleration);
//
//	HAL_USART_Transmit(&husart2, (uint8_t *) msg2, strlen(msg2), 10);
//
//	//memset(msg, 0, sizeof msg);
//
//
////	sprintf(msg, "Acceleration y: %7.4f\n", state.y_acceleration);
////
////	HAL_USART_Transmit(&husart2, (uint8_t *) msg, strlen(msg), 10);
////
////	sprintf(msg, "Acceleration z: %7.4f\n", state.z_acceleration);
////
////	HAL_USART_Transmit(&husart2, (uint8_t *) msg, strlen(msg), 10);
//
////	snprintf(msg, 64, "Angular Rate: roll:%7.4f, pitch:%7.4f\n", state.roll, state.pitch);
////
////	HAL_USART_Transmit(&husart2, (uint8_t *) msg, sizeof(msg)+1, 10);
//}

void StartCanTask(void const * argument){
	imuState state;
	  for(;;)
	  {
		  if(CAN_Polling() == HAL_OK)
		  {
			/* OK: Turn on LED1 */
			  if(RxHeader.IDE == CAN_ID_EXT)
			  {
				  switch (RxHeader.ExtId)
				  {
					  case 0x8F02D80:
						  imuProcessAccelerationPacket(&state, RxData);
						  print_USART_float(&huart1, state.x_acceleration);
						  print_USART_float(&huart1, state.y_acceleration);
						  print_USART_float(&huart1, state.z_acceleration);
						  break;

					  case 0xCF02A80:
						  imuProcessAngularRatePacket(&state, RxData);
						  print_USART_float(&huart1, state.x_angular_rate);
						  print_USART_float(&huart1, state.y_angular_rate);
						  print_USART_float(&huart1, state.z_angular_rate);
						  break;

				  }
			  }

	//			sprintf(msg2, "Acceleration");
	//			HAL_USART_Transmit(&husart2, (uint8_t *) msg2, strlen(msg2), 10);
	//		  snprintf(msg2, 7, "%5.4f", state.x_acceleration);
	//			HAL_USART_Transmit(&husart2, (uint8_t *) msg2, sizeof(msg2), 10);
	//			memset(msg2, 0, sizeof msg2);
	//			//sprintf(msg2, "\r\n");
				//HAL_USART_Transmit(&husart2, (uint8_t *) msg2, strlen(msg2), 10);
	//		  printIMUtoUSART();
	//		HAL_USART_Transmit(&husart2, (uint8_t *) "1", 1, 10);
		  }
		  else
		  {
			/* KO: Turn on LED3 */
	//		HAL_USART_Transmit(&husart2, (uint8_t *) "0", 1, 10);
		  }
		  //HAL_Delay(50);
  }
}

void print_USART_float(UART_HandleTypeDef* huart, double number)
{
	char msg[16];
	sprintf(msg, "%04d", (int) number);
	HAL_UART_Transmit(huart, (uint8_t *) msg, strlen(msg), 10);
	sprintf(msg, ".");
	HAL_UART_Transmit(huart, (uint8_t *) msg, 1, 10);
	sprintf(msg, "%04d", (int) number*1000 - ((int) number)*1000);
	HAL_UART_Transmit(huart, (uint8_t *) msg, strlen(msg), 10);
	sprintf(msg, "\r\n");
	HAL_UART_Transmit(huart, (uint8_t *) msg, 2, 10);
}

//void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan){ /* Get RX message */
//	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &RxHeader, RxData) != HAL_OK) {
//		/* Reception Error */
//		Error_Handler();
//	}
//	/* Display LEDx */
//	if ((RxHeader.StdId == 0x11) && (RxHeader.IDE == CAN_ID_STD) && (RxHeader.DLC == 2))
//	{
//		HAL_USART_Transmit(&husart2, (uint8_t *) "1", 1, 10);
//	}
//}
/* USER CODE END 1 */
