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
#define NUM_TEETH 		10				// Number of teeth on the sproket which we count for a full rotation of the wheel
#define WHEEL_DIAMETER	0.41			// In meters

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



/* Private functions ---------------------------------------------------------*/



void Init_WheelSpeed_Logging_Task(){

	//Gatekeeper
	xWheelSpeed_Logger_Handle = xTaskCreateStatic(	xWheelSpeed_Logger,
														"Wheel Speed Logger",
														1024,
														NULL,
														WHEELSPEED_LOG_TASK_PRIORITY,
														xWheelSpeed_Logger_Stack,
														&xWheelSpeed_Logger_Buffer);	// Create static task for logging the wheelspeed sensor to the sdcard

int i =3;
}


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
		overflow = periodOF_FL;
		periodcurr = periodFLcurr;
		periodprev = periodFLprev;
		break;
	case frontRightWheel:
		overflow = periodOF_FR;
		periodcurr = periodFRcurr;
		periodprev = periodFRprev;
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
			speed = ((float)clkFrq) / (NUM_TEETH * (float)(periodcurr + (65535 - periodprev) + ((overflow-1) * 65535)));			// calculate speed
		}
		else if(overflow == 0){
			assert_param(periodcurr - periodprev >= 0);					// ensure that this assumption holds. (If we're getting negative values, we're not counting overflows)
			speed = ((float)clkFrq) / (NUM_TEETH * (float)(periodcurr - periodprev));			// calculate speed
		}

		//check that the division didnt give nan
		if (isnan(speed))
			speed = 0;

	}

	return speed;
}



void xWheelSpeed_Logger(void* pvParameters){
	char logged_msgFR[256] =  {0};
	char logged_msgFL[256] =  {0};
	char logged_msgCOMB[512] = {63};

	time_delta td;
	float timedelt;
	float wheelsped_bufferFL, wheelsped_bufferFR;

	for(;;){
		wheelsped_bufferFL = get_wheel_ang_vel(frontLeftWheel);
		td = getTime();
		timedelt = (float)td.seconds + td.subseconds;

		sprintf(logged_msgFL, "Delta: %f, WSPD(FL): %f\n", timedelt, wheelsped_bufferFL); //always include \n after an entry

		wheelsped_bufferFR = get_wheel_ang_vel(frontRightWheel);
		td = getTime();
		timedelt = (float)td.seconds + td.subseconds;

		sprintf(logged_msgFR, "Delta: %f, WSPD(FR): %f\n", timedelt, wheelsped_bufferFR);//always include \n after an entry

		sprintf(logged_msgCOMB, "%s%s",logged_msgFR, logged_msgFL); //ALWAYS make sure final output to queue is a single item

		//Log the left and right wheels
		int buffer_length = strlen(logged_msgCOMB);
        xSemaphoreTake(xMutex, portMAX_DELAY);
		SD_Log(logged_msgCOMB, buffer_length);
        xSemaphoreGive(xMutex);


		vTaskDelay(pdMS_TO_TICKS(5));	// Log the wheel speeds every 5ms
	}


}//xWheelSpeed_Logger


/*HAL_TIM_IC_CaptureCallback
 *
 * @brief interrupt on rising edge of wheel speed pins. Puts most recently captred value in circular buffer
 */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
	// Timer Channel 1, PA8 -> Rear Right Wheel

	if(htim->Instance == TIM2){
		periodFRprev = periodFRcurr;						// Save the old value
		periodFRcurr = htim2.Instance->CCR1;				// Get capture compare register 1's value, CC1 flag is cleared from reading this
		periodOF_FR = overflow_cnt_FR;						// Save the period of the overflow counter
		overflow_cnt_FR = 0;								// Reset the overflow counter.
	}
	if(htim->Instance == TIM3){
		periodFLprev = periodFLcurr;						// Save the old value
		periodFLcurr = htim3.Instance->CCR1;					// Get capture compare register 2's value
		periodOF_FL = overflow_cnt_FL;						// Save the period of the overflow counter
		overflow_cnt_FL = 0;								// Reset the overflow counter.
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



