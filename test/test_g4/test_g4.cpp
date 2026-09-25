#include <unity.h>

#include "debouncer.h"
#include "tap_tempo.h"
#include "tempo.h"
#include "tempo_control.h"
#include "transport.h"

namespace {

void test_debouncer_filters_bounce_holds_and_rearms() {
  metronome::Debouncer button;
  TEST_ASSERT_FALSE(button.update(true, 0));
  TEST_ASSERT_FALSE(button.update(false, 1));
  TEST_ASSERT_FALSE(button.update(true, 20));
  TEST_ASSERT_FALSE(button.update(true, 60));

  TEST_ASSERT_FALSE(button.update(false, 70));
  TEST_ASSERT_TRUE(button.update(false, 100));
  TEST_ASSERT_FALSE(button.update(false, 200));

  TEST_ASSERT_FALSE(button.update(true, 210));
  TEST_ASSERT_FALSE(button.update(true, 239));
  TEST_ASSERT_FALSE(button.update(true, 240));
  TEST_ASSERT_FALSE(button.update(false, 250));
  TEST_ASSERT_TRUE(button.update(false, 280));
}

void test_transport_stops_and_restarts_at_beat_one() {
  metronome::Tempo tempo(120);
  metronome::Transport transport(tempo, metronome::Pattern(4, 0));
  TEST_ASSERT_TRUE(transport.toggle(0));
  TEST_ASSERT_EQUAL_UINT16(1, transport.update(500000));
  TEST_ASSERT_EQUAL_UINT8(0, transport.sequence().position());
  TEST_ASSERT_FALSE(transport.toggle(500001));
  TEST_ASSERT_EQUAL_UINT16(0, transport.update(1500000));
  TEST_ASSERT_TRUE(transport.toggle(2000000));
  TEST_ASSERT_FALSE(transport.sequence().active());
  TEST_ASSERT_EQUAL_UINT16(0, transport.update(2499999));
  TEST_ASSERT_EQUAL_UINT16(1, transport.update(2500000));
  TEST_ASSERT_EQUAL_UINT8(0, transport.sequence().position());
}

void test_tap_tempo_uses_rolling_average_and_rejects_invalid_intervals() {
  metronome::TapTempo tap;
  TEST_ASSERT_FALSE(tap.tap(0));
  TEST_ASSERT_TRUE(tap.tap(500000));
  TEST_ASSERT_EQUAL_UINT16(120, tap.bpm());
  TEST_ASSERT_TRUE(tap.tap(1005000));
  TEST_ASSERT_TRUE(tap.tap(1500000));
  TEST_ASSERT_EQUAL_UINT16(120, tap.bpm());

  tap.reset();
  TEST_ASSERT_FALSE(tap.tap(0));
  TEST_ASSERT_FALSE(tap.tap(200000));
  TEST_ASSERT_TRUE(tap.tap(700000));
  TEST_ASSERT_EQUAL_UINT16(120, tap.bpm());

  tap.reset();
  TEST_ASSERT_FALSE(tap.tap(0));
  TEST_ASSERT_FALSE(tap.tap(1600000));
  TEST_ASSERT_TRUE(tap.tap(2100000));
  TEST_ASSERT_EQUAL_UINT16(120, tap.bpm());
  TEST_ASSERT_TRUE(tap.bpm() >= metronome::Tempo::kMinBpm);
  TEST_ASSERT_TRUE(tap.bpm() <= metronome::Tempo::kMaxBpm);
}

void test_tap_timeout_restarts_the_sequence() {
  metronome::TapTempo tap;
  TEST_ASSERT_FALSE(tap.tap(0));
  TEST_ASSERT_FALSE(tap.tap(2000001));
  TEST_ASSERT_TRUE(tap.tap(2500001));
  TEST_ASSERT_EQUAL_UINT16(120, tap.bpm());
}

void test_tap_updates_clock_without_an_immediate_beat_even_when_stopped() {
  metronome::Tempo tempo(120);
  metronome::Transport transport(tempo, metronome::Pattern(4, 0));
  metronome::TapTempo tap;
  TEST_ASSERT_FALSE(tap.tap(0));
  TEST_ASSERT_TRUE(tap.tap(250000));
  TEST_ASSERT_EQUAL_UINT16(240, tap.bpm());
  tempo.setBpm(tap.bpm());
  transport.setTempo(tempo, 200000);
  TEST_ASSERT_EQUAL_UINT32(250000UL, transport.clock().intervalUs());
  TEST_ASSERT_EQUAL_UINT16(0, transport.update(450000));

  TEST_ASSERT_TRUE(transport.toggle(500000));
  tempo.setBpm(120);
  transport.setTempo(tempo, 600000);
  TEST_ASSERT_EQUAL_UINT16(0, transport.update(600000));
  TEST_ASSERT_EQUAL_UINT16(0, transport.update(1099999));
  TEST_ASSERT_EQUAL_UINT16(1, transport.update(1100000));
}

void test_tap_ownership_blocks_noise_until_meaningful_pot_motion() {
  metronome::TempoControl control(120);
  TEST_ASSERT_TRUE(control.acceptPotAdc(425));
  TEST_ASSERT_EQUAL_UINT16(123, control.bpm());

  control.acceptTap(137, 500);
  TEST_ASSERT_EQUAL(metronome::TempoControl::Owner::TAP_CONTROL, control.owner());
  TEST_ASSERT_FALSE(control.acceptPotAdc(501));
  TEST_ASSERT_FALSE(control.acceptPotAdc(511));
  TEST_ASSERT_EQUAL(metronome::TempoControl::Owner::TAP_CONTROL, control.owner());
  TEST_ASSERT_TRUE(control.acceptPotAdc(512));
  TEST_ASSERT_EQUAL(metronome::TempoControl::Owner::POTENTIOMETER_CONTROL, control.owner());
  TEST_ASSERT_EQUAL_UINT16(metronome::BpmInput::mapAdcToBpm(512), control.bpm());
  TEST_ASSERT_FALSE(control.acceptPotAdc(513));
}

}  // namespace

#ifdef ARDUINO
void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_debouncer_filters_bounce_holds_and_rearms);
  RUN_TEST(test_transport_stops_and_restarts_at_beat_one);
  RUN_TEST(test_tap_tempo_uses_rolling_average_and_rejects_invalid_intervals);
  RUN_TEST(test_tap_timeout_restarts_the_sequence);
  RUN_TEST(test_tap_updates_clock_without_an_immediate_beat_even_when_stopped);
  RUN_TEST(test_tap_ownership_blocks_noise_until_meaningful_pot_motion);
  UNITY_END();
}
void loop() {}
#else
int main() {
  UNITY_BEGIN();
  RUN_TEST(test_debouncer_filters_bounce_holds_and_rearms);
  RUN_TEST(test_transport_stops_and_restarts_at_beat_one);
  RUN_TEST(test_tap_tempo_uses_rolling_average_and_rejects_invalid_intervals);
  RUN_TEST(test_tap_timeout_restarts_the_sequence);
  RUN_TEST(test_tap_updates_clock_without_an_immediate_beat_even_when_stopped);
  RUN_TEST(test_tap_ownership_blocks_noise_until_meaningful_pot_motion);
  return UNITY_END();
}
#endif
