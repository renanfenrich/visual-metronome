#include <unity.h>

#include "debouncer.h"
#include "time_signature.h"
#include "tempo.h"
#include "transport.h"

namespace {

void test_default_mode_is_four_four_and_cycles() {
  metronome::TimeSignature signature;
  TEST_ASSERT_EQUAL(metronome::TimeSignature::Mode::FOUR_FOUR, signature.mode());
  TEST_ASSERT_EQUAL_UINT8(4, signature.pattern().steps());
  signature.cycle();
  TEST_ASSERT_EQUAL(metronome::TimeSignature::Mode::THREE_FOUR, signature.mode());
  TEST_ASSERT_EQUAL_UINT8(3, signature.pattern().steps());
  signature.cycle();
  TEST_ASSERT_EQUAL(metronome::TimeSignature::Mode::FOUR_FOUR, signature.mode());
}

void test_active_low_mode_press_switches_to_three_four_after_debounce() {
  metronome::TimeSignature signature;
  metronome::Debouncer modeButton;
  TEST_ASSERT_FALSE(modeButton.update(false, 10));
  TEST_ASSERT_TRUE(modeButton.update(false, 40));
  signature.cycle();
  TEST_ASSERT_EQUAL(metronome::TimeSignature::Mode::THREE_FOUR, signature.mode());
}

void test_four_and_three_step_sequences_wrap_without_d5_in_three_four() {
  metronome::BeatSequence four(metronome::Pattern(4, 0));
  metronome::BeatSequence three(metronome::Pattern(3, 0));
  for (uint8_t expected = 0; expected < 4; ++expected) {
    four.advance(1);
    TEST_ASSERT_EQUAL_UINT8(expected, four.position());
  }
  four.advance(1);
  TEST_ASSERT_EQUAL_UINT8(0, four.position());
  for (uint8_t beat = 0; beat < 9; ++beat) {
    three.advance(1);
    TEST_ASSERT_LESS_THAN_UINT8(3, three.position());
    TEST_ASSERT_EQUAL_UINT8(beat % 3, three.position());
  }
}

void test_mode_change_resets_running_sequence_without_changing_clock_or_bpm() {
  metronome::Tempo tempo(120);
  metronome::Transport transport(tempo, metronome::Pattern(4, 0));
  TEST_ASSERT_TRUE(transport.toggle(0));
  TEST_ASSERT_EQUAL_UINT16(2, transport.update(1000000));
  TEST_ASSERT_EQUAL_UINT8(1, transport.sequence().position());
  const uint32_t deadline = transport.clock().nextDeadlineUs();
  const uint16_t bpm = tempo.bpm();
  transport.setPattern(metronome::Pattern(3, 0));
  TEST_ASSERT_TRUE(transport.running());
  TEST_ASSERT_FALSE(transport.sequence().active());
  TEST_ASSERT_EQUAL_UINT8(0, transport.sequence().position());
  TEST_ASSERT_EQUAL_UINT32(500000UL, transport.clock().intervalUs());
  TEST_ASSERT_EQUAL_UINT16(bpm, tempo.bpm());
  TEST_ASSERT_EQUAL_UINT32(deadline, transport.clock().nextDeadlineUs());
  TEST_ASSERT_EQUAL_UINT16(0, transport.update(deadline - 1));
  TEST_ASSERT_EQUAL_UINT16(1, transport.update(deadline));
  TEST_ASSERT_EQUAL_UINT8(0, transport.sequence().position());
}

void test_mode_change_while_stopped_preserves_stopped_state_and_first_beat() {
  metronome::Tempo tempo(120);
  metronome::Transport transport(tempo, metronome::Pattern(4, 0));
  transport.setPattern(metronome::Pattern(3, 0));
  TEST_ASSERT_FALSE(transport.running());
  TEST_ASSERT_FALSE(transport.sequence().active());
  TEST_ASSERT_TRUE(transport.toggle(1000));
  TEST_ASSERT_EQUAL_UINT16(1, transport.update(501000));
  TEST_ASSERT_EQUAL_UINT8(0, transport.sequence().position());
}

void test_catch_up_remains_correct_for_both_signatures() {
  metronome::BeatSequence four(metronome::Pattern(4, 0));
  metronome::BeatSequence three(metronome::Pattern(3, 0));
  four.advance(6);
  three.advance(6);
  TEST_ASSERT_EQUAL_UINT8(1, four.position());
  TEST_ASSERT_EQUAL_UINT8(2, three.position());
  four.advance(5);
  three.advance(5);
  TEST_ASSERT_EQUAL_UINT8(2, four.position());
  TEST_ASSERT_EQUAL_UINT8(1, three.position());
}

}  // namespace

#ifdef ARDUINO
void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_default_mode_is_four_four_and_cycles);
  RUN_TEST(test_active_low_mode_press_switches_to_three_four_after_debounce);
  RUN_TEST(test_four_and_three_step_sequences_wrap_without_d5_in_three_four);
  RUN_TEST(test_mode_change_resets_running_sequence_without_changing_clock_or_bpm);
  RUN_TEST(test_mode_change_while_stopped_preserves_stopped_state_and_first_beat);
  RUN_TEST(test_catch_up_remains_correct_for_both_signatures);
  UNITY_END();
}
void loop() {}
#else
int main() {
  UNITY_BEGIN();
  RUN_TEST(test_default_mode_is_four_four_and_cycles);
  RUN_TEST(test_active_low_mode_press_switches_to_three_four_after_debounce);
  RUN_TEST(test_four_and_three_step_sequences_wrap_without_d5_in_three_four);
  RUN_TEST(test_mode_change_resets_running_sequence_without_changing_clock_or_bpm);
  RUN_TEST(test_mode_change_while_stopped_preserves_stopped_state_and_first_beat);
  RUN_TEST(test_catch_up_remains_correct_for_both_signatures);
  return UNITY_END();
}
#endif
