#pragma once

#include <stdint.h>

#include "beat_sequence.h"
#include "clock.h"

namespace metronome {

class Transport {
 public:
  Transport(const Tempo& tempo, const Pattern& pattern);

  bool toggle(uint32_t nowUs);
  void setTempo(const Tempo& tempo, uint32_t nowUs);
  // Changing signatures resets the beat position but preserves clock state.
  void setPattern(const Pattern& pattern);
  uint16_t update(uint32_t nowUs);
  bool running() const;
  const BeatSequence& sequence() const;
  const Clock& clock() const;

 private:
  Clock clock_;
  BeatSequence sequence_;
};

}  // namespace metronome
