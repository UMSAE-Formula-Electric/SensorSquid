/*
 * 	sd_card.c
 *
 *  Created on: Jan 6, 2021
 *      Author: Blane Cypurda
 *      Desc: 	RTOS tasks and functions related to operating logging, reading and writing
 *      		to the SD card, within the context of the operating system.
 *
 *		Notes for use of API: 	-Maximum 2 sequential requests queued for logging per loop of RTOS task
 *								-Always include "\n" at the end of an entry
 *
 *
 */


#include "sd_card.h"


//FATFS
static FATFS fs;							// 	File System
static FIL fil[FS_MAX_CONCURRENT_FILES];	// 	array of file objects
static BYTE work[FF_BUFFER_SIZE];			//	working buffer		//TODO: decide what to do with this
static UINT bw;								//	bytes written
static UINT br;								// 	bytes read
static FRESULT f_res;						//	function result
static DIR dir;								// 	current directory



//	Operation modes of SD card
typedef enum{
	Read,
	Write,
	Sync,
	Eject
}SD_Request_Type;

//Data being sent through the Queue
typedef struct{

	SD_Request_Type type;		//	Whether to read or write from the SD
	FileEnum fileName;			// 	File to write to
	int32_t size;				// 	Number of Bytes to Read or Bytes to write
	char *buff;				// 	pointer to a string to be written or a data buffer to store what is read

}SD_Request;	// end struct

//RTOS
#define SD_GATEKEEPER_TASK_STACK_SIZE			256		// stack size of gatekeeper task
#define SD_SYNC_TASK_STACK_SIZE					128		// stack size of sync task
#define SD_GATEKEEPER_TASK_PRIORITY				osPriorityRealtime		//priority of each task
#define SD_SYNC_TASK_PRIORITY					osPriorityLow			//priority of each task

//RTOS STATIC DEFS
#define SD_QUEUE_LEN 					20							// length of queue	(queue planned on being a mailbox, 20 is to make sure we dont lose requests)
#define SD_QUEUE_SIZE 					sizeof(SD_Request)			// size of a pointer to a string
#define SD_LOG_MSG_SPACING				4							// size of the newLine string
#define SD_SYNC_ERROR_CHECK_TIMEOUT		100							// How long to wait to check with error with gatekeeper / min period of sync requests

#define SD_LOGFILE_STRING				"LOG_FILE.txt"		// String of the log file to format


// Compiler optimizations should make these not just sit in RAM
static const char LOG_DIR_NAME[] = "LOGS";				// directory to store the error log
static char LOG_FILE_NAME[] = SD_LOGFILE_STRING;			// Name for the error log file
static const char newLine[] = "\n";	// Space out messages written


static RTC_DateTypeDef RTC_DATE;


//SD_Card Logger Task
//^Gatekeeper
static StackType_t xSD_Card_Gatekeeper_Stack[SD_GATEKEEPER_TASK_STACK_SIZE];		// Stack for the SD_Card logger task
static StaticTask_t xSD_Card_Gatekeeper_Task_Buffer;
static TaskHandle_t	xSD_Card_Gatekeeper_Handle;	//Task handle for the SD card task

//^Sync Task
static StackType_t xSD_Sync_Stack[SD_SYNC_TASK_STACK_SIZE];		// Stack for the SD_Card logger task
static StaticTask_t xSD_Sync_Task_Buffer;
static TaskHandle_t	xSD_Card_Sync_Handle;	//Task handle for the SD card task

//^Test Task
static StackType_t xSD_Test_Stack[256];		// Stack for the SD_Card logger task
static StaticTask_t xSD_Test_Task_Buffer;
static TaskHandle_t	xSD_Card_Test_Handle;	//Task handle for the SD card task

//SD Card RTOS Task Data
static StaticQueue_t xSD_Card_Queue_Static;						// Queue for things to write to the SD card.
QueueHandle_t xSD_Card_Queue;									// Handle for the static queue
uint8_t xSD_Card_Queue_Storage[SD_QUEUE_LEN * SD_QUEUE_SIZE];	// Storage for the static queue



//	Initializes the SD card
// 	Creates the static objects related to the RTOS task's queue.
void Init_SD_Card(){

	// Create a Queue for the SD Card logging RTOS Task before the scheduler starts
	xSD_Card_Queue = xQueueCreateStatic(SD_QUEUE_LEN,
										SD_QUEUE_SIZE,
										xSD_Card_Queue_Storage,
										&xSD_Card_Queue_Static);

	configASSERT(xSD_Card_Queue);	// xSD_Card_Queue_Storage was not NULL so xQueue should not be NULL.

}// Init_SD_Card


//To be called from wherever tasks are being created within the RTOS
void Init_SD_RTOS_Tasks(){

	//Gatekeeper
	 xSD_Card_Gatekeeper_Handle = xTaskCreateStatic(	xSD_Card_Gatekeeper,
														"SD_Gatekeeper",
														256,
														NULL,
														SD_GATEKEEPER_TASK_PRIORITY,
														xSD_Card_Gatekeeper_Stack,
														&xSD_Card_Gatekeeper_Task_Buffer);	// Create static task for Logging to the SD_Card
	 //Sync Task
	xSD_Card_Sync_Handle = xTaskCreateStatic(	xSD_Sync,
												"SD_Sync",
												SD_SYNC_TASK_STACK_SIZE,
												NULL,
												SD_SYNC_TASK_PRIORITY,
												xSD_Sync_Stack,
												&xSD_Sync_Task_Buffer);	// Create static task for Logging to the SD_Card

	xSD_Card_Test_Handle = xTaskCreateStatic(	xTest_Sender_Task,
												"SD_Test2",
												256,
												NULL,
												osPriorityNormal,
												xSD_Test_Stack,
												&xSD_Test_Task_Buffer);	// Create static task for Logging to the SD_Card


}//init RTOS tasks


//Private functions	**********************************************************************

//	Read from the SD card
// 	For now, will start from beginning of file and read btr num of bytes from there.
// 	Variables can be static in this function as only one instance of them should run at once within the
// 	SD_Gatekeeper task
//TODO: More extensive error checking
static _Bool SD_Task_Read(int32_t btr, char * buff, FileEnum fileNum){

	//can get away with static as there will only be one instance of xSD_Card_Gatekeeper running
	static FSIZE_t ofs;					//	offset from the beginning of the file
	ofs = f_tell(&fil[fileNum]);		// Get file pointer of open file

	f_res = f_lseek(&fil[fileNum], 0);	// point to beginning of file

	f_res = f_read(&fil[fileNum], buff, btr, &br);		// Read the desired file

	// If the read doesn't work
	if(f_res != FR_OK){
		return 0;
	}

	if(br < btr){
		// Reached End of File during read
		__NOP();
	}//if

	f_res = f_lseek(&fil[fileNum], ofs);	// point back to where we were in the file

	f_res = f_puts("SD Logger - Read From SD Card\n\n", &fil[LogFile]);

	return 1;

}// SD Task Read

//	Write a string to the current log file
//	Should only be called by the SD_Card_Gatekeeper
// int32_t btw (bytes to write)
// char * str (string we are logging)
// FileEnum fileNum (file number)
static _Bool SD_Task_Write(int32_t btw, char * str, FileEnum fileNum){

	static uint32_t len;			// length of string

	if(btw == -1)
		len = strlen(str);			// Dont need this feature but eh
	else
		len = btw;

	// Writing brokey
	if(f_res != FR_OK){
		return 0;
	}
	Time_Stamp(str);
	f_res = f_write(&fil[fileNum], str, len, &bw);							// Write to SD buffer

	f_sync(&fil[fileNum]);

	// Writing brokey
	if(f_res != FR_OK){
		return 0;
	}
	return 1;
}// SD_Task_Write


//Private functions end	**********************************************************************

//Public Functions **********************************************************************


// Log a message to the SD card
// Each message passed through this function will be passed to the SD_Card queue

// Passing a defined length of your string speeds up writing of your error message
// HOWEVER: Passing -1 will make the task compute the length of your error string. (If you're lazy do that)

// WARNING: Must be called within a RTOS Task
// WARNING: Entries must have "\n" at the end to maintain formatting

_Bool SD_Log(char * msg, int32_t bytesToWrite){

	BaseType_t ret;			// RTOS function returns
	SD_Request request;		// Request Struct

	request.type = Write;
	request.buff = msg;
	request.fileName = LogFile;
	request.size = bytesToWrite;

	ret = xQueueSendToBack(xSD_Card_Queue, &request, 0);	//Queue should never have more than one value in it thus wait = 0
	if(ret != pdPASS){
		//ERROR! Queue is full
		return 0;
	}
	return 1;
}//SD_ Log


// Log a message to the SD card
// Each message passed through this function will be passed to the SD_Card queue
// WARNING: Use this function to call from Interrupt
_Bool SD_Log_From_ISR(char * msg, int32_t bytesToWrite){

	// should be able to get away with static as interrupts cannot be spliced
	BaseType_t ret, pxHigherPriorityTaskWoken;			// RTOS function returns
	SD_Request request;		// Request Struct


		request.type = Write;
		request.buff = msg;
		request.fileName = LogFile;
		request.size = bytesToWrite;

	ret = xQueueSendToBackFromISR(	xSD_Card_Queue,
									&request,
									&pxHigherPriorityTaskWoken);

	if(ret != pdPASS || pxHigherPriorityTaskWoken != pdTRUE){

		//	Calling this function should trigger a context switch to SD_Card_Logger
		// 	after the interrupt.
		// 	^thus is pxHigherPriorityTaskWoken is not pdTRUE, that will not happen

		// ret != pdPASS  means that the queue is full, which should not be the case

		return 0;
	}// if error occured

	return 1;
}


// Request a read from the SD card
_Bool SD_Read(char *readBuff, int32_t bytesToRead, FileEnum file){

	BaseType_t ret;			// RTOS function return
	SD_Request request;		// Request Struct


	request.type = Read;
	request.buff = readBuff;
	request.fileName = file;
	request.size = bytesToRead;

	ret = xQueueSendToBack(xSD_Card_Queue, &request, 0);	//Queue should never have more than one value in it thus wait = 0
	if(ret != pdPASS){
		//ERROR! Queue is full
		return 0;	//return
	}

	return 1;
}


//Eject the SD card
_Bool SD_Eject(){

	BaseType_t ret;			// RTOS function return
	SD_Request request;		// Request Struct

	request.type = Eject;	// Request to Eject

	ret = xQueueSendToBack(xSD_Card_Queue, &request, 0);	//Queue should never have more than one value in it thus wait = 0
	if(ret != pdPASS){
		//ERROR! Queue is full
		return 0;	//return
	}
	return 1;
}

//task that timestamps each entry at the gatekeeper
void Time_Stamp(char str){
	RTC_TimeTypeDef timeStruct;
	char rtcTimeBuff[256];

	HAL_RTC_GetTime(&hrtc, &timeStruct, RTC_FORMAT_BCD);			// Get the time of the recording
	sprintf(rtcTimeBuff, "Time: (%2d:%2d:%2.7lf) \n", timeStruct.Hours, timeStruct.Minutes, timeStruct.Seconds+(double)1/timeStruct.SubSeconds);
	strcat(str,rtcTimeBuff);

	return 1;
}


//	Reads and Writes to the SD card upon request of other RTOS tasks.
// 	This Task should have a higher priority within RTOS
// 	than sender Tasks which report to this task through its Queue: xSD_Card_Queue
void xSD_Card_Gatekeeper(void* pvParameters){

	static BaseType_t xStatus;					// storage for RTOS function returns
	static SD_Request sd_req;					// request being sent to SD gatekeeper

	f_res = f_mount(&fs, "", 1);		// mount the SD card's default drive immediately

	if(f_res != FR_OK){
		__NOP();	//debug
		//Send message to Queue of Error Handler ()		//TODO: RTOS Error Handler
	}//if mounted

	f_res = f_mkdir(LOG_DIR_NAME);		// Make the directory if it hasn't been made
	f_res = f_chdir(LOG_DIR_NAME);		// open the logs directory

	f_res = f_open(&fil[LogFile], LOG_FILE_NAME, FA_READ | FA_WRITE | FA_OPEN_APPEND);
	f_res = f_sync(&fil[LogFile]);		// sync so we dont lose the opened file

	for(;;){

		if( uxQueueMessagesWaiting(xSD_Card_Queue) != 0 )
		{
			// Queue not empty when entering task!
			// Error!
			// Trace which task beat this task's priority?
			//__NOP();
		}//if
		if( uxQueueMessagesWaiting(xSD_Card_Queue) == 0 )
				{
			vTaskDelay(pdMS_TO_TICKS(10)); //current has a delay when there is no request

					// Queue not empty when entering task!
					// Error!
					// Trace which task beat this task's priority?
					//__NOP();
				}//if
		// Wait for new request to be sent to the Queue
		xStatus = xQueueReceive(xSD_Card_Queue, &sd_req, 100);	//TODO: Make this timeout and check for errors (We should be constantly logging from BMS)

		// If data received within time frame (if we decide to have a wait time)
		if(xStatus == pdTRUE){

			switch (sd_req.type) {
				case Read:
					SD_Task_Read(sd_req.size, sd_req.buff, sd_req.fileName);
					//Let the SD Sync task know that it can request a sync now during downtime
					xTaskNotifyGive(xSD_Card_Sync_Handle);
					break;
				case Write:
					//Time_Stamp(sd_req);
					SD_Task_Write(sd_req.size, sd_req.buff, sd_req.fileName);
					//Let the SD Sync task know that it can request a sync now during downtime
					xTaskNotifyGive(xSD_Card_Sync_Handle);
					break;
				case Sync:
					//Sync all files
					for(int i=0; i<FS_MAX_CONCURRENT_FILES; i++){
						f_sync(&fil[i]);
					}
					break;
				case Eject:
					//Close all files
					for(int i=0; i<FS_MAX_CONCURRENT_FILES; i++){
						f_close(&fil[i]);
					}
					f_mount(0,"",0);	//unmount the fs
					vTaskSuspend(xSD_Card_Gatekeeper_Handle);	// Suspend this task
					break;
				default:
					break;
			}//switch

		}//if xStatus

	}//for

}// xSD_Card_Gatekeeper



// Low priority Task which will call the Gate-keeper to sync the write buffer to the sd card
// TODO: Add Error Handling
void xSD_Sync(void * pvparameters){

	static BaseType_t ret;			// ret
	static SD_Request syncRequest;		// Request Struct

	uint32_t MS_WAIT = pdMS_TO_TICKS(SD_SYNC_ERROR_CHECK_TIMEOUT);		//	MS to wait for notification

	syncRequest.type = Sync;	// Sync


	for(;;){

		ret = ulTaskNotifyTake(pdTRUE, MS_WAIT);		// enter blocked state and wait for a write

		if(ret == pdTRUE){

			ret = xQueueSendToBack(xSD_Card_Queue, &syncRequest, 0);	//Queue should never have more than one value in it thus wait = 0

			if(ret != pdPASS){
				//ERROR! Queue is full
				__NOP();
			}

		}// if task was notified

		else{
			//Check for errors, did gatekeeper fail?
			__NOP();
		}

		vTaskDelay(MS_WAIT);	//Delay this much at least to sync again.
	}

}//xSD_Sync task


/*
//Test
void xTest_Sender_Task2(void * pvParameters){

	_Bool gotQueued;			// store return value
	uint32_t MS_WAIT = pdMS_TO_TICKS(1000);



	for(;;){

		gotQueued = SD_Log(test2, -1);	// Log

		vTaskDelay(MS_WAIT);	// delay a second
	}

}


//Test
void xTest_Sender_Task1(void * pvParameters){

	_Bool gotQueued;			// store return value
	uint32_t MS_WAIT = pdMS_TO_TICKS(1000);



	for(;;){

		gotQueued = SD_Read(buff2, 10, LogFile);	// Read

		vTaskDelay(MS_WAIT);	// delay a second
	}

}
*/


//Test
void xTest_Sender_Task(void * pvParameters){

	_Bool gotQueued;			// store return value
	uint32_t MS_WAIT = pdMS_TO_TICKS(1000);
	RTC_TimeTypeDef timeStruct;

	char rtcTimeBuff[128];			// Buffer for a rtc time string

	int i = 0;

	for(;;){

		if(i> 1000){
			//gotQueued = SD_Eject();			// Read from the log file
		}
		else{

			HAL_RTC_GetTime(&hrtc, &timeStruct, RTC_FORMAT_BCD);			// Get the time of the recording

			sprintf(rtcTimeBuff, "Time: %2d:%2d:%2.7lf	-> ", timeStruct.Hours, timeStruct.Minutes, timeStruct.Seconds+(double)1/timeStruct.SubSeconds);

			//gotQueued = SD_Log(rtcTimeBuff, -1);
			i++;
		}


		vTaskDelay(MS_WAIT);	// delay a second
	}

	//We broke out of the Loop

}


//HAL_Falling edge EXT interrupt to detect power loss
//Finish writing to file



//END_FILE
