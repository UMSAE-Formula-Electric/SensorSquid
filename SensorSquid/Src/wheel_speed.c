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

#define WHEELSPEED_LOG_TASK_PRIORITY		osPriorityRealtime			// Ensure we're logging as realtime as we can get


/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/
#define NUMPERIODS 4	//number of periods to average over
#define NumCaptureVals (NUMPERIODS+1)	//number of input capture values to save

//Stores the last 10 wheel speeds
#define WHEEL_SPEEDS_STORED  10
#define WHEEL_DIAMETER	1			// In meters
#define WHEEL_CIRCUMFERENCE (WHEEL_DIAMETER * 3.14159)	// In meters

#define NUM_TEETH  10				// Number of teeth on the sproket which we count for a full rotation of the wheel
#define TEETH_DIST (WHEEL_CIRCUMFERENCE / NUM_TEETH)	// The distance between each tooth

#define ACCURATE_SPEED 30				// In Meter / S, the speed at which we want the number of ticks per tooth [TICKS_PER_TOOTH] vvv
#define TICKS_PER_TOOTH 100				// In ticks / tooth, how level of precision you want at the speed of [ACCURATE_SPEED] ^^^

#define TICKS_PER_ROTATION (TICKS_PER_TOOTH * NUM_TEETH * ACCURATE_SPEED)		// How many ticks there are for each full revolution of the wheel at [ACCURATE_SPEED]
#define TICKS_PER_METER (TICKS_PER_ROTATION * WHEEL_CIRCUMFERENCE)	// How many ticks there are for each meter traveled

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

//todo: Test if this is an actual solution. (RTOS Queues otherwise?)
uint32_t periodFL = 0;	//most recent period of front left
uint32_t periodFR = 0;	//most recent period of front right


volatile static uint32_t periodFLprev = 0;			//most recent period of back left 		(Maybe volatile will make it work better?)
volatile static uint32_t periodFLcurr = 0;			//most recent period of back left 		(Maybe volatile will make it work better?)

volatile static uint32_t periodFRprev = 0;			//most recent period of back right
volatile static uint32_t periodFRcurr = 0;			//most recent period of back left 		(Maybe volatile will make it work better?)

volatile static uint32_t periodBLprev = 0;			//most recent period of back left 		(Maybe volatile will make it work better?)
volatile static uint32_t periodBLcurr = 0;			//most recent period of back left 		(Maybe volatile will make it work better?)

volatile static uint32_t periodBRprev = 0;			//most recent period of back right
volatile static uint32_t periodBRcurr = 0;			//most recent period of back left 		(Maybe volatile will make it work better?)

// DO NOT WRITE
volatile static uint32_t overflow_cnt_FR = 0;			// Count the overflows of the timer on the back right ()
volatile static uint32_t overflow_cnt_FL = 0;

//
volatile static uint32_t periodOF_FR = 0;				// How many overflows have occured since last sproket tooth
volatile static uint32_t periodOF_FL = 0;


volatile static uint32_t overflow_cnt_BR = 0;			// Count the overflows of the timer on the back right ()
volatile static uint32_t overflow_cnt_BL = 0;

//
volatile static uint32_t periodOF_BR = 0;				// How many overflows have occured since last sproket tooth
volatile static uint32_t periodOF_BL = 0;


//Static Task Data
static StackType_t xWheelSpeed_Logger_Stack[256];		// Stack for the SD_Card logger task
static StaticTask_t xWheelSpeed_Logger_Buffer;
static TaskHandle_t	xWheelSpeed_Logger_Handle;	//Task handle for the SD card task


/* Private function prototypes -----------------------------------------------*/

void xWheelSpeed_Logger(void* pvParameters);


int temp_CLOCK_RATE = 0;

/* Private functions ---------------------------------------------------------*/
void Init_WheelSpeed_Logging_Task(){
	temp_CLOCK_RATE = HAL_RCC_GetSysClockFreq();
	//Gatekeeper
	xWheelSpeed_Logger_Handle = xTaskCreateStatic(	xWheelSpeed_Logger,
														"Wheel Speed Logger",
														1024,
														NULL,
														WHEELSPEED_LOG_TASK_PRIORITY,
														xWheelSpeed_Logger_Stack,
														&xWheelSpeed_Logger_Buffer);	// Create static task for logging the wheelspeed sensor to the sdcard

}


/*
 * @Brief Creates a wheel speed struct, so conversions are handled automatically
 *	Creates a WheelSpeed in M/s, Km/h & Radians/s
 */
const struct WheelSpeed create_wheel_speed(const float METERS_PER_SECOND) {
	struct WheelSpeed speed;

	speed.METERS_PER_SECOND = METERS_PER_SECOND;
	speed.KILOMETERS_PER_HOUR = METERS_PER_SECOND * 3.6;
	speed.RADIANS_PER_SECOND = 6.2832 * (METERS_PER_SECOND / WHEEL_CIRCUMFERENCE);

	return speed;
}

/*get_wheel_speed_timer_prescaler
 *
 * @Brief Gets the required prescalar for accurate wheel speed readings
*/
int get_wheel_speed_timer_prescaler() {
	const int GOAL = TICKS_PER_METER;
	const uint32_t CLOCK_RATE = HAL_RCC_GetSysClockFreq();
	const int PRESCALER = CLOCK_RATE / GOAL;

	return PRESCALER;
}




int front_left_index = 0;
int front_right_index = 0;

int front_left_ticks[WHEEL_SPEEDS_STORED];
int front_right_ticks[WHEEL_SPEEDS_STORED];


/*get_wheel_ang_vel
 * 	@Brief returns the param wheel's speed in the WheelSpeed struct
 * 	The WheelSpeed struct contains the speed in M/s, km/h and Radians / s
 */
const struct WheelSpeed get_wheel_ang_vel(enum wheelPosition wheel) {
#ifdef ACB
	assert_param((wheel == backLeftWheel) || (wheel == backRightWheel));			// ensure that we do not call for wheels the board is not configured for
#else
	assert_param((wheel == frontLeftWheel) || (wheel == frontRightWheel));
#endif
	//wheel specific
	uint32_t periodcurr = 0;
	uint32_t periodprev = 0;
	uint32_t overflow = 0;

	const int OVERFLOW_NUMBER = 65535;

	int *values = 0;

	//get wheel specific data buffer
	switch (wheel) {
		case frontLeftWheel:
			overflow = periodOF_FL;
			periodcurr = periodFLcurr;
			periodprev = periodFLprev;
			values = front_left_ticks;
			break;

		case frontRightWheel:
			overflow = periodOF_FR;
			periodcurr = periodFRcurr;
			periodprev = periodFRprev;
			values = front_right_ticks;
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

	//Accounts for the overflow
	if (overflow > 0) {
		periodcurr += OVERFLOW_NUMBER * overflow;
	}

	const uint32_t TICKS_PER_SECOND = temp_CLOCK_RATE / (2 * get_wheel_speed_timer_prescaler());

	// The amount of ticks between each tooth, must be atleast 1
	int tick_difference = (periodcurr - periodprev);
	if (tick_difference == 0) {
		tick_difference = 1;
	}

	// Calculates how much time in seconds it has been between the last rising edge
	const float TIME_BETWEEN_TEETH = tick_difference / (float)TICKS_PER_SECOND;
	const float SPEED = TEETH_DIST / TIME_BETWEEN_TEETH;

	return create_wheel_speed(SPEED);
}



void xWheelSpeed_Logger(void* pvParameters){

	char logged_msgFR[256] =  {0};
	char logged_msgFL[256] =  {0};

	time_delta td;
	float timedelta;

	struct WheelSpeed front_left;
	struct WheelSpeed front_right;

	for(;;){
		// Logs the FL wheel speed in M/S
		front_left = get_wheel_ang_vel(frontLeftWheel);
		td = getTime();
		timedelta = (float)td.seconds + td.subseconds;
		sprintf(logged_msgFL, "Delta: %f, WSPD(FL): %f", timedelta, front_left.METERS_PER_SECOND);

		// Logs the FR wheel speed in M/S
		front_right = get_wheel_ang_vel(frontRightWheel);
		td = getTime();
		timedelta = (float)td.seconds + td.subseconds;
		sprintf(logged_msgFR, "Delta: %f, WSPD(FR): %f", timedelta, front_right.METERS_PER_SECOND);

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
	static uint32_t time_dif = 0;

	if(htim->Instance == TIM2){
		periodFRprev = periodFRcurr;						// Save the old value
		periodFRcurr = htim2.Instance->CCR1;				// Get capture compare register 1's value, CC1 flag is cleared from reading this


		periodOF_FR = overflow_cnt_FR;						// Save the period of the overflow counter
		overflow_cnt_FR = 0;								// Reset the overflow counter.

		// Stores the wheel speed, enabling averaging & debugging purposes
		time_dif = periodFRcurr - periodFRprev;
		front_right_ticks[front_right_index] = time_dif;
		front_right_index += 1;
		front_right_index = front_right_index % WHEEL_SPEEDS_STORED;
	}

	if(htim->Instance == TIM3){
		periodFLprev = periodFLcurr;						// Save the old value
		periodFLcurr = htim2.Instance->CCR1;;							// Get capture compare register 2's value

		periodOF_FL = overflow_cnt_FL;						// Save the period of the overflow counter
		overflow_cnt_FL = 0;								// Reset the overflow counter.

		// Stores the wheel speed, enabling averaging & debugging purposes
		time_dif = periodFLcurr - periodFLprev;
		front_left_ticks[front_left_index] = time_dif;
		front_left_index += 1;
		front_left_index = front_left_index % WHEEL_SPEEDS_STORED;
	}
}


//timer 2 interrupt handler
void HAL_FR_Wheelspeed_Overflow_Callback(void) {
	overflow_cnt_FR++;
}

// Timer 3 interrupt handler
void HAL_FL_Wheelspeed_Overflow_Callback(void) {
	overflow_cnt_FL++;										// increment the overflow
}



