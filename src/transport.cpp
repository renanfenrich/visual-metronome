#include "transport.h"

namespace metronome {

Transport::Transport(const Tempo& tempo, const Pattern& pattern)
    : clock_(tempo), sequence_(pattern) {}

bool Transport::toggle(uint32_t nowUs) {
  if (clock_.running()) {
    clock_.stop();
    return false;
  }
  sequence_.reset();
  clock_.start(nowUs);
  return true;
}

void Transport::setTempo(const Tempo& tempo, uint32_t nowUs) {
  clock_.setTempo(tempo, nowUs);
}

void Transport::setPattern(const Pattern& pattern) {
  sequence_.setPattern(pattern);
}

uint16_t Transport::update(uint32_t nowUs) {
  const uint16_t elapsedBeats = clock_.update(nowUs);
  sequence_.advance(elapsedBeats);
  return elapsedBeats;
}

bool Transport::running() const { return clock_.running(); }

const BeatSequence& Transport::sequence() const { return sequence_; }

const Clock& Transport::clock() const { return clock_; }

}  // namespace metronome
