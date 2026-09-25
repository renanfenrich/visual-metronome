#pragma once

#include <stdint.h>

namespace metronome {

class TapTempo {
 public:
  static constexpr uint32_t kTimeoutUs = 2000000UL;
  static constexpr uint32_t kMinIntervalUs = 250000UL;
  static constexpr uint32_t kMaxIntervalUs = 1500000UL;

  TapTempo();

  // Returns true only when this tap produces an accepted BPM; the first tap
  // establishes the reference time.
  bool tap(uint32_t nowUs);
  uint16_t bpm() const;
  void reset();

 private:
  void begin(uint32_t nowUs);

  uint32_t previousTapUs_;
  uint32_t intervals_[3];
  uint8_t intervalCount_;
  bool hasPreviousTap_;
  uint16_t bpm_;
};

}  // namespace metronome
