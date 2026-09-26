#pragma once

#include <stdint.h>

namespace metronome {

enum class Accent : uint8_t { NONE, SECONDARY, PRIMARY };

class Pattern {
 public:
  static constexpr uint8_t kMinSteps = 1;
  static constexpr uint8_t kMaxSteps = 16;

  // Step counts are clamped to 1..16. An invalid downbeat becomes step zero.
  // Accent and grouping arrays are copied, keeping patterns immutable.
  Pattern(uint8_t steps = 4, uint8_t downbeat = 0,
          const Accent* accents = nullptr, const uint8_t* groupLengths = nullptr,
          uint8_t groupCount = 0);

  uint8_t steps() const;
  uint8_t downbeat() const;
  Accent accentAt(uint8_t position) const;
  bool isGroupBoundary(uint8_t position) const;

 private:
  uint8_t steps_;
  uint8_t downbeat_;
  Accent accents_[kMaxSteps];
  bool groupBoundaries_[kMaxSteps];
};

}  // namespace metronome
