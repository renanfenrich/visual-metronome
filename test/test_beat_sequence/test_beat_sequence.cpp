#include <unity.h>

#include "beat_sequence.h"

namespace {

metronome::BeatSequence fourFour() {
  return metronome::BeatSequence(metronome::Pattern(4, 0));
}

void test_sequence_starts_inactive_at_downbeat_position() {
  const metronome::BeatSequence sequence = fourFour();
  TEST_ASSERT_FALSE(sequence.active());
  TEST_ASSERT_EQUAL_UINT8(0, sequence.position());
}

void test_first_beat_selects_downbeat_then_advances_and_wraps() {
  metronome::BeatSequence sequence = fourFour();
  sequence.advance(1);
  TEST_ASSERT_TRUE(sequence.active());
  TEST_ASSERT_EQUAL_UINT8(0, sequence.position());
  sequence.advance(1);
  TEST_ASSERT_EQUAL_UINT8(1, sequence.position());
  sequence.advance(1);
  TEST_ASSERT_EQUAL_UINT8(2, sequence.position());
  sequence.advance(1);
  TEST_ASSERT_EQUAL_UINT8(3, sequence.position());
  sequence.advance(1);
  TEST_ASSERT_EQUAL_UINT8(0, sequence.position());
}

void test_catch_up_advances_by_full_elapsed_count() {
  metronome::BeatSequence sequence = fourFour();
  sequence.advance(1);
  sequence.advance(2);
  TEST_ASSERT_EQUAL_UINT8(2, sequence.position());
  sequence.advance(4);
  TEST_ASSERT_EQUAL_UINT8(2, sequence.position());
  sequence.advance(5);
  TEST_ASSERT_EQUAL_UINT8(3, sequence.position());
}

}  // namespace

#ifdef ARDUINO
void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_sequence_starts_inactive_at_downbeat_position);
  RUN_TEST(test_first_beat_selects_downbeat_then_advances_and_wraps);
  RUN_TEST(test_catch_up_advances_by_full_elapsed_count);
  UNITY_END();
}

void loop() {}
#else
int main() {
  UNITY_BEGIN();
  RUN_TEST(test_sequence_starts_inactive_at_downbeat_position);
  RUN_TEST(test_first_beat_selects_downbeat_then_advances_and_wraps);
  RUN_TEST(test_catch_up_advances_by_full_elapsed_count);
  return UNITY_END();
}
#endif
