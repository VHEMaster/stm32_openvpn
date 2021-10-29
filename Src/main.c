/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>
#include "stm32f7xx_hal.h"
#include "cmsis_os.h"
#include "lwip.h"
#include "mp3common.h"
//#include "mp3test.h"
//#include "network.h"
#include "api.h"
#include "ip_addr.h"
#include "delay.h"
#include "fatfs.h"
#include "usb_host.h"
#include "openvpn_main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s2;
DMA_HandleTypeDef hdma_spi2_tx;

RNG_HandleTypeDef hrng;
CRC_HandleTypeDef hcrc;


SDRAM_HandleTypeDef hsdram1;
SDRAM_HandleTypeDef hsdram2;

osThreadId WebTaskHandle;
osThreadId MP3TaskHandle;

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_CRC_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S2_Init(void);
static void MX_RNG_Init(void);
static void MX_FMC_Init(void);
static void MPU_Config(void);
void StartWebTask(void const * argument);
void StartMP3Task(void const * argument);
void StartUSBTask(void const * argument);
void MX_USB_HOST_Process(void);
RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* StartWebTask function */

void StartUSBTask(void const * argument)
{
  MX_USB_HOST_Init();
	while(1)
	{
		MX_USB_HOST_Process();
		osDelay(1);
	}
}

typedef struct {
	uint8_t * data;
	uint32_t datafilled;
	uint32_t size;
	uint32_t overflow;
	uint32_t writepos;
	uint32_t readpos;
	osThreadId * ReceiveTask; 
	uint32_t requestedsize;
} DynamicQueueTypeDef;

DynamicQueueTypeDef * FrameBuffer;


DynamicQueueTypeDef * DynamicQueueCreate(uint32_t size)
{
	DynamicQueueTypeDef * queue = (DynamicQueueTypeDef*)pvPortMalloc(sizeof(DynamicQueueTypeDef));
	if(queue == NULL) return NULL;
	queue->data = (uint8_t*)pvPortMalloc(sizeof(uint8_t[size]));
	if(queue->data == NULL) { vPortFree(queue); return NULL; }
	
	queue->size = size;
	queue->writepos = 0;
	queue->readpos = 0;
	queue->overflow = 0;
	queue->datafilled = 0;
	
	return queue;
}

uint32_t DynamicQueueSend(DynamicQueueTypeDef * queue, void * data, uint32_t size)
{
	uint32_t i;
	uint32_t uxSavedInterruptStatus = 0;
	if(queue == NULL) return 0;
	if(size > queue->size) return 0;
	if(size > queue->size-queue->datafilled)
	{
		queue->overflow++;
		if(__get_IPSR() == 0)
			while(size > queue->size-queue->datafilled) osDelay(1);
		else return 0;
	}
	
	if(__get_IPSR() != 0) uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	else taskENTER_CRITICAL();
	for(i=0;i<size;i++)
	{
		queue->data[queue->writepos++] = ((uint8_t*)data)[i];
		if(queue->writepos >= queue->size) queue->writepos = 0;
	}
	queue->datafilled+=size;
	if(queue->ReceiveTask != NULL)
	{
		if(queue->datafilled >= queue->requestedsize)
		{
			vTaskResume(queue->ReceiveTask);
			queue->ReceiveTask = NULL;
		}
	}
	if(__get_IPSR() != 0) taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
	else taskEXIT_CRITICAL();
	
	return i;
}

uint32_t DynamicQueueCheck(DynamicQueueTypeDef * queue)
{
	if(queue == NULL) return 0;
	return queue->datafilled;
}

uint32_t DynamicQueueReceive(DynamicQueueTypeDef * queue, void * data, uint32_t size)
{
	uint32_t i;
	uint32_t uxSavedInterruptStatus = 0;
	if(queue == NULL) return 0;
	queue->requestedsize = size;
	
	while(queue->datafilled < queue->requestedsize)
	{
		queue->ReceiveTask = xTaskGetCurrentTaskHandle();
		vTaskSuspend(queue->ReceiveTask);
	}
	queue->ReceiveTask = NULL;
	
	if(__get_IPSR() != 0) uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	else taskENTER_CRITICAL();
	for(i=0;i<size;i++)
	{
		((uint8_t*)data)[i] = queue->data[queue->readpos++];
		if(queue->readpos >= queue->size) queue->readpos = 0;
	}
	queue->datafilled -= size;
	queue->requestedsize -= size;
	if(__get_IPSR() != 0) taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
	else taskEXIT_CRITICAL();
	
	return queue->datafilled;
}

uint32_t DataReceiveAll(DynamicQueueTypeDef * queue, void * data)
{
	uint32_t i,size;
	uint32_t uxSavedInterruptStatus = 0;
	if(queue == NULL) return 0;
	
	if(__get_IPSR() != 0) uxSavedInterruptStatus = taskENTER_CRITICAL_FROM_ISR();
	else taskENTER_CRITICAL();
	size = queue->datafilled; 
	for(i=0;i<size;i++)
	{
		while(queue->datafilled == 0)
		{
			queue->ReceiveTask = xTaskGetCurrentTaskHandle();
			queue->requestedsize = size-i;
			taskEXIT_CRITICAL();
			vTaskSuspend(queue->ReceiveTask);
			taskENTER_CRITICAL();
		}
		((uint8_t*)data)[i] = queue->data[queue->readpos];
		if(++queue->readpos >= queue->size)
			queue->readpos = 0;
		queue->datafilled--;
	}
	if(__get_IPSR() != 0) taskEXIT_CRITICAL_FROM_ISR(uxSavedInterruptStatus);
	else taskEXIT_CRITICAL();
	
	return size;
}

#define DAC_ADDR 0x94
uint8_t DAC_Read(uint8_t addr)
{
	if(HAL_I2C_Master_Transmit(&hi2c1,DAC_ADDR,&addr,1,1000) != HAL_OK)
		return 0xFF;
	if(HAL_I2C_Master_Receive(&hi2c1,DAC_ADDR+1,&addr,1,1000) != HAL_OK)
		return 0xFE;
	return addr;
}

uint8_t DAC_Write(uint8_t addr, uint8_t data)
{
	uint8_t d[2] = {addr,data};
	if(HAL_I2C_Master_Transmit(&hi2c1,DAC_ADDR,d,2,1000) != HAL_OK)
		return 0xFF;
	return 0;
}

uint8_t * MP3RAWDATA = (uint8_t *)0x080C0000;
uint32_t MP3RAWDATASIZE = 160958;

extern struct netif gnetif;
extern struct dhcp gdhcp;
char netbuff[512];
uint32_t DEBUG = 0;
void StartWebTask(void const * argument)
{
  /* init code for LWIP */
	volatile int err;
	struct netconn * fd;
	struct netbuf * nb;
	ip_addr_t IP;
	char * data;
	uint16_t lng,i;
	uint32_t started,bytes,last,all;
	printf("Hello\r\n");
	
  MX_FATFS_Init();
	
	
	HAL_GPIO_WritePin(LED_D3_GPIO_Port, LED_D3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(ETH_NRST_GPIO_Port, ETH_NRST_Pin, GPIO_PIN_SET);
  MX_LWIP_Init();
	
	
	while(gnetif.ip_addr.u_addr.ip4.addr == 0)
	{
		HAL_GPIO_TogglePin(LED_D4_GPIO_Port, LED_D4_Pin);
		osDelay(100);
	}
	HAL_GPIO_WritePin(LED_D4_GPIO_Port, LED_D4_Pin, GPIO_PIN_SET);
	
	Openvpn_Task_Init();
	while(1) osDelay(1000);
	
	
	FrameBuffer = DynamicQueueCreate(65536);
	
	fd = netconn_new(NETCONN_TCP);
	netconn_bind(fd, &(gnetif.ip_addr), 0); 
	netconn_gethostbyname("online-kissfm.tavrmedia.ua", &IP); 
	while(1)
	{
		HAL_GPIO_WritePin(LED_D5_GPIO_Port, LED_D5_Pin, GPIO_PIN_RESET);
		
		err = netconn_connect(fd, &IP, 80);
		if(err < 0)
		{
			printf("Failed to connect\r\n");
			osDelay(500);
			continue;
		}
		 
		sprintf(netbuff, "GET /KissFM_Digital HTTP/1.1\r\nhost: online-kissfm.tavrmedia.ua\r\n\r\n");
		netconn_write(fd, netbuff, strlen(netbuff), NETCONN_COPY);
		
		vTaskPrioritySet(MP3TaskHandle, osPriorityAboveNormal);
		
		started = last = Delay_Tick;
		bytes = all = 0;
		for(;;)
		{
			osDelay(1);
			HAL_GPIO_TogglePin(LED_D5_GPIO_Port, LED_D5_Pin);
			
			err = netconn_recv(fd, &nb);
			if(err < 0) break;
			lng = 512;
			lng = netbuf_len(nb);
			err = netbuf_copy(nb,netbuff,lng); 
			if(err < 0) break;
			netbuf_delete(nb);
			
			
			all += lng;
			bytes += lng;
			if(Delay_Tick-last > 5000000)
			{
				printf("Res: % 4dms - %.2fkbps  avg %.2fkbps\r\n", (Delay_Tick-last)/1000, ((float)bytes/128.0f)/((Delay_Tick-last)/1000000.0f),((float)all/128.0f)/((Delay_Tick-started)/1000000.0f));
				last = Delay_Tick;
				bytes = 0;
			}
			DynamicQueueSend(FrameBuffer,netbuff,lng);
			DEBUG = FrameBuffer->datafilled;
		}
	}
  /* USER CODE END 5 */ 
}

/* StartMP3Task function */


volatile uint8_t dacstep = 0;
HMP3Decoder MP3;
MP3FrameInfo MP3Info;
short int SampleBuffer[2304*2];
#define MP3BufferSize 1024
uint8_t mp3buff[MP3BufferSize];
uint32_t MP3ERRORS = 0;
uint32_t MP3DECODEERRORS = 0;
uint32_t MP3SUCCESS = 0;

uint8_t FreqMode = 0;
void FreqUp(void)
{
	/*
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 113; //113
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 5; //5
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	*/
  __HAL_RCC_PLLI2S_CONFIG(159 , RCC_PLLP_DIV2, 2, 7);
}
void FreqDown(void)
{
	/*
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 158; //112
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 7; //7
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	*/
  __HAL_RCC_PLLI2S_CONFIG(158 , RCC_PLLP_DIV2, 2, 7);
}

void StartMP3Task(void const * argument)
{
	int syncword;
	uint16_t shift,i;
	uint16_t left = 0;
	uint8_t * tempbuffer;
	int16_t * samplebuffer;
  MP3 = MP3InitDecoder();
	
	printf("Started!\r\n");
  for(;;)
  {
		
		if(DynamicQueueCheck(FrameBuffer) < 2048)
		{
			DAC_Write(0x04,0xFF); //Mute
			for(i=0;i<2304*2;i++)
				SampleBuffer[i] = 0;
			SCB_CleanDCache_by_Addr((uint32_t*)SampleBuffer, 2304*4);
			while(DynamicQueueCheck(FrameBuffer) < 32768)
			{
				osDelay(1);
			}
			DAC_Write(0x04,0xAA); //Unmute
		}
		if(DynamicQueueCheck(FrameBuffer) > 55296)
		{
			DAC_Write(0x04,0xFF); //Mute
			for(i=0;i<2304*2;i++)
				SampleBuffer[i] = 0;
			SCB_CleanDCache_by_Addr((uint32_t*)SampleBuffer, 2304*4);
			while(DynamicQueueCheck(FrameBuffer) > 20480)
			{
				DynamicQueueReceive(FrameBuffer, NULL, 10240);
			}
			DAC_Write(0x04,0xAA); //Unmute
		}
		
		if(DynamicQueueCheck(FrameBuffer) < 10240)
		{
			if(FreqMode != 2)
			{
				FreqMode = 2;
				FreqDown();
			}
		}
		else if(DynamicQueueCheck(FrameBuffer) > 20480) //57344
		{
			if(FreqMode != 1)
			{
				FreqMode = 1;
				FreqUp();
			}
		}
		
		DynamicQueueReceive(FrameBuffer,&mp3buff[left],MP3BufferSize-left);
		syncword = MP3FindSyncWord(mp3buff, MP3BufferSize);
		//printf("MP3FindSyncWord - %d\r\n",syncword);
		if(syncword < 0)
		{
			left = 0;
			continue;
		}
		else if(syncword > 0)
		{
			left = MP3BufferSize-syncword;
			for(i=0;i<left;i++)
				mp3buff[i] = mp3buff[i+syncword];
			DynamicQueueReceive(FrameBuffer,&mp3buff[left],syncword);
		}
		left = MP3BufferSize;
		syncword = MP3GetNextFrameInfo(MP3, &MP3Info, mp3buff);
		//printf("MP3GetNextFrameInfo - %d\r\n",syncword);
		if(syncword != 0)
		{
			MP3ERRORS++;
			left-=2;
			shift = MP3BufferSize-left;
			for(i=0;i<left;i++)
				mp3buff[i] = mp3buff[i+shift];
			continue;
		}
		
		tempbuffer = mp3buff;
		
		while(dacstep == 0) osDelay(1);
		
		if(((dacstep&5) > 0) && ((dacstep&0xC) != 8))
		{
			samplebuffer = SampleBuffer;
			syncword = MP3Decode(MP3, &tempbuffer, (int*)&left, samplebuffer, 0);
			SCB_CleanDCache_by_Addr((uint32_t*)samplebuffer, 2304*2);
			dacstep &= ~5;
		}
		if(((dacstep&10) > 0) && ((dacstep&0xC) != 4))
		{
			samplebuffer = &SampleBuffer[2304];
			syncword = MP3Decode(MP3, &tempbuffer, (int*)&left, samplebuffer, 0);
			SCB_CleanDCache_by_Addr((uint32_t*)samplebuffer, 2304*2);
			dacstep &= ~10;
		}
		
		//printf("MP3Decode - %d, left %d\r\n",syncword,left);
		
		HAL_GPIO_TogglePin(LED_D6_GPIO_Port, LED_D6_Pin);
		if(syncword != 0)
		{
			MP3DECODEERRORS++;
			left = 0;
			continue;
		}
		MP3GetLastFrameInfo(MP3, &MP3Info);
		MP3SUCCESS++;
		
		shift = MP3BufferSize-left;
		for(i=0;i<left;i++)
			mp3buff[i] = mp3buff[i+shift];
		
  }
  /* USER CODE END StartMP3Task */
}



void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s)
{
	dacstep |= 1;
	if(((dacstep&3) == 3) && ((dacstep&0xC) != 8)) dacstep |= 4;
}
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
	dacstep |= 2;
	if(((dacstep&3) == 3) && ((dacstep&0xC) != 4)) dacstep |= 8;
}

void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct;

  /* Disables the MPU */
  HAL_MPU_Disable();
    /**Initializes and configures the Region and the memory to be protected 
    */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER0;
  MPU_InitStruct.BaseAddress = 0x08000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_1MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_PRIV_RO_URO;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

    /**Initializes and configures the Region and the memory to be protected 
    */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER1;
  MPU_InitStruct.BaseAddress = 0x20000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_512KB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);
	
	
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = MPU_REGION_NUMBER2;
  MPU_InitStruct.BaseAddress = 0x60000000;
  MPU_InitStruct.Size = MPU_REGION_SIZE_256MB;
  MPU_InitStruct.SubRegionDisable = 0x0;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;

  HAL_MPU_ConfigRegion(&MPU_InitStruct);

  /* Enables the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

}


int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */
	
  /* MCU Configuration----------------------------------------------------------*/
  //MPU_Config();

  /* Enable I-Cache-------------------------------------------------------------*/
  SCB_EnableDCache();
  SCB_EnableICache();


  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
	MX_FMC_Init();
  MX_I2C1_Init();
  MX_I2S2_Init();
  MX_RNG_Init();
	DelayInit();

  /* USER CODE BEGIN 2 */

	HAL_GPIO_WritePin(I2S2_NRST_GPIO_Port, I2S2_NRST_Pin, GPIO_PIN_SET);
	HAL_Delay(10);
	
	while((DAC_Read(0x01)&0xF8) != 0xE0)
	{
		HAL_Delay(100);
	}
	HAL_GPIO_WritePin(LED_D6_GPIO_Port, LED_D6_Pin, GPIO_PIN_SET);
	
	DAC_Write(0x01,0x01);
	
	DAC_Write(0x05,0xA1);
	DAC_Write(0x06,0x04);
	DAC_Write(0x0F,0x06);
	DAC_Write(0x24,0x00);
	DAC_Write(0x25,0x00);
	DAC_Write(0x0A,0x00);
	DAC_Write(0x0E,0x04);
	DAC_Write(0x27,0x00);
	DAC_Write(0x1F,0x88);
	
	//VOLUME
	DAC_Write(0x1A,0);
	DAC_Write(0x1B,0);
	
	DAC_Write(0x22,-80);
	DAC_Write(0x23,-80);
	
	DAC_Write(0x04,0xAA); //Unmute
	//DAC_Write(0x04,0xFF); //Mute
	
	DAC_Write(0x02,0x9E); //PowerUP
	//DAC_Write(0x02,0x01); //PowerDown
	

	//DAC_Write(0x04,0xFF); //Mute
	
	DAC_Write(0x22,0);
	DAC_Write(0x23,0);
	
	
	DAC_Write(0x22,-30);
	DAC_Write(0x23,-30);
	
	
	
	
	
	HAL_I2S_Transmit_DMA(&hi2s2,(uint16_t*)SampleBuffer,2304*2);  
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of WebTask */
  osThreadDef(WebTask, StartWebTask, osPriorityNormal, 0, 4096);
  WebTaskHandle = osThreadCreate(osThread(WebTask), NULL);

  /* definition and creation of MP3Task */
  //osThreadDef(MP3Task, StartMP3Task, osPriorityIdle, 0, 2048); //osPriorityIdle
  //MP3TaskHandle = osThreadCreate(osThread(MP3Task), NULL);

  /* definition and creation of MP3Task */
  osThreadDef(USBTask, StartUSBTask, osPriorityNormal, 0, 2048);
  osThreadCreate(osThread(USBTask), NULL);
	

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
 

  /* Start scheduler */
  osKernelStart();
  
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  //RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 432;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Activate the Over-Drive mode 
    */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
	

  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2S
                              |RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.PLLI2S.PLLI2SN = 113; //123 - 48k, 113 - 44k
  PeriphClkInitStruct.PLLI2S.PLLI2SP = RCC_PLLP_DIV2;
  PeriphClkInitStruct.PLLI2S.PLLI2SR = 5;
  PeriphClkInitStruct.PLLI2S.PLLI2SQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 192;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV4;
  PeriphClkInitStruct.PLLI2SDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_2;
  PeriphClkInitStruct.I2sClockSelection = RCC_I2SCLKSOURCE_PLLI2S;
  PeriphClkInitStruct.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLLSAIP; //RCC_CLK48SOURCE_PLL RCC_CLK48SOURCE_PLLSAIP
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x20404768;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Analogue filter 
    */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure Digital filter 
    */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* CRC init function */
static void MX_CRC_Init(void)
{

  __HAL_RCC_CRC_CLK_ENABLE();
  hcrc.Instance = CRC;
  hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
  hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
  hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
  hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
  hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_BYTES;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }

}

/* I2S2 init function */
static void MX_I2S2_Init(void)
{

  hi2s2.Instance = SPI2;
  hi2s2.Init.Mode = I2S_MODE_MASTER_TX;
  hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
  hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B;
  hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
  hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_48K;
  hi2s2.Init.CPOL = I2S_CPOL_LOW;
  hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
  if (HAL_I2S_Init(&hi2s2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/* RNG init function */
static void MX_RNG_Init(void)
{

  hrng.Instance = RNG;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 9, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);

}

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOJ_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(I2S2_NRST_GPIO_Port, I2S2_NRST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LED_D3_Pin|LED_D4_Pin|LED_D5_Pin|LED_D6_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ETH_NRST_GPIO_Port, ETH_NRST_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : BUT_G6_Pin BUT_G7_Pin */
  GPIO_InitStruct.Pin = BUT_G6_Pin|BUT_G7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : I2S2_NRST_Pin */
  GPIO_InitStruct.Pin = I2S2_NRST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(I2S2_NRST_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_D3_Pin LED_D4_Pin LED_D5_Pin LED_D6_Pin */
  GPIO_InitStruct.Pin = LED_D3_Pin|LED_D4_Pin|LED_D5_Pin|LED_D6_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : ETH_NRST_Pin */
  GPIO_InitStruct.Pin = ETH_NRST_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ETH_NRST_GPIO_Port, &GPIO_InitStruct);

}

/* FMC initialization function */
static void MX_FMC_Init(void)
{
  FMC_SDRAM_TimingTypeDef SdramTiming;
	FMC_SDRAM_CommandTypeDef SdramCommand;

  /** Perform the SDRAM1 memory initialization sequence
  */
	
  hsdram1.Instance = FMC_SDRAM_DEVICE;
	
  hsdram1.Init.SDBank = FMC_SDRAM_BANK1;
  hsdram1.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_10;
  hsdram1.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
  hsdram1.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_32;
  hsdram1.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram1.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_3;
  hsdram1.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram1.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram1.Init.ReadBurst = FMC_SDRAM_RBURST_DISABLE;
  hsdram1.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_2;
	
  SdramTiming.LoadToActiveDelay = 4;
  SdramTiming.ExitSelfRefreshDelay = 6;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 8;
  SdramTiming.WriteRecoveryTime = 4;
  SdramTiming.RPDelay = 4;
  SdramTiming.RCDDelay = 4;

  if (HAL_SDRAM_Init(&hsdram1, &SdramTiming) != HAL_OK)
  {
    Error_Handler();
  }
	HAL_EnableFMCMemorySwapping();
	
	/*
	hsdram2.Instance = FMC_SDRAM_DEVICE;
  hsdram2.Init.SDBank = FMC_SDRAM_BANK2;
  hsdram2.Init.ColumnBitsNumber = FMC_SDRAM_COLUMN_BITS_NUM_10;
  hsdram2.Init.RowBitsNumber = FMC_SDRAM_ROW_BITS_NUM_13;
  hsdram2.Init.MemoryDataWidth = FMC_SDRAM_MEM_BUS_WIDTH_32;
  hsdram2.Init.InternalBankNumber = FMC_SDRAM_INTERN_BANKS_NUM_4;
  hsdram2.Init.CASLatency = FMC_SDRAM_CAS_LATENCY_2;
  hsdram2.Init.WriteProtection = FMC_SDRAM_WRITE_PROTECTION_DISABLE;
  hsdram2.Init.SDClockPeriod = FMC_SDRAM_CLOCK_PERIOD_2;
  hsdram2.Init.ReadBurst = FMC_SDRAM_RBURST_ENABLE;
  hsdram2.Init.ReadPipeDelay = FMC_SDRAM_RPIPE_DELAY_0;
	
  SdramTiming.LoadToActiveDelay = 2;
  SdramTiming.ExitSelfRefreshDelay = 6;
  SdramTiming.SelfRefreshTime = 4;
  SdramTiming.RowCycleDelay = 6;
  SdramTiming.WriteRecoveryTime = 2;
  SdramTiming.RPDelay = 2;
  SdramTiming.RCDDelay = 2;

  if (HAL_SDRAM_Init(&hsdram2, &SdramTiming) != HAL_OK)
  {
    Error_Handler();
  }
	*/
  //SYSCFG->MEMRMP |= SYSCFG_MEMRMP_SWP_FMC_0;
	
	SdramCommand.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
  SdramCommand.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  SdramCommand.AutoRefreshNumber = 1;
  SdramCommand.ModeRegisterDefinition = 0;
  HAL_SDRAM_SendCommand(&hsdram1, &SdramCommand,1000);
	
	HAL_Delay(2);

  SdramCommand.CommandMode = FMC_SDRAM_CMD_PALL;
  SdramCommand.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  SdramCommand.AutoRefreshNumber = 1;
  SdramCommand.ModeRegisterDefinition = 0;
  HAL_SDRAM_SendCommand(&hsdram1, &SdramCommand,1000);
	
	HAL_Delay(2);

  SdramCommand.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  SdramCommand.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  SdramCommand.AutoRefreshNumber = 8;
  SdramCommand.ModeRegisterDefinition = 0;
  HAL_SDRAM_SendCommand(&hsdram1, &SdramCommand,1000);
	
	HAL_Delay(2);

  SdramCommand.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
  SdramCommand.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1;
  SdramCommand.AutoRefreshNumber = 1;
  SdramCommand.ModeRegisterDefinition = (uint32_t)0x0230;
  HAL_SDRAM_SendCommand(&hsdram1, &SdramCommand,1000);
	
	HAL_Delay(2);
	/*
	
	SdramCommand.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE;
  SdramCommand.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
  SdramCommand.AutoRefreshNumber = 1;
  SdramCommand.ModeRegisterDefinition = 0;
  HAL_SDRAM_SendCommand(&hsdram2, &SdramCommand,1000);
	
	HAL_Delay(2);

  SdramCommand.CommandMode = FMC_SDRAM_CMD_PALL;
  SdramCommand.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
  SdramCommand.AutoRefreshNumber = 1;
  SdramCommand.ModeRegisterDefinition = 0;
  HAL_SDRAM_SendCommand(&hsdram2, &SdramCommand,1000);
	
	HAL_Delay(2);

  SdramCommand.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  SdramCommand.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
  SdramCommand.AutoRefreshNumber = 8;
  SdramCommand.ModeRegisterDefinition = 0;
  HAL_SDRAM_SendCommand(&hsdram2, &SdramCommand,1000);
	
	HAL_Delay(2);

  SdramCommand.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
  SdramCommand.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK2;
  SdramCommand.AutoRefreshNumber = 1;
  SdramCommand.ModeRegisterDefinition = (uint32_t)0x0220;
  HAL_SDRAM_SendCommand(&hsdram2, &SdramCommand,1000);
	
	HAL_Delay(2);
	*/
	HAL_SDRAM_ProgramRefreshRate(&hsdram1, 1600);
	//HAL_SDRAM_ProgramRefreshRate(&hsdram2, 1600);
	HAL_Delay(2);

}


/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void _Error_Handler(char * file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler_Debug */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
