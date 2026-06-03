/**
 * @file led_blink.h
 * @brief LED blinking control for status indication
 */

#ifndef INC_LED_BLINK_H_
#define INC_LED_BLINK_H_

#include <stdint.h>

/**
 * @brief Initialize LED blinking module
 * @note Must be called after FreeRTOS kernel started
 */
void led_blink_init(void);

/**
 * @brief Set LED to solid on state (initialization indicator)
 */
void led_solid(void);

/**
 * @brief Start slow blinking pattern (500ms on/off) - success indicator
 */
void led_blink_slow(void);

/**
 * @brief Blink LED a specific number of times
 * @param count Number of blinks (1-7 for test IDs)
 * @note Blocks until blinks complete
 */
void led_blink_count(uint8_t count);

/**
 * @brief Cycle through multiple blink counts indefinitely
 * @param test_ids Array of test IDs that failed
 * @param count Number of failed tests
 * @note Runs indefinitely; call only when tests complete
 */
void led_cycle_counts(uint8_t *test_ids, uint8_t count);

/**
 * @brief Turn LED off
 */
void led_off(void);

#endif /* INC_LED_BLINK_H_ */
