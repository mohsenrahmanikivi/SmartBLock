/*
 * _receiveUART.cpp
 *
 */


#include <_uartRingBufDMA.h>
#include "stm32f4xx_hal.h"
#include "string.h"

extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;



void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if (huart->Instance == USART2)
		{



			/* start the DMA again */
			HAL_UARTEx_ReceiveToIdle_DMA(&huart2, (uint8_t *) RxBuf, RxBuf_SIZE);
			__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);

		}
}






