/**
 * @file hw_test_lora.c
 * @brief LoRa SX1276 module hardware test implementation
 */

#include "hw_test.h"
#include "main.h"
#include "hw_handles.h"
#include "hw_test_util.h"

void osDelay(uint32_t ticks);

/* SX1276 register definitions */
#define LORA_REG_VERSION            0x42
#define LORA_REG_OPMODE             0x01
#define LORA_REG_BITRATE_MSB        0x02
#define LORA_REG_BITRATE_LSB        0x03
#define LORA_REG_FDEV_MSB           0x04
#define LORA_REG_FDEV_LSB           0x05
#define LORA_REG_FRF_MSB            0x06
#define LORA_REG_FRF_MID            0x07
#define LORA_REG_FRF_LSB            0x08
#define LORA_REG_PA_CONFIG          0x09
#define LORA_REG_LNA                0x0C
#define LORA_REG_FIFO_ADDR_PTR      0x0D
#define LORA_REG_FIFO_TX_BASE_ADDR  0x0E
#define LORA_REG_FIFO_RX_BASE_ADDR  0x0F
#define LORA_REG_IRQ_FLAGS          0x12
#define LORA_REG_RX_NB_BYTES        0x13
#define LORA_REG_RX_HEADER_CNT      0x14
#define LORA_REG_RX_PACKET_CNT      0x15
#define LORA_REG_MODEM_STAT         0x18
#define LORA_REG_PKT_SNR_VALUE      0x19
#define LORA_REG_PKT_RSSI_VALUE     0x1A
#define LORA_REG_RSSI_VALUE         0x1B
#define LORA_REG_HOP_CHANNEL        0x1C
#define LORA_REG_MODEM_CONFIG1      0x1D
#define LORA_REG_MODEM_CONFIG2      0x1E
#define LORA_REG_SYMB_TIMEOUT       0x1F
#define LORA_REG_PREAMBLE_MSB       0x20
#define LORA_REG_PREAMBLE_LSB       0x21
#define LORA_REG_PAYLOAD_LEN        0x22
#define LORA_REG_MAX_PAYLOAD_LEN    0x23
#define LORA_REG_HOP_PERIOD         0x24
#define LORA_REG_FIFO_RX_BYTE_ADDR  0x25
#define LORA_REG_MODEM_CONFIG3      0x26
#define LORA_REG_PPM_CORRECTION     0x27
#define LORA_REG_FEI_MSB            0x28
#define LORA_REG_FEI_MID            0x29
#define LORA_REG_FEI_LSB            0x2A
#define LORA_REG_RSSI_WIDEBAND      0x2C
#define LORA_REG_DETECT_OPTIMIZE    0x31
#define LORA_REG_INVERT_IQ          0x33
#define LORA_REG_DETECT_THRESHOLD   0x37
#define LORA_REG_SYNC_WORD          0x39
#define LORA_REG_INVERT_IQ2         0x3B
#define LORA_REG_DIO_MAPPING1       0x40
#define LORA_REG_DIO_MAPPING2       0x41

#define LORA_VERSION_EXPECTED       0x12

/* LoRa mode bits */
#define LORA_MODE_SLEEP             0x00
#define LORA_MODE_STANDBY           0x01
#define LORA_MODE_FSTX              0x02
#define LORA_MODE_TX                0x03
#define LORA_MODE_FSRX              0x04
#define LORA_MODE_RX_CONTINUOUS     0x05
#define LORA_MODE_RX_SINGLE         0x06

#define LORA_LORA_MODE_ENABLE       0x80

/**
 * @brief Write a single register to LoRa module via SPI
 */
static HAL_StatusTypeDef lora_write_register(uint8_t reg, uint8_t value)
{
  uint8_t tx_buf[2] = {reg | 0x80, value};  /* MSB of address = 1 for write */
  uint8_t rx_buf[2];

  HAL_GPIO_WritePin(LoRa_CS_GPIO_Port, LoRa_CS_Pin, GPIO_PIN_RESET);  /* CS low */
  osDelay(1);

  HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&hspi2, tx_buf, rx_buf, 2, 100);

  osDelay(1);
  HAL_GPIO_WritePin(LoRa_CS_GPIO_Port, LoRa_CS_Pin, GPIO_PIN_SET);  /* CS high */

  return status;
}

/**
 * @brief Read a single register from LoRa module via SPI
 */
static HAL_StatusTypeDef lora_read_register(uint8_t reg, uint8_t *value)
{
  uint8_t tx_buf[2] = {reg & 0x7F, 0x00};  /* MSB of address = 0 for read */
  uint8_t rx_buf[2];

  HAL_GPIO_WritePin(LoRa_CS_GPIO_Port, LoRa_CS_Pin, GPIO_PIN_RESET);  /* CS low */
  osDelay(1);

  HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(&hspi2, tx_buf, rx_buf, 2, 100);

  osDelay(1);
  HAL_GPIO_WritePin(LoRa_CS_GPIO_Port, LoRa_CS_Pin, GPIO_PIN_SET);  /* CS high */

  if (status == HAL_OK)
    *value = rx_buf[1];

  return status;
}

/**
 * @brief Reset LoRa module
 */
static HAL_StatusTypeDef lora_reset(void)
{
  /* Assert reset */
  HAL_GPIO_WritePin(LoRa_RST_GPIO_Port, LoRa_RST_Pin, GPIO_PIN_RESET);
  osDelay(10);

  /* Release reset */
  HAL_GPIO_WritePin(LoRa_RST_GPIO_Port, LoRa_RST_Pin, GPIO_PIN_SET);
  osDelay(100);

  return HAL_OK;
}

/**
 * @brief Test LoRa SX1276 module
 */
test_result_t test_lora(void)
{
  test_result_t result;
  result.test_id = TEST_ID_LORA;
  result.passed = false;
  hw_test_clear_buffer(result.msg, sizeof(result.msg));

  /* Step 1: Reset the module */
  if (lora_reset() != HAL_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "LoRa: FAIL - Reset sequence failed");
    return result;
  }

  /* Step 2: Read and verify version register */
  uint8_t version = 0;
  if (lora_read_register(LORA_REG_VERSION, &version) != HAL_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "LoRa: FAIL - Cannot read version register");
    return result;
  }

  if (version != LORA_VERSION_EXPECTED)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "LoRa: FAIL - Version mismatch (got 0x");
    hw_test_append_u32_hex(result.msg, sizeof(result.msg), version, 2U);
    hw_test_append_text(result.msg, sizeof(result.msg), ", expected 0x");
    hw_test_append_u32_hex(result.msg, sizeof(result.msg), LORA_VERSION_EXPECTED, 2U);
    hw_test_append_text(result.msg, sizeof(result.msg), ")");
    return result;
  }

  /* Step 3: Set sleep mode */
  if (lora_write_register(LORA_REG_OPMODE, LORA_MODE_SLEEP | LORA_LORA_MODE_ENABLE) != HAL_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "LoRa: FAIL - Cannot set sleep mode");
    return result;
  }
  osDelay(10);

  /* Step 4: Configure frequency (915 MHz) */
  /* Frequency = Fxosc * (0xFRF / 2^19) where Fxosc = 32MHz */
  /* For 915MHz: FRF = 0xE4C000 */
  if (lora_write_register(LORA_REG_FRF_MSB, 0xE4) != HAL_OK ||
      lora_write_register(LORA_REG_FRF_MID, 0xC0) != HAL_OK ||
      lora_write_register(LORA_REG_FRF_LSB, 0x00) != HAL_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "LoRa: FAIL - Cannot set frequency");
    return result;
  }

  /* Step 5: Read back frequency to verify */
  uint8_t frf_msb = 0, frf_mid = 0, frf_lsb = 0;
  if (lora_read_register(LORA_REG_FRF_MSB, &frf_msb) != HAL_OK ||
      lora_read_register(LORA_REG_FRF_MID, &frf_mid) != HAL_OK ||
      lora_read_register(LORA_REG_FRF_LSB, &frf_lsb) != HAL_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "LoRa: FAIL - Cannot read frequency");
    return result;
  }

  if (frf_msb != 0xE4 || frf_mid != 0xC0 || frf_lsb != 0x00)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "LoRa: FAIL - Frequency mismatch (got 0x");
    hw_test_append_u32_hex(result.msg, sizeof(result.msg), frf_msb, 2U);
    hw_test_append_u32_hex(result.msg, sizeof(result.msg), frf_mid, 2U);
    hw_test_append_u32_hex(result.msg, sizeof(result.msg), frf_lsb, 2U);
    hw_test_append_text(result.msg, sizeof(result.msg), ")");
    return result;
  }

  /* Step 6: Configure LoRa mode bits */
  /* ModemConfig1: BW=125kHz (0x70), CodingRate=4/5 (0x02), ImplicitHeader=0 (0x00) */
  if (lora_write_register(LORA_REG_MODEM_CONFIG1, 0x72) != HAL_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "LoRa: FAIL - Cannot set ModemConfig1");
    return result;
  }

  /* ModemConfig2: SF=7 (0x70), TxContinuousMode=0 (0x00), RxPayloadCrcOn=1 (0x04) */
  if (lora_write_register(LORA_REG_MODEM_CONFIG2, 0x74) != HAL_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "LoRa: FAIL - Cannot set ModemConfig2");
    return result;
  }

  /* Step 7: Read back ModemConfig to verify */
  uint8_t modem_cfg1 = 0, modem_cfg2 = 0;
  if (lora_read_register(LORA_REG_MODEM_CONFIG1, &modem_cfg1) != HAL_OK ||
      lora_read_register(LORA_REG_MODEM_CONFIG2, &modem_cfg2) != HAL_OK)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "LoRa: FAIL - Cannot read ModemConfig");
    return result;
  }

  if (modem_cfg1 != 0x72 || modem_cfg2 != 0x74)
  {
    hw_test_append_text(result.msg, sizeof(result.msg), "LoRa: FAIL - ModemConfig mismatch (CFG1: 0x");
    hw_test_append_u32_hex(result.msg, sizeof(result.msg), modem_cfg1, 2U);
    hw_test_append_text(result.msg, sizeof(result.msg), ", CFG2: 0x");
    hw_test_append_u32_hex(result.msg, sizeof(result.msg), modem_cfg2, 2U);
    hw_test_append_text(result.msg, sizeof(result.msg), ")");
    return result;
  }

  /* All checks passed */
  result.passed = true;
  hw_test_append_text(result.msg, sizeof(result.msg), "LoRa: PASS - SX1276 v0x");
  hw_test_append_u32_hex(result.msg, sizeof(result.msg), version, 2U);
  hw_test_append_text(result.msg, sizeof(result.msg), " initialized (915MHz, SF7, BW125)");

  return result;
}
