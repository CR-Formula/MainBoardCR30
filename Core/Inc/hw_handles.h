/*
 * @file hw_handles.h
 * @brief Shared peripheral handle declarations for hardware tests.
 */

#ifndef INC_HW_HANDLES_H_
#define INC_HW_HANDLES_H_

#include "stm32f4xx_hal.h"

typedef HAL_StatusTypeDef hw_handles_status_t;
static const hw_handles_status_t hw_handles_anchor_status = HAL_OK;

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;
extern CAN_HandleTypeDef hcan1;
extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi2;
extern UART_HandleTypeDef huart3;
extern DMA_HandleTypeDef hdma_usart3_rx;

#endif /* INC_HW_HANDLES_H_ */