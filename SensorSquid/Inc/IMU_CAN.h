/*
 * IMU_CAN.h
 *
 *  Created on: Jul 31, 2022
 *      Author: Brett Stevens
 */

#include <stdio.h>

#ifndef IMU_CAN_H_
#define IMU_CAN_H_



#endif /* IMU_CAN_H_ */

typedef struct{
	float x_acceleration;

	float y_acceleration;

	float z_acceleration;

	float x_angular_rate;

	float y_angular_rate;

	float z_angular_rate;

	float pitch;	/*!< Specifies pitch in degrees with high resolution. Can be between -250 and +252 */

	float roll;		/*!< Specifies roll in degrees with high resolution. Can be between -250 and +252 */

	float yaw;		/*!< Specifies roll in degrees with high resolution. Can be between -250 and +252 */
}imuState;

void imuProcessSlopePacket(imuState* state, uint8_t data[]);
void imuProcessAngularRatePacket(imuState* state, uint8_t data[]);
void imuProcessAccelerationPacket(imuState* state, uint8_t data[]);
