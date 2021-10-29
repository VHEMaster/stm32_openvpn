#include "openvpn_main.h"
#include "lwip.h"
#include "openvpn.h"

static void openvpn_task(void const * argument);
osThreadId OpenvpnTaskHandle;

void Openvpn_Task_Init(void)
{
  osThreadDef(openvpn, openvpn_task, osPriorityNormal, 0, 65536);
  OpenvpnTaskHandle = osThreadCreate(osThread(openvpn), NULL);
}

FATFS fs;

static void openvpn_task(void const * argument)
{
	char * args[] = { "openvpn", "stm32f7openvpn.conf" };
	//char * args[] = { "openvpn", "stm32f7server.conf" };
	
	
	FRESULT result = f_mount(&fs, USBHPath, 1);
	if(result != FR_OK)
	{
		printf("Unable to mount filesystem: %d\r\n", result);
	}
	
	HAL_GPIO_WritePin(LED_D6_GPIO_Port, LED_D6_Pin, GPIO_PIN_RESET);
	openvpn_main(2, args);
	while(1)
	{
		osDelay(1);
	}
}
