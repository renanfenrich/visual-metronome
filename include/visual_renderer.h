#pragma once

#include <stdint.h>

#include "pattern.h"

namespace metronome {

// Tracks a single visual pulse without depending on Arduino GPIO APIs.
class VisualRenderer {
 public:
  static constexpr uint8_t kLedCapacity = 8;
  static constexpr uint32_t kPrimaryDurationUs = 150000UL;
  static constexpr uint32_t kSecondaryDurationUs = 100000UL;
  static constexpr uint32_t kNoneDurationUs = 60000UL;

  // A pulse is never longer than half of its current beat interval.
  static constexpr uint8_t kSafeIntervalNumerator = 1;
  static constexpr uint8_t kSafeIntervalDenominator = 2;

  void startPulse(uint8_t led, Accent accent, uint32_t nowUs,
                  uint32_t beatIntervalUs);
  bool update(uint32_t nowUs);
  void cancel();

  bool active() const;
  uint8_t led() const;
  uint32_t durationUs() const;
  static uint32_t configuredDurationUs(Accent accent);
  static uint32_t effectiveDurationUs(Accent accent, uint32_t beatIntervalUs);

 private:
  static bool reached(uint32_t nowUs, uint32_t deadlineUs);

  uint8_t led_ = 0;
  uint32_t durationUs_ = 0;
  uint32_t offDeadlineUs_ = 0;
  bool active_ = false;
};

}  // namespace metronome
