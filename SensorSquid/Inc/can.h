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

#endif /* CAN_H_ */
