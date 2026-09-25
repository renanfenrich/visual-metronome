#include <unity.h>

#include "clock.h"

namespace {

metronome::Clock clockAt(uint16_t bpm) {
  return metronome::Clock(metronome::Tempo(bpm));
}

void test_bpm_intervals_use_integer_microseconds() {
  TEST_ASSERT_EQUAL_UINT32(1000000UL, metronome::Tempo(60).intervalUs());
  TEST_ASSERT_EQUAL_UINT32(500000UL, metronome::Tempo(120).intervalUs());
  TEST_ASSERT_EQUAL_UINT32(250000UL, metronome::Tempo(240).intervalUs());
}

void test_new_clock_is_stopped_and_start_sets_first_deadline() {
  metronome::Clock clock = clockAt(120);
  TEST_ASSERT_FALSE(clock.running());
  TEST_ASSERT_EQUAL_UINT32(500000UL, clock.intervalUs());
  clock.start(1000);
  TEST_ASSERT_TRUE(clock.running());
  TEST_ASSERT_EQUAL_UINT32(501000UL, clock.nextDeadlineUs());
}

void test_stop_prevents_events() {
  metronome::Clock clock = clockAt(120);
  clock.start(0);
  clock.stop();
  TEST_ASSERT_FALSE(clock.running());
  TEST_ASSERT_EQUAL_UINT16(0, clock.update(500000));
}

void test_deadline_is_exact_and_accumulated() {
  metronome::Clock clock = clockAt(120);
  clock.start(1000);
  TEST_ASSERT_EQUAL_UINT16(0, clock.update(500999));
  TEST_ASSERT_EQUAL_UINT16(1, clock.update(501000));
  TEST_ASSERT_EQUAL_UINT32(1001000UL, clock.nextDeadlineUs());
}

void test_delayed_polling_reports_all_elapsed_beats_without_drift() {
  metronome::Clock clock = clockAt(120);
  clock.start(1000);
  TEST_ASSERT_EQUAL_UINT16(3, clock.update(1501000));
  TEST_ASSERT_EQUAL_UINT32(2001000UL, clock.nextDeadlineUs());
  TEST_ASSERT_EQUAL_UINT16(1, clock.update(2001000));
  TEST_ASSERT_EQUAL_UINT32(2501000UL, clock.nextDeadlineUs());
}

void test_rollover_deadline_is_reached() {
  metronome::Clock clock = clockAt(240);
  const uint32_t start = 0xFFF00000UL;
  clock.start(start);
  const uint32_t deadline = start + clock.intervalUs();
  TEST_ASSERT_EQUAL_UINT16(0, clock.update(deadline - 1));
  TEST_ASSERT_EQUAL_UINT16(1, clock.update(deadline));
}

}  // namespace

#ifdef ARDUINO
void setup() {
  UNITY_BEGIN();
  RUN_TEST(test_bpm_intervals_use_integer_microseconds);
  RUN_TEST(test_new_clock_is_stopped_and_start_sets_first_deadline);
  RUN_TEST(test_stop_prevents_events);
  RUN_TEST(test_deadline_is_exact_and_accumulated);
  RUN_TEST(test_delayed_polling_reports_all_elapsed_beats_without_drift);
  RUN_TEST(test_rollover_deadline_is_reached);
  UNITY_END();
}

void loop() {}
#else
int main() {
  UNITY_BEGIN();
  RUN_TEST(test_bpm_intervals_use_integer_microseconds);
  RUN_TEST(test_new_clock_is_stopped_and_start_sets_first_deadline);
  RUN_TEST(test_stop_prevents_events);
  RUN_TEST(test_deadline_is_exact_and_accumulated);
  RUN_TEST(test_delayed_polling_reports_all_elapsed_beats_without_drift);
  RUN_TEST(test_rollover_deadline_is_reached);
  return UNITY_END();
}
#endif
