/*
 * @file hw_test_util.h
 * @brief Small local helpers for hardware test string handling.
 */

#ifndef INC_HW_TEST_UTIL_H_
#define INC_HW_TEST_UTIL_H_

#include <stdint.h>

static inline uint32_t hw_test_strnlen_local(const char *text, uint32_t max_len)
{
  uint32_t length = 0;

  while (length < max_len && text[length] != '\0')
  {
    length++;
  }

  return length;
}

static inline void hw_test_clear_buffer(char *buffer, uint32_t buffer_size)
{
  uint32_t index;

  for (index = 0; index < buffer_size; index++)
  {
    buffer[index] = '\0';
  }
}

static inline uint32_t hw_test_append_char(char *buffer, uint32_t buffer_size, uint32_t position, char value)
{
  if (position + 1 < buffer_size)
  {
    buffer[position] = value;
    buffer[position + 1] = '\0';
  }

  return position + 1;
}

static inline uint32_t hw_test_append_text(char *buffer, uint32_t buffer_size, const char *text)
{
  uint32_t position = hw_test_strnlen_local(buffer, buffer_size);

  while (*text != '\0')
  {
    position = hw_test_append_char(buffer, buffer_size, position, *text++);
  }

  return position;
}

static inline uint32_t hw_test_append_u32_dec(char *buffer, uint32_t buffer_size, uint32_t value)
{
  char digits[10];
  uint32_t digit_count = 0;
  uint32_t position;

  do
  {
    digits[digit_count++] = (char)('0' + (value % 10U));
    value /= 10U;
  } while (value != 0U && digit_count < (uint32_t)sizeof(digits));

  position = hw_test_strnlen_local(buffer, buffer_size);
  while (digit_count > 0U)
  {
    position = hw_test_append_char(buffer, buffer_size, position, digits[--digit_count]);
  }

  return position;
}

static inline uint32_t hw_test_append_u32_hex(char *buffer, uint32_t buffer_size, uint32_t value, uint8_t width)
{
  static const char hex_digits[] = "0123456789ABCDEF";
  uint32_t position = hw_test_strnlen_local(buffer, buffer_size);
  int32_t shift;

  if (width == 0U)
  {
    return position;
  }

  shift = ((int32_t)width - 1) * 4;
  while (shift >= 0)
  {
    uint8_t nibble = (uint8_t)((value >> shift) & 0x0FU);
    position = hw_test_append_char(buffer, buffer_size, position, hex_digits[nibble]);
    shift -= 4;
  }

  return position;
}

static inline uint8_t hw_test_bytes_equal(const uint8_t *left, const uint8_t *right, uint32_t length)
{
  uint32_t index;

  for (index = 0; index < length; index++)
  {
    if (left[index] != right[index])
    {
      return 0U;
    }
  }

  return 1U;
}

#endif /* INC_HW_TEST_UTIL_H_ */
