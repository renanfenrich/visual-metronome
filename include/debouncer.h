#pragma once

#include <stdint.h>

namespace metronome {

class Debouncer {
 public:
  static constexpr uint32_t kDebounceMs = 30;

  explicit Debouncer(bool initialHigh = true, uint32_t debounceMs = kDebounceMs);

  // rawHigh is the GPIO level; INPUT_PULLUP makes a press LOW.
  bool update(bool rawHigh, uint32_t nowMs);

 private:
  bool stableHigh_;
  bool candidateHigh_;
  uint32_t candidateSinceMs_;
  uint32_t debounceMs_;
};

}  // namespace metronome
