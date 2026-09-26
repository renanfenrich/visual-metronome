#include <unity.h>

#include "beat_sequence.h"
#include "config.h"
#include "time_signature.h"

namespace {

using metronome::BeatSequence;
using metronome::Pattern;
using metronome::TimeSignature;

Pattern patternFor(TimeSignature::Mode target) {
  TimeSignature signature;
  while (signature.mode() != target) {
    signature.cycle();
  }
  return signature.pattern();
}

void test_eight_led_pins_are_configured_once_in_order() {
  const uint8_t expected[] = {2, 3, 4, 5, 9, 10, 11, 12};
  TEST_ASSERT_EQUAL_UINT8(8, metronome::kLedCount);
  for (uint8_t index = 0; index < metronome::kLedCount; ++index) {
    TEST_ASSERT_EQUAL_UINT8(expected[index], metronome::kLedPins[index]);
  }
}

void test_supported_patterns_fit_the_led_bank_and_leave_led_eight_unused() {
  const TimeSignature::Mode modes[] = {
      TimeSignature::Mode::THREE_FOUR, TimeSignature::Mode::FOUR_FOUR,
      TimeSignature::Mode::FIVE_FOUR, TimeSignature::Mode::SIX_EIGHT,
      TimeSignature::Mode::SEVEN_EIGHT};
  const uint8_t expectedSteps[] = {3, 4, 5, 6, 7};
  for (uint8_t index = 0; index < 5; ++index) {
    const Pattern pattern = patternFor(modes[index]);
    TEST_ASSERT_EQUAL_UINT8(expectedSteps[index], pattern.steps());
    TEST_ASSERT_LESS_THAN_UINT8(metronome::kLedCount, pattern.steps());
  }
}

void test_each_supported_sequence_never_selects_an_unused_position() {
  const uint8_t counts[] = {3, 4, 5, 6, 7};
  for (uint8_t index = 0; index < 5; ++index) {
    BeatSequence sequence(Pattern(counts[index], 0));
    sequence.advance(1);
    TEST_ASSERT_EQUAL_UINT8(0, sequence.position());
    for (uint8_t beat = 0; beat < counts[index]; ++beat) {
      sequence.advance(1);
      TEST_ASSERT_LESS_THAN_UINT8(counts[index], sequence.position());
    }
    TEST_ASSERT_EQUAL_UINT8(0, sequence.position());
  }
}

void test_shorter_pattern_resets_sequence_to_position_zero() {
  BeatSequence sequence(Pattern(7, 0));
  sequence.advance(6);
  TEST_ASSERT_EQUAL_UINT8(5, sequence.position());
  sequence.setPattern(Pattern(3, 0));
  TEST_ASSERT_FALSE(sequence.active());
  TEST_ASSERT_EQUAL_UINT8(0, sequence.position());
  sequence.advance(1);
  TEST_ASSERT_EQUAL_UINT8(0, sequence.position());
}

}  // namespace

#ifdef ARDUINO
void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_eight_led_pins_are_configured_once_in_order);
  RUN_TEST(test_supported_patterns_fit_the_led_bank_and_leave_led_eight_unused);
  RUN_TEST(test_each_supported_sequence_never_selects_an_unused_position);
  RUN_TEST(test_shorter_pattern_resets_sequence_to_position_zero);
  UNITY_END();
}
void loop() {}
#else
int main() {
  UNITY_BEGIN();
  RUN_TEST(test_eight_led_pins_are_configured_once_in_order);
  RUN_TEST(test_supported_patterns_fit_the_led_bank_and_leave_led_eight_unused);
  RUN_TEST(test_each_supported_sequence_never_selects_an_unused_position);
  RUN_TEST(test_shorter_pattern_resets_sequence_to_position_zero);
  return UNITY_END();
}
#endif
