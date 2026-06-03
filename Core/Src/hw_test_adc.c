/**
 * @file hw_test_adc.c
 * @brief ADC1 hardware test implementation
 */

#include "hw_test.h"
#include "hw_handles.h"
#include "hw_test_util.h"

void osDelay(uint32_t ticks);

/* ADC DMA buffer (should match DMA configuration) */
uint32_t adc_dma_buffer[15];  /* 15 channels */

/**
 * @brief Test ADC1 functionality
 */
test_result_t test_adc(void)
{
  test_result_t result;
  result.test_id = TEST_ID_ADC;
  result.passed = false;
  hw_test_clear_buffer(result.msg, sizeof(result.msg));

  /* Start ADC if not already running */
  HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_dma_buffer, 15);

  /* Give ADC time to collect samples */
  osDelay(100);

  /* Check DMA has transferred data */
  uint32_t dma_count = __HAL_DMA_GET_COUNTER(&hdma_adc1);
  if (dma_count == 0)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "ADC: FAIL - DMA counter is zero");
    return result;
  }

  /* Validate samples are not stuck at extreme values */
  uint8_t valid_samples = 0;
  for (uint8_t i = 0; i < 15; i++)
  {
    uint32_t sample = adc_dma_buffer[i];
    /* Check if sample is in reasonable range (not 0x0000 or 0xFFFF) */
    if (sample > 100 && sample < 4090)
    {
      valid_samples++;
    }
  }

  if (valid_samples >= 8)  /* At least 8 channels with valid data */
  {
    result.passed = true;
    hw_test_append_text(result.msg, sizeof(result.msg), "ADC: PASS - DMA count=");
    hw_test_append_u32_dec(result.msg, sizeof(result.msg), dma_count);
    hw_test_append_text(result.msg, sizeof(result.msg), ", valid_samples=");
    hw_test_append_u32_dec(result.msg, sizeof(result.msg), valid_samples);
  }
  else
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "ADC: FAIL - Only ");
    hw_test_append_u32_dec(result.msg, sizeof(result.msg), valid_samples);
    hw_test_append_text(result.msg, sizeof(result.msg), " valid samples out of 15");
  }

  return result;
}
