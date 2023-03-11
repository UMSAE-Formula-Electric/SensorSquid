/*
 * sd_card.h
 *
 *  Created on: Jan 6, 2021
 *      Author: Blane
 */

#ifndef SD_CARD_H_
#define SD_CARD_H_

#include "main.h"
#include "FreeRTOS.h"
#include "queue.h"
#include <stdio.h>
#include "string.h"
#include "usart.h"
#include "rtc.h"
#include "fatfs.h"
#include "task.h"
#include "semphr.h"
#include "stm32F4xx_hal_rtc.h"

// Files that can be accessed
// Enum of open files
//Currently set up to log everything to a single file
//That file can be processed later via a script
typedef enum{
	LogFile,

}FileEnum;


//FATFS
#define FF_BUFFER_SIZE 				1024			//  Max size of a sector
#define FS_MAX_CONCURRENT_FILES		3			// 	Max number of files
#define FS_MAX_FILENAME_SIZE		32			// 	Max size of filenames
//Logging

//Semaphore to handle conflicting SD LOG tasks


//Externalizations
extern QueueHandle_t xSD_Card_Queue;			// Handle for the static queue
extern SemaphoreHandle_t xMutex;			//Handle for the SD logging Mutex


//External Functions
void Init_SD_Card(void);
void Init_SD_RTOS_Tasks(void);
_Bool SD_Log(char * msg, int32_t bytesToWrite);
_Bool SD_Log_From_ISR(char * msg, int32_t bytesToWrite);
_Bool SD_Read(char readBuff[], int32_t bytesToRead, FileEnum file);



//RTOS tasks
void xSD_Card_Gatekeeper(void* pvParameters);
void xSD_Sync(void* pvParameters);

//TESTS
void xTest_Sender_Task(void * pvParameters);
void xTest_Sender_Task1(void * pvParameters);
void xTest_Sender_Task2(void * pvParameters);


#endif /* SD_CARD_H_ */
