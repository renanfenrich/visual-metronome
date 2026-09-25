#include "beat_sequence.h"

namespace metronome {

BeatSequence::BeatSequence(const Pattern& pattern)
    : pattern_(pattern), position_(pattern.downbeat()), active_(false) {}

void BeatSequence::advance(uint16_t elapsedBeats) {
  if (elapsedBeats == 0) {
    return;
  }

  const uint8_t steps = pattern_.steps();
  const uint16_t offset = active_ ? elapsedBeats : elapsedBeats - 1;
  position_ = static_cast<uint8_t>((position_ + offset) % steps);
  active_ = true;
}

bool BeatSequence::active() const {
  return active_;
}

uint8_t BeatSequence::position() const {
  return position_;
}

}  // namespace metronome
