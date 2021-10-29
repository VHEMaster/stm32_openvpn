#include "random.h"

extern RNG_HandleTypeDef hrng;

static volatile xSemaphoreHandle rnd_mutex = NULL;
HAL_StatusTypeDef RANDOM_Generate(uint32_t * random)
{
	HAL_StatusTypeDef status;
	if(rnd_mutex == NULL)
		rnd_mutex = xSemaphoreCreateMutex();
	xSemaphoreTake(rnd_mutex, 100);
	status = HAL_RNG_GenerateRandomNumber(&hrng, random);
	xSemaphoreGive(rnd_mutex);
	return status;
}

