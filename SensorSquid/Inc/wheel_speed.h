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

struct WheelSpeed {
	float METERS_PER_SECOND;
	float KILOMETERS_PER_HOUR;
	float RADIANS_PER_SECOND;
};

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

// Gets the required prescalar for accurate wheel speed readings
int get_wheel_speed_timer_prescaler();

struct WheelSpeed get_wheel_ang_vel(enum wheelPosition wheel);

void HAL_FR_Wheelspeed_Overflow_Callback();
void HAL_FL_Wheelspeed_Overflow_Callback();

#endif
