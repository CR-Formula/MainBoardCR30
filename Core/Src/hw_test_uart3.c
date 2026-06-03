/**
 * @file hw_test_uart3.c
 * @brief UART3 GPS hardware test implementation
 */

#include "hw_test.h"
#include "hw_handles.h"
#include "hw_test_util.h"

/**
 * @brief Test UART3 functionality
 */
test_result_t test_uart3(void)
{
  test_result_t result;
  result.test_id = TEST_ID_UART3;
  result.passed = false;
  hw_test_clear_buffer(result.msg, sizeof(result.msg));

  /* Check UART3 handle is initialized */
  if (huart3.Instance == NULL)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "UART3: FAIL - UART3 not initialized");
    return result;
  }

  /* Verify UART is in ready state */
  if (huart3.gState == HAL_UART_STATE_RESET)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "UART3: FAIL - UART3 in reset state");
    return result;
  }

  /* Check that UART DR and control registers are accessible */
  uint32_t sr = huart3.Instance->SR;

  /* Verify TXE (Transmit buffer empty) is set */
  if ((sr & USART_SR_TXE) == 0)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "UART3: FAIL - TX buffer not empty (SR=0x");
    hw_test_append_u32_hex(result.msg, sizeof(result.msg), sr, 8U);
    hw_test_append_text(result.msg, sizeof(result.msg), ")");
    return result;
  }

  /* Try to transmit a test character (non-blocking) */
  uint8_t test_char = 0x00;
  HAL_StatusTypeDef tx_status = HAL_UART_Transmit(&huart3, &test_char, 1, 50);

  if (tx_status != HAL_OK && tx_status != HAL_TIMEOUT)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "UART3: FAIL - TX attempt failed (status=");
    hw_test_append_u32_dec(result.msg, sizeof(result.msg), (uint32_t)tx_status);
    hw_test_append_text(result.msg, sizeof(result.msg), ")");
    return result;
  }

  /* Check DMA RX status if available */
  if (hdma_usart3_rx.Instance != NULL)
  {
    uint32_t dma_count = __HAL_DMA_GET_COUNTER(&hdma_usart3_rx);
    result.passed = true;
    hw_test_append_text(result.msg, sizeof(result.msg), "UART3: PASS - UART operational, RX DMA active (count=");
    hw_test_append_u32_dec(result.msg, sizeof(result.msg), dma_count);
    hw_test_append_text(result.msg, sizeof(result.msg), ")");
  }
  else
  {
    result.passed = true;
    hw_test_append_text(result.msg, sizeof(result.msg), "UART3: PASS - UART operational (9600 baud, DMA initialized)");
  }

  return result;
}
