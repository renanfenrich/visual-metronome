#pragma once

#include <stdint.h>

namespace metronome {

class BpmInput {
 public:
  static constexpr uint16_t kAdcMax = 1023;
  static constexpr uint16_t kChangeThresholdBpm = 2;

  explicit BpmInput(uint16_t initialBpm,
                    uint16_t changeThresholdBpm = kChangeThresholdBpm);

  static uint16_t mapAdcToBpm(uint16_t adc);
  // Ignores small mapped changes to prevent analog-input jitter from retiming.
  bool acceptAdc(uint16_t adc);
  void setBpm(uint16_t bpm);
  uint16_t bpm() const;

 private:
  uint16_t bpm_;
  uint16_t changeThresholdBpm_;
};

}  // namespace metronome
