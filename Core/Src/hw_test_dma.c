/**
 * @file hw_test_dma.c
 * @brief DMA transfer hardware test implementation
 */

#include "hw_test.h"
#include "hw_handles.h"
#include "hw_test_util.h"

/**
 * @brief Test DMA streams
 */
test_result_t test_dma(void)
{
  test_result_t result;
  result.test_id = TEST_ID_DMA;
  result.passed = false;
  hw_test_clear_buffer(result.msg, sizeof(result.msg));

  uint8_t dma_tests_passed = 0;
  char status_log[128] = "";

  /* Test 1: ADC DMA (DMA2_Stream0) */
  uint32_t adc_dma_count = __HAL_DMA_GET_COUNTER(&hdma_adc1);
  if (adc_dma_count > 0)
  {
    dma_tests_passed++;
    hw_test_append_text(status_log, sizeof(status_log), "ADC_DMA_OK ");
  }
  else
  {
    hw_test_append_text(status_log, sizeof(status_log), "ADC_DMA_FAIL ");
  }

  /* Test 2: UART3 RX DMA (DMA1_Stream1) */
  uint32_t uart_dma_count = __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
  if (uart_dma_count > 0)
  {
    dma_tests_passed++;
    hw_test_append_text(status_log, sizeof(status_log), "UART3_DMA_OK");
  }
  else
  {
    hw_test_append_text(status_log, sizeof(status_log), "UART3_DMA_FAIL");
  }

  if (dma_tests_passed >= 1)  /* At least one DMA stream active */
  {
    result.passed = true;
    hw_test_append_text(result.msg, sizeof(result.msg), "DMA: PASS - ");
    hw_test_append_u32_dec(result.msg, sizeof(result.msg), dma_tests_passed);
    hw_test_append_text(result.msg, sizeof(result.msg), " DMA stream(s) active: ");
    hw_test_append_text(result.msg, sizeof(result.msg), status_log);
  }
  else
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "DMA: FAIL - No active DMA streams: ");
    hw_test_append_text(result.msg, sizeof(result.msg), status_log);
  }

  return result;
}
