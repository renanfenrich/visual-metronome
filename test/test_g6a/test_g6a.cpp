#include <unity.h>

#include "time_signature.h"
#include "tempo.h"
#include "transport.h"

namespace {

using metronome::Accent;
using metronome::Pattern;
using metronome::TimeSignature;

Pattern patternFor(TimeSignature::Mode target) {
  TimeSignature signature;
  while (signature.mode() != target) {
    signature.cycle();
  }
  return signature.pattern();
}

void assertAccents(const Pattern& pattern, const Accent* expected,
                   uint8_t count) {
  TEST_ASSERT_EQUAL_UINT8(count, pattern.steps());
  for (uint8_t position = 0; position < count; ++position) {
    TEST_ASSERT_EQUAL(expected[position], pattern.accentAt(position));
  }
}

void test_default_and_complete_mode_cycle() {
  TimeSignature signature;
  const TimeSignature::Mode expected[] = {
      TimeSignature::Mode::FOUR_FOUR, TimeSignature::Mode::THREE_FOUR,
      TimeSignature::Mode::FIVE_FOUR, TimeSignature::Mode::SIX_EIGHT,
      TimeSignature::Mode::SEVEN_EIGHT, TimeSignature::Mode::FOUR_FOUR};
  for (uint8_t index = 0; index < 6; ++index) {
    TEST_ASSERT_EQUAL(expected[index], signature.mode());
    if (index < 5) {
      signature.cycle();
    }
  }
}

void test_labels_and_steps_for_all_modes() {
  const TimeSignature::Mode modes[] = {
      TimeSignature::Mode::THREE_FOUR, TimeSignature::Mode::FOUR_FOUR,
      TimeSignature::Mode::FIVE_FOUR, TimeSignature::Mode::SIX_EIGHT,
      TimeSignature::Mode::SEVEN_EIGHT};
  const char* labels[] = {"3/4", "4/4", "5/4", "6/8", "7/8"};
  const uint8_t steps[] = {3, 4, 5, 6, 7};
  for (uint8_t index = 0; index < 5; ++index) {
    TimeSignature signature;
    while (signature.mode() != modes[index]) {
      signature.cycle();
    }
    TEST_ASSERT_EQUAL_STRING(labels[index], signature.label());
    TEST_ASSERT_EQUAL_UINT8(steps[index], signature.pattern().steps());
  }
}

void test_default_accent_maps() {
  const Accent three[] = {Accent::PRIMARY, Accent::NONE, Accent::NONE};
  const Accent four[] = {Accent::PRIMARY, Accent::NONE, Accent::NONE,
                         Accent::NONE};
  const Accent five[] = {Accent::PRIMARY, Accent::NONE, Accent::NONE,
                         Accent::SECONDARY, Accent::NONE};
  const Accent six[] = {Accent::PRIMARY, Accent::NONE, Accent::NONE,
                        Accent::SECONDARY, Accent::NONE, Accent::NONE};
  const Accent seven[] = {Accent::PRIMARY, Accent::NONE, Accent::SECONDARY,
                          Accent::NONE, Accent::SECONDARY, Accent::NONE,
                          Accent::NONE};
  assertAccents(patternFor(TimeSignature::Mode::THREE_FOUR), three, 3);
  assertAccents(patternFor(TimeSignature::Mode::FOUR_FOUR), four, 4);
  assertAccents(patternFor(TimeSignature::Mode::FIVE_FOUR), five, 5);
  assertAccents(patternFor(TimeSignature::Mode::SIX_EIGHT), six, 6);
  assertAccents(patternFor(TimeSignature::Mode::SEVEN_EIGHT), seven, 7);
}

void test_compound_group_boundaries() {
  const Pattern five = patternFor(TimeSignature::Mode::FIVE_FOUR);
  const Pattern six = patternFor(TimeSignature::Mode::SIX_EIGHT);
  const Pattern seven = patternFor(TimeSignature::Mode::SEVEN_EIGHT);
  TEST_ASSERT_TRUE(five.isGroupBoundary(0));
  TEST_ASSERT_TRUE(five.isGroupBoundary(3));
  TEST_ASSERT_FALSE(five.isGroupBoundary(1));
  TEST_ASSERT_TRUE(six.isGroupBoundary(0));
  TEST_ASSERT_TRUE(six.isGroupBoundary(3));
  TEST_ASSERT_FALSE(six.isGroupBoundary(4));
  TEST_ASSERT_TRUE(seven.isGroupBoundary(0));
  TEST_ASSERT_TRUE(seven.isGroupBoundary(2));
  TEST_ASSERT_TRUE(seven.isGroupBoundary(4));
  TEST_ASSERT_FALSE(seven.isGroupBoundary(6));
}

void test_extended_sequences_wrap_and_catch_up() {
  const uint8_t counts[] = {5, 6, 7};
  for (uint8_t index = 0; index < 3; ++index) {
    metronome::BeatSequence sequence(Pattern(counts[index], 0));
    sequence.advance(1);
    for (uint8_t beat = 1; beat <= counts[index]; ++beat) {
      sequence.advance(1);
    }
    TEST_ASSERT_EQUAL_UINT8(0, sequence.position());
    sequence.advance(static_cast<uint16_t>(counts[index] + 2));
    TEST_ASSERT_EQUAL_UINT8(2, sequence.position());
  }
}

void test_pattern_switch_preserves_running_clock_and_bpm() {
  metronome::Tempo tempo(120);
  metronome::Transport transport(tempo, Pattern(4, 0));
  TEST_ASSERT_TRUE(transport.toggle(0));
  transport.update(1000000);
  const uint32_t deadline = transport.clock().nextDeadlineUs();
  transport.setPattern(patternFor(TimeSignature::Mode::SEVEN_EIGHT));
  TEST_ASSERT_TRUE(transport.running());
  TEST_ASSERT_EQUAL_UINT16(120, tempo.bpm());
  TEST_ASSERT_EQUAL_UINT32(deadline, transport.clock().nextDeadlineUs());
  TEST_ASSERT_FALSE(transport.sequence().active());
  TEST_ASSERT_EQUAL_UINT16(1, transport.update(deadline));
  TEST_ASSERT_EQUAL_UINT8(0, transport.sequence().position());
}

void test_pattern_switch_preserves_stopped_state() {
  metronome::Tempo tempo(120);
  metronome::Transport transport(tempo, Pattern(4, 0));
  transport.setPattern(patternFor(TimeSignature::Mode::FIVE_FOUR));
  TEST_ASSERT_FALSE(transport.running());
  TEST_ASSERT_FALSE(transport.sequence().active());
  TEST_ASSERT_EQUAL_UINT8(0, transport.sequence().position());
}

}  // namespace

#ifdef ARDUINO
void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_default_and_complete_mode_cycle);
  RUN_TEST(test_labels_and_steps_for_all_modes);
  RUN_TEST(test_default_accent_maps);
  RUN_TEST(test_compound_group_boundaries);
  RUN_TEST(test_extended_sequences_wrap_and_catch_up);
  RUN_TEST(test_pattern_switch_preserves_running_clock_and_bpm);
  RUN_TEST(test_pattern_switch_preserves_stopped_state);
  UNITY_END();
}
void loop() {}
#else
int main() {
  UNITY_BEGIN();
  RUN_TEST(test_default_and_complete_mode_cycle);
  RUN_TEST(test_labels_and_steps_for_all_modes);
  RUN_TEST(test_default_accent_maps);
  RUN_TEST(test_compound_group_boundaries);
  RUN_TEST(test_extended_sequences_wrap_and_catch_up);
  RUN_TEST(test_pattern_switch_preserves_running_clock_and_bpm);
  RUN_TEST(test_pattern_switch_preserves_stopped_state);
  return UNITY_END();
}
#endif
