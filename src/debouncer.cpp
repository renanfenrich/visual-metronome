#include "debouncer.h"

namespace metronome {

Debouncer::Debouncer(bool initialHigh, uint32_t debounceMs)
    : stableHigh_(initialHigh),
      candidateHigh_(initialHigh),
      candidateSinceMs_(0),
      debounceMs_(debounceMs) {}

bool Debouncer::update(bool rawHigh, uint32_t nowMs) {
  if (rawHigh != candidateHigh_) {
    candidateHigh_ = rawHigh;
    candidateSinceMs_ = nowMs;
  }
  if (stableHigh_ == candidateHigh_ || nowMs - candidateSinceMs_ < debounceMs_) {
    return false;
  }
  stableHigh_ = candidateHigh_;
  return !stableHigh_;
}

}  // namespace metronome
