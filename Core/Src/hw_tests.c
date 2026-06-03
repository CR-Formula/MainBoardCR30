/**
 * @file hw_tests.c
 * @brief Hardware test orchestrator
 */

#include "hw_test.h"
#include "led_blink.h"
#include "sd_log.h"
#include "hw_test_util.h"

/* Array to track failed tests */
static uint8_t failed_tests[7];
static uint8_t failed_count = 0;

/**
 * @brief Run all hardware tests and manage results
 */
void run_all_tests(void)
{
  /* Initialize variables */
  failed_count = 0;
  hw_test_clear_buffer((char *)failed_tests, sizeof(failed_tests));

  char log_line[300];
  test_result_t result;

  /* Test 1: ADC */
#ifdef TEST_ADC_ENABLE
  result = test_adc();
  hw_test_clear_buffer(log_line, sizeof(log_line));
  hw_test_append_text(log_line, sizeof(log_line), "[Test ");
  hw_test_append_u32_dec(log_line, sizeof(log_line), result.test_id);
  hw_test_append_text(log_line, sizeof(log_line), "] ");
  hw_test_append_text(log_line, sizeof(log_line), result.msg);
  sd_log_write(log_line);
  if (!result.passed)
  {
    failed_tests[failed_count++] = result.test_id;
  }
#endif

  /* Test 2: CAN */
#ifdef TEST_CAN_ENABLE
  result = test_can();
  hw_test_clear_buffer(log_line, sizeof(log_line));
  hw_test_append_text(log_line, sizeof(log_line), "[Test ");
  hw_test_append_u32_dec(log_line, sizeof(log_line), result.test_id);
  hw_test_append_text(log_line, sizeof(log_line), "] ");
  hw_test_append_text(log_line, sizeof(log_line), result.msg);
  sd_log_write(log_line);
  if (!result.passed)
  {
    failed_tests[failed_count++] = result.test_id;
  }
#endif

  /* Test 3: I2C */
#ifdef TEST_I2C_ENABLE
  result = test_i2c();
  hw_test_clear_buffer(log_line, sizeof(log_line));
  hw_test_append_text(log_line, sizeof(log_line), "[Test ");
  hw_test_append_u32_dec(log_line, sizeof(log_line), result.test_id);
  hw_test_append_text(log_line, sizeof(log_line), "] ");
  hw_test_append_text(log_line, sizeof(log_line), result.msg);
  sd_log_write(log_line);
  if (!result.passed)
  {
    failed_tests[failed_count++] = result.test_id;
  }
#endif

  /* Test 4: LoRa */
#ifdef TEST_LORA_ENABLE
  result = test_lora();
  hw_test_clear_buffer(log_line, sizeof(log_line));
  hw_test_append_text(log_line, sizeof(log_line), "[Test ");
  hw_test_append_u32_dec(log_line, sizeof(log_line), result.test_id);
  hw_test_append_text(log_line, sizeof(log_line), "] ");
  hw_test_append_text(log_line, sizeof(log_line), result.msg);
  sd_log_write(log_line);
  if (!result.passed)
  {
    failed_tests[failed_count++] = result.test_id;
  }
#endif

  /* Test 5: SD Card */
#ifdef TEST_SD_ENABLE
  result = test_sd();
  hw_test_clear_buffer(log_line, sizeof(log_line));
  hw_test_append_text(log_line, sizeof(log_line), "[Test ");
  hw_test_append_u32_dec(log_line, sizeof(log_line), result.test_id);
  hw_test_append_text(log_line, sizeof(log_line), "] ");
  hw_test_append_text(log_line, sizeof(log_line), result.msg);
  sd_log_write(log_line);
  if (!result.passed)
  {
    failed_tests[failed_count++] = result.test_id;
  }
#endif

  /* Test 6: DMA */
#ifdef TEST_DMA_ENABLE
  result = test_dma();
  hw_test_clear_buffer(log_line, sizeof(log_line));
  hw_test_append_text(log_line, sizeof(log_line), "[Test ");
  hw_test_append_u32_dec(log_line, sizeof(log_line), result.test_id);
  hw_test_append_text(log_line, sizeof(log_line), "] ");
  hw_test_append_text(log_line, sizeof(log_line), result.msg);
  sd_log_write(log_line);
  if (!result.passed)
  {
    failed_tests[failed_count++] = result.test_id;
  }
#endif

  /* Test 7: UART3 */
#ifdef TEST_UART3_ENABLE
  result = test_uart3();
  hw_test_clear_buffer(log_line, sizeof(log_line));
  hw_test_append_text(log_line, sizeof(log_line), "[Test ");
  hw_test_append_u32_dec(log_line, sizeof(log_line), result.test_id);
  hw_test_append_text(log_line, sizeof(log_line), "] ");
  hw_test_append_text(log_line, sizeof(log_line), result.msg);
  sd_log_write(log_line);
  if (!result.passed)
  {
    failed_tests[failed_count++] = result.test_id;
  }
#endif

  /* Close log file */
  sd_log_close();

  /* Set final LED pattern based on results */
  if (failed_count == 0)
  {
    /* All tests passed - slow blink */
    led_blink_slow();
  }
  else
  {
    /* Some tests failed - cycle through failure codes */
    led_cycle_counts(failed_tests, failed_count);
  }
}
