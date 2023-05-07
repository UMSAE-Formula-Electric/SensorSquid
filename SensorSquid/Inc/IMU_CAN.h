/*
 * IMU_CAN.h
 *
 *  Created on: Jul 31, 2022
 *      Author: Brett Stevens
 */

#ifndef IMU_CAN_H_
#define IMU_CAN_H_

#include <stdint.h>
#include "FreeRTOS.h"

void init_imu_post_task();
void imuPostStateTask();
void imuProcessSlopePacket(uint8_t data[]);
void imuProcessAngularRatePacket(uint8_t data[]);
void imuProcessAccelerationPacket(uint8_t data[]);

struct imuState{
	float x_acceleration;

	float y_acceleration;

	float z_acceleration;

	float x_angular_rate;

	float y_angular_rate;

	float z_angular_rate;

	float pitch; /*!< Specifies pitch in degrees with high resolution. Can be between -250 and +252 */

	float roll;		/*!< Specifies roll in degrees with high resolution. Can be between -250 and +252 */
};


#endif /* IMU_CAN_H_ */


