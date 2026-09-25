#include "time_signature.h"

namespace metronome {

TimeSignature::TimeSignature() : mode_(Mode::FOUR_FOUR) {}

void TimeSignature::cycle() {
  mode_ = mode_ == Mode::FOUR_FOUR ? Mode::THREE_FOUR : Mode::FOUR_FOUR;
}

TimeSignature::Mode TimeSignature::mode() const {
  return mode_;
}

Pattern TimeSignature::pattern() const {
  return Pattern(mode_ == Mode::FOUR_FOUR ? 4 : 3, 0);
}

const char* TimeSignature::label() const {
  return mode_ == Mode::FOUR_FOUR ? "4/4" : "3/4";
}

}  // namespace metronome
