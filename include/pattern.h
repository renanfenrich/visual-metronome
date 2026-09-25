#pragma once

#include <stdint.h>

namespace metronome {

class Pattern {
 public:
  static constexpr uint8_t kMinSteps = 1;
  static constexpr uint8_t kMaxSteps = 16;

  // Step counts are clamped to 1..16. An invalid downbeat becomes step zero.
  Pattern(uint8_t steps = 4, uint8_t downbeat = 0);

  uint8_t steps() const;
  uint8_t downbeat() const;

 private:
  uint8_t steps_;
  uint8_t downbeat_;
};

}  // namespace metronome
