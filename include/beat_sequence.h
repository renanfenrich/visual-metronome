#pragma once

#include <stdint.h>

#include "pattern.h"

namespace metronome {

class BeatSequence {
 public:
  explicit BeatSequence(const Pattern& pattern);

  // The first elapsed beat selects the pattern downbeat.
  void advance(uint16_t elapsedBeats);
  void reset();
  void setPattern(const Pattern& pattern);

  bool active() const;
  uint8_t position() const;

 private:
  Pattern pattern_;
  uint8_t position_;
  bool active_;
};

}  // namespace metronome
