/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef WHEEL_SPEED_H
#define WHEEL_SPEED_H

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "timestamps.h"
#include "FreeRTOS.h"

//#include <assert.h>
#include "math.h"
#include "stdlib.h"
#include "stdio.h"
#include "sd_card.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

//enumerated type for indicating which wheel we are reffering too
 enum wheelPosition{
	frontLeftWheel = 0,
	frontRightWheel,
	backLeftWheel,
	backRightWheel
};
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

void Init_WheelSpeed_Logging_Task();

float get_wheel_ang_vel(enum wheelPosition wheel);

void HAL_Wheelspeed_Overflow_Callback();

#endif
