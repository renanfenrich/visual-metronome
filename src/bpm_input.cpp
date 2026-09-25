#include "bpm_input.h"

#include "tempo.h"

namespace metronome {

BpmInput::BpmInput(uint16_t initialBpm, uint16_t changeThresholdBpm)
    : bpm_(initialBpm), changeThresholdBpm_(changeThresholdBpm) {}

uint16_t BpmInput::mapAdcToBpm(uint16_t adc) {
  if (adc > kAdcMax) {
    adc = kAdcMax;
  }
  return Tempo::kMinBpm +
         static_cast<uint32_t>(adc) * (Tempo::kMaxBpm - Tempo::kMinBpm) /
             kAdcMax;
}

bool BpmInput::acceptAdc(uint16_t adc) {
  const uint16_t mappedBpm = mapAdcToBpm(adc);
  // Compare after mapping because the ADC threshold varies across the BPM range.
  const uint16_t difference = mappedBpm > bpm_ ? mappedBpm - bpm_ : bpm_ - mappedBpm;
  if (difference < changeThresholdBpm_) {
    return false;
  }

  bpm_ = mappedBpm;
  return true;
}

void BpmInput::setBpm(uint16_t bpm) {
  bpm_ = bpm;
}

uint16_t BpmInput::bpm() const {
  return bpm_;
}

}  // namespace metronome
