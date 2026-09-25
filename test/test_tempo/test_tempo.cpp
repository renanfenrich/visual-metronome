#include <unity.h>

#include "tempo.h"

void test_default_bpm_is_120() {
  metronome::Tempo tempo;
  TEST_ASSERT_EQUAL_UINT16(120, tempo.bpm());
}

void test_bpm_boundaries_are_accepted() {
  TEST_ASSERT_EQUAL_UINT16(40, metronome::Tempo(40).bpm());
  TEST_ASSERT_EQUAL_UINT16(240, metronome::Tempo(240).bpm());
}

void test_low_bpm_is_clamped() {
  TEST_ASSERT_EQUAL_UINT16(40, metronome::Tempo(0).bpm());
}

void test_high_bpm_is_clamped() {
  TEST_ASSERT_EQUAL_UINT16(240, metronome::Tempo(241).bpm());
}

#ifdef ARDUINO
void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_default_bpm_is_120);
  RUN_TEST(test_bpm_boundaries_are_accepted);
  RUN_TEST(test_low_bpm_is_clamped);
  RUN_TEST(test_high_bpm_is_clamped);
  UNITY_END();
}

void loop() {}
#else
int main() {
  UNITY_BEGIN();
  RUN_TEST(test_default_bpm_is_120);
  RUN_TEST(test_bpm_boundaries_are_accepted);
  RUN_TEST(test_low_bpm_is_clamped);
  RUN_TEST(test_high_bpm_is_clamped);
  return UNITY_END();
}
#endif
