/**
 * @file sd_log.c
 * @brief SD card logging implementation
 */

#include "sd_log.h"
#include "ff.h"

static FIL log_file;
static uint8_t sd_available = 0;
static FATFS fs;

/**
 * @brief Initialize SD card and logging file
 */
uint8_t sd_log_init(void)
{
  /* Mount SD card */
  FRESULT res = f_mount(&fs, "", 0);
  if (res != FR_OK)
  {
    sd_available = 0;
    return 1;
  }

  /* Try to open log file in append mode */
  res = f_open(&log_file, "TEST_LOG.TXT", FA_OPEN_APPEND | FA_WRITE);
  if (res != FR_OK)
  {
    sd_available = 0;
    return 1;
  }

  sd_available = 1;

  /* Write header */
  f_puts("\n=== Test Run Start ===\n", &log_file);
  f_sync(&log_file);

  return 0;
}

/**
 * @brief Write a message to SD card log
 */
void sd_log_write(const char *msg)
{
  if (!sd_available)
    return;

  f_puts(msg, &log_file);
  f_puts("\n", &log_file);

  /* Sync after each write for reliability */
  f_sync(&log_file);
}

/**
 * @brief Close the log file
 */
void sd_log_close(void)
{
  if (!sd_available)
    return;

  f_puts("=== Test Run Complete ===\n", &log_file);
  f_close(&log_file);
  f_mount(NULL, "", 0);
  sd_available = 0;
}

/**
 * @brief Check if SD card is available
 */
uint8_t sd_log_is_available(void)
{
  return sd_available;
}
