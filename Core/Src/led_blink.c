/**
 * @file led_blink.c
 * @brief LED blinking control implementation
 */

#include "led_blink.h"
#include "main.h"
#include "cmsis_os.h"

/* LED timing constants (in milliseconds) */
#define LED_SLOW_BLINK_ON   500
#define LED_SLOW_BLINK_OFF  500
#define LED_COUNT_BLINK_ON  100
#define LED_COUNT_BLINK_OFF 100
#define LED_COUNT_PAUSE     300

static uint8_t led_mode = 0;  /* 0=off, 1=solid, 2=slow_blink, 3=cycling */
static uint8_t *cycle_test_ids = NULL;
static uint8_t cycle_test_count = 0;

/**
 * @brief Internal function to set LED pin state
 */
static void led_set(uint8_t state)
{
  if (state)
    HAL_GPIO_WritePin(LED_OUT_GPIO_Port, LED_OUT_Pin, GPIO_PIN_SET);
  else
    HAL_GPIO_WritePin(LED_OUT_GPIO_Port, LED_OUT_Pin, GPIO_PIN_RESET);
}

/**
 * @brief LED timer callback function
 * @param argument Unused
 */
static void led_timer_callback(void *argument)
{
  static uint32_t timer_ticks = 0;
  static uint8_t cycle_index = 0;
  static uint8_t blink_count = 0;
  static uint8_t blink_state = 0;  /* 0=blinking, 1=paused between blinks */

  timer_ticks++;

  if (led_mode == 1)
  {
    /* Solid mode - LED always on */
    led_set(1);
  }
  else if (led_mode == 2)
  {
    /* Slow blink mode (500ms on/off) */
    if ((timer_ticks * 10) % (LED_SLOW_BLINK_ON + LED_SLOW_BLINK_OFF) < LED_SLOW_BLINK_ON)
      led_set(1);
    else
      led_set(0);
  }
  else if (led_mode == 3)
  {
    /* Cycling blink mode - cycles through failed test counts */
    if (cycle_test_count == 0)
    {
      led_set(0);
      return;
    }

    uint32_t cycle_time = (timer_ticks * 10);
    uint32_t total_blink_time = (LED_COUNT_BLINK_ON + LED_COUNT_BLINK_OFF) * cycle_test_ids[cycle_index] + LED_COUNT_PAUSE;
    uint32_t full_cycle_time = total_blink_time * cycle_test_count;

    uint32_t pos = cycle_time % full_cycle_time;
    uint32_t current_test_start = 0;

    for (uint8_t i = 0; i < cycle_test_count; i++)
    {
      uint32_t test_blink_time = (LED_COUNT_BLINK_ON + LED_COUNT_BLINK_OFF) * cycle_test_ids[i];
      uint32_t test_total_time = test_blink_time + LED_COUNT_PAUSE;

      if (pos < current_test_start + test_total_time)
      {
        uint32_t test_pos = pos - current_test_start;
        uint8_t blink_num = 0;

        /* Count which blink we're in */
        for (uint8_t b = 0; b < cycle_test_ids[i]; b++)
        {
          if (test_pos < (LED_COUNT_BLINK_ON + LED_COUNT_BLINK_OFF) * (b + 1))
          {
            blink_num = b;
            break;
          }
        }

        uint32_t blink_phase = (test_pos % (LED_COUNT_BLINK_ON + LED_COUNT_BLINK_OFF));
        if (blink_phase < LED_COUNT_BLINK_ON)
          led_set(1);
        else
          led_set(0);

        return;
      }

      current_test_start += test_total_time;
    }
  }
  else
  {
    /* Off mode */
    led_set(0);
  }
}

/**
 * @brief Initialize LED blinking module
 */
void led_blink_init(void)
{
  /* Timer callback runs every 10ms */
  osTimerId_t timer = osTimerNew(led_timer_callback, osTimerPeriodic, NULL, NULL);
  osTimerStart(timer, 10);
}

/**
 * @brief Set LED to solid on state
 */
void led_solid(void)
{
  led_mode = 1;
  led_set(1);
}

/**
 * @brief Start slow blinking pattern
 */
void led_blink_slow(void)
{
  led_mode = 2;
}

/**
 * @brief Blink LED a specific number of times (blocking)
 */
void led_blink_count(uint8_t count)
{
  for (uint8_t i = 0; i < count; i++)
  {
    led_set(1);
    osDelay(LED_COUNT_BLINK_ON);
    led_set(0);
    osDelay(LED_COUNT_BLINK_OFF);
  }
  osDelay(LED_COUNT_PAUSE);
}

/**
 * @brief Cycle through multiple blink counts indefinitely
 */
void led_cycle_counts(uint8_t *test_ids, uint8_t count)
{
  cycle_test_ids = test_ids;
  cycle_test_count = count;
  led_mode = 3;
}

/**
 * @brief Turn LED off
 */
void led_off(void)
{
  led_mode = 0;
  led_set(0);
}
