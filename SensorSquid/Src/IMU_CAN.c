/*
 * IMU_CAN.c
 *
 *  Created on: Jul 31, 2022
 *      Author: Brett Stevens
 */

//Datasheets
//https://openimu.readthedocs.io/en/latest/software/CAN/CAN_J1939_DataPacketMessages.html
//https://navview.blob.core.windows.net/web-resources/7430-3321-01%20User%20Manual%20OpenIMU335.pdf?_t=1621434422173



#include "IMU_CAN.h"

extern imuState state;

//TODO: Semaphore for state struct

void imuProcessSlopePacket(imuState* state, uint8_t data[]){
	uint16_t r_Pitch = data[0] | (data[1] << 8);
	uint16_t r_Roll = data[2] | (data[3] << 8);
	uint16_t r_Yaw = data[4] | (data[5] << 8);

	state->pitch = ((float)r_Pitch / 32768) - 250;
	state->roll = ((float)r_Roll / 32768) - 250;
	state->yaw = ((float)r_Yaw / 32768) - 250;
}

void imuProcessAngularRatePacket(imuState* state, uint8_t data[]){
	uint16_t r_yAngularRate = data[0] | (data[1] << 8);
	uint16_t r_xAngularRate = data[2] | (data[3] << 8);
	uint16_t r_zAngularRate = data[4] | (data[5] << 8);

	state->x_angular_rate = ((float)r_xAngularRate / 128) - 250;
	state->y_angular_rate = ((float)r_yAngularRate / 128) - 250;
	state->z_angular_rate = ((float)r_zAngularRate / 128) - 250;
}

void imuProcessAccelerationPacket(imuState* state, uint8_t data[]){
	uint16_t r_yAcceleration = data[0] | (data[1] << 8);
	uint16_t r_xAcceleration = data[2] | (data[3] << 8);
	uint16_t r_zAcceleration = data[4] | (data[5] << 8);

	state->x_acceleration = ((float)r_xAcceleration / 100) - 320;
	state->y_acceleration = ((float)r_yAcceleration / 100) - 320;
	state->z_acceleration = ((float)r_zAcceleration / 100) - 320;
}

//TODO: Export to uart
