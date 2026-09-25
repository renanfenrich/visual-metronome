#include <unity.h>

#include "bpm_input.h"

namespace {

void test_adc_endpoints_map_to_bpm_endpoints() {
  TEST_ASSERT_EQUAL_UINT16(40, metronome::BpmInput::mapAdcToBpm(0));
  TEST_ASSERT_EQUAL_UINT16(240, metronome::BpmInput::mapAdcToBpm(1023));
}

void test_adc_midpoint_maps_near_bpm_midpoint() {
  TEST_ASSERT_EQUAL_UINT16(140, metronome::BpmInput::mapAdcToBpm(512));
}

void test_mapped_bpm_stays_in_range() {
  TEST_ASSERT_EQUAL_UINT16(40, metronome::BpmInput::mapAdcToBpm(0));
  TEST_ASSERT_EQUAL_UINT16(240, metronome::BpmInput::mapAdcToBpm(1023));
  TEST_ASSERT_EQUAL_UINT16(240, metronome::BpmInput::mapAdcToBpm(1024));
}

void test_small_mapped_changes_are_ignored() {
  metronome::BpmInput input(120);
  TEST_ASSERT_FALSE(input.acceptAdc(414));  // Maps to 120 BPM.
  TEST_ASSERT_FALSE(input.acceptAdc(419));  // Maps to 121 BPM.
  TEST_ASSERT_EQUAL_UINT16(120, input.bpm());
}

void test_significant_mapped_change_is_accepted() {
  metronome::BpmInput input(120);
  TEST_ASSERT_TRUE(input.acceptAdc(425));  // Maps to 123 BPM.
  TEST_ASSERT_EQUAL_UINT16(123, input.bpm());
}

}  // namespace

#ifdef ARDUINO
void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_adc_endpoints_map_to_bpm_endpoints);
  RUN_TEST(test_adc_midpoint_maps_near_bpm_midpoint);
  RUN_TEST(test_mapped_bpm_stays_in_range);
  RUN_TEST(test_small_mapped_changes_are_ignored);
  RUN_TEST(test_significant_mapped_change_is_accepted);
  UNITY_END();
}

void loop() {}
#else
int main() {
  UNITY_BEGIN();
  RUN_TEST(test_adc_endpoints_map_to_bpm_endpoints);
  RUN_TEST(test_adc_midpoint_maps_near_bpm_midpoint);
  RUN_TEST(test_mapped_bpm_stays_in_range);
  RUN_TEST(test_small_mapped_changes_are_ignored);
  RUN_TEST(test_significant_mapped_change_is_accepted);
  return UNITY_END();
}
#endif
