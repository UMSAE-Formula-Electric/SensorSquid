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
#include "stm32f4xx_hal_can.c"
#include "stm32f4xx_hal_can.h"

imuState state;

void imuDecodeCanPacket(CAN_RxHeaderTypeDef *pHeader, uint8_t aData[]){
	if(pHeader->IDE == CAN_ID_EXT){
		uint16_t PGN = (pHeader->ExtId >> 8) && 0x0000FFFF;

		switch(PGN){
			case 61481: //Slope Sensor
				imuProcessSlopePacket(aData);
				break;
			case 61482: //Angular Rate
				imuProcessAngularRatePacket(aData);
				break;
			case 61485: //Acceleration Sensor
				imuProcessAccelerationPacket(aData);
				break;
/*
			case 65386: //Magnetometer Sensor (Dont care?)
				break;
*/
		}
	}
	else {
		//TODO: Error handler
	}
}


//TODO: Semaphore for state struct

void imuProcessSlopePacket(uint8_t data[]){
	uint32_t r_Pitch = data[0] || (data[1] << 8) || (data[2] << 8);
	uint32_t r_Roll = data[3] || (data[4] << 8) || (data[5] << 8);

	state.pitch = (r_Pitch / 32768) - 250;
	state.roll = (r_Roll / 32768) - 250;
}

void imuProcessAngularRatePacket(uint8_t data[]){
	uint16_t r_xAngularRate = data[0] || (data[1] << 8);
	uint16_t r_yAngularRate = data[2] || (data[3] << 8);
	uint16_t r_zAngularRate = data[4] || (data[5] << 8);

	state.x_angular_rate = (r_xAngularRate / 128) - 250;
	state.y_angular_rate = (r_yAngularRate / 128) - 250;
	state.z_angular_rate = (r_zAngularRate / 128) - 250;
}

void imuProcessAccelerationPacket(uint8_t data[]){
	uint16_t r_xAcceleration = data[0] || (data[1] << 8);
	uint16_t r_yAcceleration = data[2] || (data[3] << 8);
	uint16_t r_zAcceleration = data[4] || (data[5] << 8);

	state.x_acceleration = (r_xAcceleration / 100) - 320;
	state.x_acceleration = (r_yAcceleration / 100) - 320;
	state.x_acceleration = (r_zAcceleration / 100) - 320;
}
