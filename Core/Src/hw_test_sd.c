/**
 * @file hw_test_sd.c
 * @brief SD card SDIO hardware test implementation
 */

#include "hw_test.h"
#include "main.h"
#include "hw_test_util.h"
#include "ff.h"

uint32_t osKernelGetTickCount(void);

/**
 * @brief Test SD card detection and write
 */
test_result_t test_sd(void)
{
  test_result_t result;
  result.test_id = TEST_ID_SD;
  result.passed = false;
  hw_test_clear_buffer(result.msg, sizeof(result.msg));

  /* Check SD detection GPIO */
  GPIO_PinState sd_det = HAL_GPIO_ReadPin(uSD_DET_GPIO_Port, uSD_DET_Pin);
  if (sd_det == GPIO_PIN_RESET)
  {
    /* SD card not detected (active low) */
    hw_test_append_text(result.msg, sizeof(result.msg), "SD: FAIL - Card not detected (GPIO=LOW)");
    return result;
  }

  /* Attempt to mount SD card */
  FATFS fs;
  FRESULT res = f_mount(&fs, "", 0);
  if (res != FR_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "SD: FAIL - Mount failed (code=");
    hw_test_append_u32_dec(result.msg, sizeof(result.msg), (uint32_t)res);
    hw_test_append_text(result.msg, sizeof(result.msg), ")");
    return result;
  }

  /* Try to write a test file */
  FIL file;
  res = f_open(&file, "HW_TEST.TXT", FA_CREATE_ALWAYS | FA_WRITE);
  if (res != FR_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "SD: FAIL - File open failed (code=");
    hw_test_append_u32_dec(result.msg, sizeof(result.msg), (uint32_t)res);
    hw_test_append_text(result.msg, sizeof(result.msg), ")");
    f_mount(NULL, "", 0);
    return result;
  }

  /* Write test data with timestamp */
  uint32_t tick = osKernelGetTickCount();
  char test_data[128];
  hw_test_clear_buffer(test_data, sizeof(test_data));
  hw_test_append_text(test_data, sizeof(test_data), "Hardware Test - SD Card Write Test\nTimestamp: ");
  hw_test_append_u32_dec(test_data, sizeof(test_data), tick);
  hw_test_append_text(test_data, sizeof(test_data), " ms\n");

  uint32_t bytes_written = 0;
  res = f_write(&file, test_data, hw_test_strnlen_local(test_data, sizeof(test_data)), (UINT *)&bytes_written);
  if (res != FR_OK || bytes_written == 0)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "SD: FAIL - Write failed (code=");
    hw_test_append_u32_dec(result.msg, sizeof(result.msg), (uint32_t)res);
    hw_test_append_text(result.msg, sizeof(result.msg), ", bytes=");
    hw_test_append_u32_dec(result.msg, sizeof(result.msg), bytes_written);
    hw_test_append_text(result.msg, sizeof(result.msg), ")");
    f_close(&file);
    f_mount(NULL, "", 0);
    return result;
  }

  /* Close and unmount */
  res = f_close(&file);
  if (res != FR_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "SD: FAIL - File close failed (code=");
    hw_test_append_u32_dec(result.msg, sizeof(result.msg), (uint32_t)res);
    hw_test_append_text(result.msg, sizeof(result.msg), ")");
    f_mount(NULL, "", 0);
    return result;
  }

  res = f_mount(NULL, "", 0);
  if (res != FR_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "SD: FAIL - Unmount failed (code=");
    hw_test_append_u32_dec(result.msg, sizeof(result.msg), (uint32_t)res);
    hw_test_append_text(result.msg, sizeof(result.msg), ")");
    return result;
  }

  /* Success */
  result.passed = true;
  hw_test_append_text(result.msg, sizeof(result.msg), "SD: PASS - Card mounted and test file written (");
  hw_test_append_u32_dec(result.msg, sizeof(result.msg), bytes_written);
  hw_test_append_text(result.msg, sizeof(result.msg), " bytes)");

  return result;
}
