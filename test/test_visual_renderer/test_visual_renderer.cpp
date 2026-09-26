#include <unity.h>

#include "beat_sequence.h"
#include "time_signature.h"
#include "transport.h"
#include "visual_renderer.h"

namespace {

using metronome::Accent;
using metronome::BeatSequence;
using metronome::Pattern;
using metronome::TimeSignature;
using metronome::VisualRenderer;

Pattern patternFor(TimeSignature::Mode target) {
  TimeSignature signature;
  while (signature.mode() != target) {
    signature.cycle();
  }
  return signature.pattern();
}

void test_accent_durations_descend() {
  TEST_ASSERT_GREATER_THAN_UINT32(VisualRenderer::kSecondaryDurationUs,
                                  VisualRenderer::kPrimaryDurationUs);
  TEST_ASSERT_GREATER_THAN_UINT32(VisualRenderer::kNoneDurationUs,
                                  VisualRenderer::kSecondaryDurationUs);
}

void test_duration_is_clamped_to_half_the_interval() {
  TEST_ASSERT_EQUAL_UINT32(125000UL,
                           VisualRenderer::effectiveDurationUs(Accent::PRIMARY,
                                                              250000UL));
  TEST_ASSERT_EQUAL_UINT32(100000UL,
                           VisualRenderer::effectiveDurationUs(Accent::SECONDARY,
                                                              250000UL));
  TEST_ASSERT_EQUAL_UINT32(60000UL,
                           VisualRenderer::effectiveDurationUs(Accent::NONE,
                                                              250000UL));
}

void test_pulse_expires_and_new_pulse_replaces_it() {
  VisualRenderer renderer;
  renderer.startPulse(1, Accent::PRIMARY, 1000, 500000);
  TEST_ASSERT_TRUE(renderer.active());
  TEST_ASSERT_EQUAL_UINT8(1, renderer.led());
  TEST_ASSERT_FALSE(renderer.update(150999));
  TEST_ASSERT_TRUE(renderer.update(151000));
  TEST_ASSERT_FALSE(renderer.active());

  renderer.startPulse(1, Accent::PRIMARY, 200000, 500000);
  renderer.startPulse(4, Accent::NONE, 201000, 500000);
  TEST_ASSERT_TRUE(renderer.active());
  TEST_ASSERT_EQUAL_UINT8(4, renderer.led());
  TEST_ASSERT_EQUAL_UINT32(60000UL, renderer.durationUs());
}

void test_cancel_clears_active_pulse_for_stop_and_mode_change() {
  VisualRenderer renderer;
  renderer.startPulse(0, Accent::PRIMARY, 0, 500000);
  renderer.cancel();
  TEST_ASSERT_FALSE(renderer.active());
  TEST_ASSERT_FALSE(renderer.update(200000));

  renderer.startPulse(3, Accent::SECONDARY, 0, 500000);
  renderer.cancel();
  TEST_ASSERT_FALSE(renderer.active());
}

void test_catch_up_renders_only_the_final_sequence_position() {
  metronome::Tempo tempo(120);
  metronome::Transport transport(tempo,
                                 patternFor(TimeSignature::Mode::FIVE_FOUR));
  VisualRenderer renderer;
  transport.toggle(0);
  TEST_ASSERT_EQUAL_UINT16(4, transport.update(2000000UL));
  TEST_ASSERT_EQUAL_UINT8(3, transport.sequence().position());
  TEST_ASSERT_EQUAL(Accent::SECONDARY, transport.sequence().accent());
  renderer.startPulse(transport.sequence().position(),
                      transport.sequence().accent(), 2000000UL,
                      transport.clock().intervalUs());
  TEST_ASSERT_EQUAL_UINT8(3, renderer.led());
  TEST_ASSERT_EQUAL_UINT32(100000UL, renderer.durationUs());
}

void test_grouped_secondary_and_simple_primary_accents() {
  const Pattern five = patternFor(TimeSignature::Mode::FIVE_FOUR);
  const Pattern six = patternFor(TimeSignature::Mode::SIX_EIGHT);
  const Pattern seven = patternFor(TimeSignature::Mode::SEVEN_EIGHT);
  const Pattern three = patternFor(TimeSignature::Mode::THREE_FOUR);
  const Pattern four = patternFor(TimeSignature::Mode::FOUR_FOUR);
  TEST_ASSERT_EQUAL(Accent::SECONDARY, five.accentAt(3));
  TEST_ASSERT_EQUAL(Accent::SECONDARY, six.accentAt(3));
  TEST_ASSERT_EQUAL(Accent::SECONDARY, seven.accentAt(2));
  TEST_ASSERT_EQUAL(Accent::SECONDARY, seven.accentAt(4));
  for (uint8_t position = 1; position < three.steps(); ++position) {
    TEST_ASSERT_EQUAL(Accent::NONE, three.accentAt(position));
  }
  for (uint8_t position = 1; position < four.steps(); ++position) {
    TEST_ASSERT_EQUAL(Accent::NONE, four.accentAt(position));
  }
}

void test_led_capacity_remains_eight() {
  TEST_ASSERT_EQUAL_UINT8(8, VisualRenderer::kLedCapacity);
}

}  // namespace

#ifdef ARDUINO
void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_accent_durations_descend);
  RUN_TEST(test_duration_is_clamped_to_half_the_interval);
  RUN_TEST(test_pulse_expires_and_new_pulse_replaces_it);
  RUN_TEST(test_cancel_clears_active_pulse_for_stop_and_mode_change);
  RUN_TEST(test_catch_up_renders_only_the_final_sequence_position);
  RUN_TEST(test_grouped_secondary_and_simple_primary_accents);
  RUN_TEST(test_led_capacity_remains_eight);
  UNITY_END();
}
void loop() {}
#else
int main() {
  UNITY_BEGIN();
  RUN_TEST(test_accent_durations_descend);
  RUN_TEST(test_duration_is_clamped_to_half_the_interval);
  RUN_TEST(test_pulse_expires_and_new_pulse_replaces_it);
  RUN_TEST(test_cancel_clears_active_pulse_for_stop_and_mode_change);
  RUN_TEST(test_catch_up_renders_only_the_final_sequence_position);
  RUN_TEST(test_grouped_secondary_and_simple_primary_accents);
  RUN_TEST(test_led_capacity_remains_eight);
  return UNITY_END();
}
#endif
