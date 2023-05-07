#ifndef CAN_H_
#define CAN_H_

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdint.h>
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
extern CAN_HandleTypeDef hcan1;
#define imuSlopePGN 		61481
#define imuAngularRatePGN 	0x4f0
#define imuAccelerationPGN	0x0f0
#define imuMagnetometerPGN	65386
/* USER CODE END Private defines */

/* USER CODE BEGIN Prototypes */
void init_hcan1_rx_task();
void hcan1_rx_readPacketsTask();
void decodeCanPacket(CAN_RxHeaderTypeDef *pHeader, uint8_t aData[]);
void MX_CAN1_Init(void);
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan1);
void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan1);
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */
