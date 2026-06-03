/**
 * @file hw_test_can.c
 * @brief CAN1 bus hardware test implementation
 */

#include "hw_test.h"
#include "hw_handles.h"
#include "hw_test_util.h"

void osDelay(uint32_t ticks);
uint32_t osKernelGetTickCount(void);

/**
 * @brief Test CAN1 functionality
 */
test_result_t test_can(void)
{
  test_result_t result;
  result.test_id = TEST_ID_CAN;
  result.passed = false;
  hw_test_clear_buffer(result.msg, sizeof(result.msg));

  /* Configure CAN RX filter to accept all messages */
  CAN_FilterTypeDef filter;
  filter.FilterBank = 0;
  filter.FilterMode = CAN_FILTERMODE_IDLIST;
  filter.FilterScale = CAN_FILTERSCALE_32BIT;
  filter.FilterIdHigh = 0x0000;
  filter.FilterIdLow = 0x0000;
  filter.FilterMaskIdHigh = 0x0000;
  filter.FilterMaskIdLow = 0x0000;
  filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
  filter.FilterActivation = ENABLE;
  filter.SlaveStartFilterBank = 14;

  if (HAL_CAN_ConfigFilter(&hcan1, &filter) != HAL_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "CAN: FAIL - Filter configuration failed");
    return result;
  }

  /* Start CAN */
  if (HAL_CAN_Start(&hcan1) != HAL_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "CAN: FAIL - Failed to start CAN");
    return result;
  }

  /* Prepare TX message */
  CAN_TxHeaderTypeDef tx_header;
  uint8_t tx_data[8] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00, 0x11};
  uint32_t tx_mailbox;

  tx_header.StdId = 0x123;
  tx_header.ExtId = 0;
  tx_header.IDE = CAN_ID_STD;
  tx_header.RTR = CAN_RTR_DATA;
  tx_header.DLC = 8;
  tx_header.TransmitGlobalTime = DISABLE;

  /* Transmit test frame */
  if (HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_data, &tx_mailbox) != HAL_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "CAN: FAIL - TX message failed");
    return result;
  }

  /* Wait for message to be sent */
  osDelay(10);

  /* Check for received message with timeout */
  uint32_t timeout = 500;  /* ms */
  uint32_t start_time = osKernelGetTickCount();

  while ((osKernelGetTickCount() - start_time) < timeout)
  {
    if (HAL_CAN_GetRxFifoFillLevel(&hcan1, CAN_RX_FIFO0) > 0)
    {
      /* Message received */
      CAN_RxHeaderTypeDef rx_header;
      uint8_t rx_data[8];

      if (HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &rx_header, rx_data) == HAL_OK)
      {
        /* Verify frame content */
        if (rx_header.StdId == tx_header.StdId && rx_header.DLC == tx_header.DLC)
        {
          if (hw_test_bytes_equal(rx_data, tx_data, 8))
          {
            result.passed = true;
            hw_test_append_text(result.msg, sizeof(result.msg), "CAN: PASS - Message loopback successful");
            HAL_CAN_Stop(&hcan1);
            return result;
          }
        }
      }
    }
    osDelay(10);
  }

  hw_test_append_text(result.msg, sizeof(result.msg), "CAN: FAIL - RX timeout or data mismatch");
  HAL_CAN_Stop(&hcan1);

  return result;
}
