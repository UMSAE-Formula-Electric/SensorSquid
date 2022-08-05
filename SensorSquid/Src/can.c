/*
 * can.c
 *
 *  Created on: Aug. 5, 2022
 *      Author: Brett
 */

#include "main.c"
#include "can.h"
#include "IMU_CAN.c"
#include "stm32f4xx_hal_can.c"
#include "stm32f4xx_hal_can.h"

void hcan1_rx_readPacketsTask( void * pvParameters )
{
    /* The parameter value is expected to be 1 as 1 is passed in the
    pvParameters value in the call to xTaskCreate() below.
    */

    configASSERT( ( ( uint32_t ) pvParameters ) == 1 );

    for( ;; )
    {
        if(HAL_CAN_GetRxFifoFillLevel(hcan1, CAN_RX_FIFO0) > 0){
        	uint8_t r_Data[8];
        	CAN_RxHeaderTypeDef r_Header;

        	//might be FIFO1?
        	HAL_CAN_GetRxMessage(hcan1, CAN_RX_FIFO0, r_Header, r_Data);

        	if(pHeader->IDE == CAN_ID_EXT){
				uint16_t PGN = (r_Header->ExtId >> 8) && 0xFFFF;

				switch(PGN){
					case imuSlopePGN: //Slope Sensor
						imuProcessSlopePacket(r_Data);
						break;
					case imuAngularRatePGN: //Angular Rate
						imuProcessAngularRatePacket(r_Data);
						break;
					case imuAccelerationPGN: //Acceleration Sensor
						imuProcessAccelerationPacket(r_Data);
						break;
		/*
					case imuMagnetometerPGN: //Magnetometer Sensor (Dont care?)
						break;
		*/
				}
			}
			else if(r_Header->IDE == CAN_ID_STD) {
				switch(r_Header){
					//empty for now
				}
			}
        }
    }
}

