#include "tap_tempo.h"

namespace metronome {

TapTempo::TapTempo() : previousTapUs_(0), intervals_{0, 0, 0}, intervalCount_(0), hasPreviousTap_(false), bpm_(0) {}

bool TapTempo::tap(uint32_t nowUs) {
  if (!hasPreviousTap_ || nowUs - previousTapUs_ > kTimeoutUs) {
    begin(nowUs);
    return false;
  }

  const uint32_t intervalUs = nowUs - previousTapUs_;
  if (intervalUs < kMinIntervalUs || intervalUs > kMaxIntervalUs) {
    begin(nowUs);
    return false;
  }

  previousTapUs_ = nowUs;
  if (intervalCount_ < 3) {
    intervals_[intervalCount_++] = intervalUs;
  } else {
    // Keep a short rolling window so one uneven tap has limited influence.
    intervals_[0] = intervals_[1];
    intervals_[1] = intervals_[2];
    intervals_[2] = intervalUs;
  }

  uint32_t total = 0;
  for (uint8_t i = 0; i < intervalCount_; ++i) {
    total += intervals_[i];
  }
  bpm_ = static_cast<uint16_t>(60000000UL / (total / intervalCount_));
  return true;
}

uint16_t TapTempo::bpm() const { return bpm_; }

void TapTempo::reset() {
  hasPreviousTap_ = false;
  intervalCount_ = 0;
}

void TapTempo::begin(uint32_t nowUs) {
  previousTapUs_ = nowUs;
  intervalCount_ = 0;
  hasPreviousTap_ = true;
}

}  // namespace metronome
