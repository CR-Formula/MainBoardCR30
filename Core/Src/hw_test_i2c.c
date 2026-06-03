/**
 * @file hw_test_i2c.c
 * @brief I2C1 bus hardware test implementation
 */

#include "hw_test.h"
#include "hw_handles.h"
#include "hw_test_util.h"

/**
 * @brief Test I2C1 functionality by scanning for devices
 */
test_result_t test_i2c(void)
{
  test_result_t result;
  result.test_id = TEST_ID_I2C;
  result.passed = false;
  hw_test_clear_buffer(result.msg, sizeof(result.msg));

  uint8_t found_devices = 0;
  char device_log[128] = "";

  /* Scan I2C addresses 0x08 to 0x77 (skip reserved ranges) */
  for (uint8_t addr = 0x08; addr <= 0x77; addr++)
  {
    /* Skip reserved addresses */
    if ((addr >= 0x00 && addr <= 0x07) || (addr >= 0x78 && addr <= 0x7F))
      continue;

    /* Attempt to write to address to detect ACK */
    HAL_StatusTypeDef status = HAL_I2C_IsDeviceReady(&hi2c1, addr << 1, 2, 50);

    if (status == HAL_OK)
    {
      found_devices++;
      hw_test_append_text(device_log, sizeof(device_log), "0x");
      hw_test_append_u32_hex(device_log, sizeof(device_log), addr, 2U);
      hw_test_append_text(device_log, sizeof(device_log), " ");
    }
  }

  if (found_devices > 0)
  {
    result.passed = true;
    hw_test_append_text(result.msg, sizeof(result.msg), "I2C: PASS - Found ");
    hw_test_append_u32_dec(result.msg, sizeof(result.msg), found_devices);
    hw_test_append_text(result.msg, sizeof(result.msg), " device(s): ");
    hw_test_append_text(result.msg, sizeof(result.msg), device_log);
  }
  else
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "I2C: FAIL - No devices found on bus");
  }

  return result;
}
