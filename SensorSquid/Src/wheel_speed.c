/**
 ******************************************************************************
 * @file
 * @author  Martin Rickey, Blane Cypurda
 * @version V2.0
 * @date    March 12th 2022
 * @brief   Used to determine wheel speed from a Hall effect wheel speed sensor
 * 			Measures the period of a square wave input to determine angular velocity of wheel.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "wheel_speed.h"



// Status register masks to see why an interrupt was triggered
#define TIM_IT_CC1_FLAG   0x02
#define TIM_IT_CC2_FLAG   0x04


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define NUMPERIODS 4	//number of periods to average over
#define NumCaptureVals (NUMPERIODS+1)	//number of input capture values to save
#define NUM_TEETH 		10				// Number of teeth on the sproket which we count for a full rotation of the wheel
#define WHEEL_DIAMETER	0.41			// In meters

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//todo: Test if this is an actual solution. (RTOS Queues otherwise?)
uint32_t periodFL = 0;	//most recent period of front left
uint32_t periodFR = 0;	//most recent period of front right

volatile uint32_t periodBLprev = 0;			//most recent period of back left 		(Maybe volatile will make it work better?)
volatile uint32_t periodBLcurr = 0;			//most recent period of back left 		(Maybe volatile will make it work better?)

volatile uint32_t periodBRprev = 0;			//most recent period of back right
volatile uint32_t periodBRcurr = 0;			//most recent period of back left 		(Maybe volatile will make it work better?)

// DO NOT WRITE
volatile static uint32_t overflow_cnt_BR = 0;			// Count the overflows of the timer on the back right ()
volatile static uint32_t overflow_cnt_BL = 0;

//
volatile static uint32_t periodOF_BR = 0;				// How many overflows have occured since last sproket tooth
volatile static uint32_t periodOF_BL = 0;


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/




/*get_wheel_ang_vel
 *
 * 		@Brief returns the param wheel's speed in revolutions per second
 *
 */
float get_wheel_ang_vel(enum wheelPosition wheel) {
#ifdef ACB
	assert_param((wheel == backLeftWheel) || (wheel == backRightWheel));			// ensure that we do not call for wheels the board is not configured for
#else
	assert_param((wheel == frontLeftWheel) || (wheel == frontRightWheel));
#endif

	float clkFrq = 0; 											//Frequency of timer peripheral clock
	float speed = 0;												//the wheel speed

	//wheel specific
	uint32_t periodcurr = 0;
	uint32_t periodprev = 0;

	uint32_t overflow = 0;

	uint32_t sys_clk = 0;

	//get wheel specific data buffer
	switch (wheel) {

	case frontLeftWheel:
		periodcurr = periodFL;
		break;
	case frontRightWheel:
		periodcurr = periodFR;
		break;
	case backLeftWheel:
		overflow = periodOF_BL;
		periodcurr = periodBLcurr;
		periodprev = periodBLprev;
		break;

	case backRightWheel:
		overflow = periodOF_BR;
		periodcurr = periodBRcurr;
		periodprev = periodBRprev;
		break;
	}

	//get clock frequency
	sys_clk = HAL_RCC_GetSysClockFreq();

	//get wheel speed
	clkFrq =  (float)sys_clk/55;						// Timer peripheral frequency

	if(periodcurr != 0 && periodprev !=0 && overflow != 0){

		if(overflow > 0){
			speed = ((float)clkFrq) * NUM_TEETH / (float)(periodcurr + (65535 - periodprev) + ((overflow-1) * 65535));			// calculate speed
		}
		else if(overflow > 0){
			assert_param(periodcurr - periodprev >= 0);					// ensure that this assumption holds. (If we're getting negative values, we're not counting overflows)
			speed = ((float)clkFrq) * NUM_TEETH / (float)(periodcurr - periodprev);			// calculate speed
		}

		//check that the division didnt give nan
		if (isnan(speed))
			speed = 0;

	}

	return speed;
}

void xWheelSpeed_Logger(void* pvParameters){

	float wheelsped_bufferFL, wheelsped_bufferFR;
	char logged_msgFR[128] =  {0};
	char logged_msgFL[128] =  {0};

	time_delta td;
	float timedelt;

	for(;;){
		wheelsped_bufferFL = get_wheel_ang_vel(frontLeftWheel);
		td = getTime();
		timedelt = (float)td.seconds + td.subseconds;

		sprintf(logged_msgFR, "Delta: %f, WSPD(FL): %3.4f", timedelt, wheelsped_bufferFL);

		wheelsped_bufferFR = get_wheel_ang_vel(frontRightWheel);
		td = getTime();
		timedelt = (float)td.seconds + td.subseconds;

		sprintf(logged_msgFR, "Delta: %f, WSPD(FR): %3.4f", timedelt, wheelsped_bufferFR);

		// Log both wheels
		SD_Log(logged_msgFL, -1);
		SD_Log(logged_msgFR, -1);

		vTaskDelay(pdMS_TO_TICKS(5));	// Log the wheel speeds every 5ms
	}


}//xWheelSpeed_Logger


/*HAL_TIM_IC_CaptureCallback
 *
 * @brief interrupt on rising edge of wheel speed pins. Puts most recently captred value in circular buffer
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	// Timer Channel 1, PA8 -> Rear Right Wheel

	if(htim->Instance->SR & TIM_IT_CC1_FLAG){
		periodBRprev = periodBRcurr;						// Save the old value
		periodBRcurr = htim2.Instance->CCR1;				// Get capture compare register 1's value, CC1 flag is cleared from reading this
		periodOF_BR = overflow_cnt_BR;						// Save the period of the overflow counter
		overflow_cnt_BR = 0;								// Reset the overflow counter.
	}
	if(htim->Instance->SR & TIM_IT_CC2_FLAG){
		periodBLprev = periodBLcurr;						// Save the old value
		periodBLcurr = htim2.Instance->CCR2;					// Get capture compare register 2's value
		periodOF_BL = overflow_cnt_BL;						// Save the period of the overflow counter
		overflow_cnt_BL = 0;								// Reset the overflow counter.
	}

}


//timer 3 interrupt handler
void HAL_Wheelspeed_Overflow_Callback(void) {

	overflow_cnt_BL++;										// increment the overflow
	overflow_cnt_BR++;

}
