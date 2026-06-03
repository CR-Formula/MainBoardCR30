/**
 * @file hw_test.h
 * @brief Hardware test framework header
 */

#ifndef INC_HW_TEST_H_
#define INC_HW_TEST_H_

#include <stdint.h>
#include <stdbool.h>

/* ============================================================================
 * TEST ENABLE/DISABLE FLAGS
 * Comment out any #define to disable that test
 * ============================================================================ */
#define TEST_ADC_ENABLE     1
#define TEST_CAN_ENABLE     1
#define TEST_I2C_ENABLE     1
#define TEST_LORA_ENABLE    1
#define TEST_SD_ENABLE      1
#define TEST_DMA_ENABLE     1
#define TEST_UART3_ENABLE   1

/* Test ID assignments (used for LED blink codes) */
#define TEST_ID_ADC         1
#define TEST_ID_CAN         2
#define TEST_ID_I2C         3
#define TEST_ID_LORA        4
#define TEST_ID_SD          5
#define TEST_ID_DMA         6
#define TEST_ID_UART3       7

/* ============================================================================
 * Test Result Structure
 * ============================================================================ */
typedef struct
{
  uint8_t test_id;
  bool passed;
  char msg[256];
} test_result_t;

/* ============================================================================
 * Test Function Prototypes
 * ============================================================================ */

void run_all_tests(void);

/**
 * @brief Test ADC functionality
 * @return Test result structure with pass/fail and message
 */
test_result_t test_adc(void);

/**
 * @brief Test CAN bus functionality
 * @return Test result structure with pass/fail and message
 */
test_result_t test_can(void);

/**
 * @brief Test I2C bus functionality
 * @return Test result structure with pass/fail and message
 */
test_result_t test_i2c(void);

/**
 * @brief Test LoRa SPI module initialization and communication
 * @return Test result structure with pass/fail and message
 */
test_result_t test_lora(void);

/**
 * @brief Test SD card detection and write
 * @return Test result structure with pass/fail and message
 */
test_result_t test_sd(void);

/**
 * @brief Test DMA transfers
 * @return Test result structure with pass/fail and message
 */
test_result_t test_dma(void);

/**
 * @brief Test UART3 functionality
 * @return Test result structure with pass/fail and message
 */
test_result_t test_uart3(void);

#endif /* INC_HW_TEST_H_ */
