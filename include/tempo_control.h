#pragma once

#include <stdint.h>

#include "bpm_input.h"

namespace metronome {

class TempoControl {
 public:
  static constexpr uint16_t kTakeoverThresholdAdc = 12;
  enum class Owner { POTENTIOMETER_CONTROL, TAP_CONTROL };

  explicit TempoControl(uint16_t initialBpm);

  // After a tap, the potentiometer must move far enough to take control back.
  bool acceptPotAdc(uint16_t adc);
  void acceptTap(uint16_t bpm, uint16_t capturedAdc);
  uint16_t bpm() const;
  Owner owner() const;

 private:
  BpmInput pot_;
  uint16_t capturedAdc_;
  Owner owner_;
};

}  // namespace metronome
