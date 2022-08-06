/*
 * can.h
 *
 *  Created on: Aug. 5, 2022
 *      Author: Brett
 */

#ifndef CAN_H_
#define CAN_H_

#define imuSlopePGN 		61481
#define imuAngularRatePGN 	61482
#define imuAccelerationPGN	61485
#define imuMagnetometerPGN	65386

void hcan1_rx_readPacketsTask( void * pvParameters );
void decodeCanPacket(CAN_RxHeaderTypeDef *pHeader, uint8_t aData[]);

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

/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan1;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_CAN1_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */
