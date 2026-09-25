#include <unity.h>

#include "pattern.h"

void test_four_step_pattern_has_step_zero_downbeat() {
  metronome::Pattern pattern(4, 0);
  TEST_ASSERT_EQUAL_UINT8(4, pattern.steps());
  TEST_ASSERT_EQUAL_UINT8(0, pattern.downbeat());
}

void test_arbitrary_step_counts_are_supported() {
  metronome::Pattern pattern(7, 3);
  TEST_ASSERT_EQUAL_UINT8(7, pattern.steps());
  TEST_ASSERT_EQUAL_UINT8(3, pattern.downbeat());
}

void test_invalid_pattern_input_is_normalized() {
  metronome::Pattern empty(0, 0);
  metronome::Pattern tooLarge(17, 20);
  TEST_ASSERT_EQUAL_UINT8(1, empty.steps());
  TEST_ASSERT_EQUAL_UINT8(16, tooLarge.steps());
  TEST_ASSERT_EQUAL_UINT8(0, tooLarge.downbeat());
}

#ifdef ARDUINO
void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_four_step_pattern_has_step_zero_downbeat);
  RUN_TEST(test_arbitrary_step_counts_are_supported);
  RUN_TEST(test_invalid_pattern_input_is_normalized);
  UNITY_END();
}

void loop() {}
#else
int main() {
  UNITY_BEGIN();
  RUN_TEST(test_four_step_pattern_has_step_zero_downbeat);
  RUN_TEST(test_arbitrary_step_counts_are_supported);
  RUN_TEST(test_invalid_pattern_input_is_normalized);
  return UNITY_END();
}
#endif
