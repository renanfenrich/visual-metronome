#pragma once

#include <stdint.h>

#include "pattern.h"

namespace metronome {

class TimeSignature {
 public:
  enum class Mode : uint8_t {
    THREE_FOUR,
    FOUR_FOUR,
    FIVE_FOUR,
    SIX_EIGHT,
    SEVEN_EIGHT,
  };

  TimeSignature();

  void cycle();
  Mode mode() const;
  Pattern pattern() const;
  const char* label() const;

 private:
  Mode mode_;
};

}  // namespace metronome
