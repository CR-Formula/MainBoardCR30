/**
 * @file sd_log.h
 * @brief SD card logging interface
 */

#ifndef INC_SD_LOG_H_
#define INC_SD_LOG_H_

#include <stdint.h>
#include "fatfs.h"

/**
 * @brief Initialize SD card and logging file
 * @return 0 on success, non-zero on error
 */
uint8_t sd_log_init(void);

/**
 * @brief Write a message to SD card log file
 * @param msg Message string to write
 * @note Handles file open/close automatically
 */
void sd_log_write(const char *msg);

/**
 * @brief Close the SD card log file
 */
void sd_log_close(void);

/**
 * @brief Check if SD card is available
 * @return 1 if available, 0 if not
 */
uint8_t sd_log_is_available(void);

#endif /* INC_SD_LOG_H_ */
