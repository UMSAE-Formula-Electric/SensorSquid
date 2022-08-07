/*
 * IMU_CAN.c
 *
 *  Created on: Jul 31, 2022
 *      Author: Brett Stevens
 */

//Datasheets
//https://openimu.readthedocs.io/en/latest/software/CAN/CAN_J1939_DataPacketMessages.html
//https://navview.blob.core.windows.net/web-resources/7430-3321-01%20User%20Manual%20OpenIMU335.pdf?_t=1621434422173

#ifndef __IMU_CAN_C
#define __IMU_CAN_C



#include "IMU_CAN.h"
#include "stdint.h"

struct imuState state;

//TODO: Semaphore for state struct

struct imuState imuGetState(){
	return state;
}

void imuProcessSlopePacket(uint8_t data[]){
	uint32_t r_Pitch = data[0] | (data[1] << 8) | (data[2] << 8);
	uint32_t r_Roll = data[3] | (data[4] << 8) | (data[5] << 8);

	state.pitch = (r_Pitch / 32768) - 250;
	state.roll = (r_Roll / 32768) - 250;
}

void imuProcessAngularRatePacket(uint8_t data[]){
	uint16_t r_xAngularRate = data[0] | (data[1] << 8);
	uint16_t r_yAngularRate = data[2] | (data[3] << 8);
	uint16_t r_zAngularRate = data[4] | (data[5] << 8);

	state.x_angular_rate = (r_xAngularRate / 128) - 250;
	state.y_angular_rate = (r_yAngularRate / 128) - 250;
	state.z_angular_rate = (r_zAngularRate / 128) - 250;
}

void imuProcessAccelerationPacket(uint8_t data[]){
	uint16_t r_xAcceleration = data[0] | (data[1] << 8);
	uint16_t r_yAcceleration = data[2] | (data[3] << 8);
	uint16_t r_zAcceleration = data[4] | (data[5] << 8);

	state.x_acceleration = (r_xAcceleration / 100) - 320;
	state.x_acceleration = (r_yAcceleration / 100) - 320;
	state.x_acceleration = (r_zAcceleration / 100) - 320;
}

//TODO: Export to uart


#endif
