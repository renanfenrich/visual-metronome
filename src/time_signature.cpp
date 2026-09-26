#include "time_signature.h"

namespace metronome {

TimeSignature::TimeSignature() : mode_(Mode::FOUR_FOUR) {}

void TimeSignature::cycle() {
  switch (mode_) {
    case Mode::FOUR_FOUR:
      mode_ = Mode::THREE_FOUR;
      break;
    case Mode::THREE_FOUR:
      mode_ = Mode::FIVE_FOUR;
      break;
    case Mode::FIVE_FOUR:
      mode_ = Mode::SIX_EIGHT;
      break;
    case Mode::SIX_EIGHT:
      mode_ = Mode::SEVEN_EIGHT;
      break;
    case Mode::SEVEN_EIGHT:
      mode_ = Mode::FOUR_FOUR;
      break;
  }
}

void TimeSignature::setMode(Mode mode) {
  if (static_cast<uint8_t>(mode) <= static_cast<uint8_t>(Mode::SEVEN_EIGHT)) {
    mode_ = mode;
  }
}

TimeSignature::Mode TimeSignature::mode() const {
  return mode_;
}

Pattern TimeSignature::pattern() const {
  static const Accent kThreeFourAccents[] = {Accent::PRIMARY, Accent::NONE,
                                              Accent::NONE};
  static const uint8_t kThreeFourGroups[] = {3};
  static const Accent kFourFourAccents[] = {Accent::PRIMARY, Accent::NONE,
                                             Accent::NONE, Accent::NONE};
  static const uint8_t kFourFourGroups[] = {4};
  static const Accent kFiveFourAccents[] = {Accent::PRIMARY, Accent::NONE,
                                             Accent::NONE, Accent::SECONDARY,
                                             Accent::NONE};
  static const uint8_t kFiveFourGroups[] = {3, 2};
  static const Accent kSixEightAccents[] = {Accent::PRIMARY, Accent::NONE,
                                             Accent::NONE, Accent::SECONDARY,
                                             Accent::NONE, Accent::NONE};
  static const uint8_t kSixEightGroups[] = {3, 3};
  static const Accent kSevenEightAccents[] = {
      Accent::PRIMARY, Accent::NONE, Accent::SECONDARY, Accent::NONE,
      Accent::SECONDARY, Accent::NONE, Accent::NONE};
  static const uint8_t kSevenEightGroups[] = {2, 2, 3};

  switch (mode_) {
    case Mode::THREE_FOUR:
      return Pattern(3, 0, kThreeFourAccents, kThreeFourGroups, 1);
    case Mode::FOUR_FOUR:
      return Pattern(4, 0, kFourFourAccents, kFourFourGroups, 1);
    case Mode::FIVE_FOUR:
      return Pattern(5, 0, kFiveFourAccents, kFiveFourGroups, 2);
    case Mode::SIX_EIGHT:
      return Pattern(6, 0, kSixEightAccents, kSixEightGroups, 2);
    case Mode::SEVEN_EIGHT:
      return Pattern(7, 0, kSevenEightAccents, kSevenEightGroups, 3);
  }
  return Pattern();
}

const char* TimeSignature::label() const {
  switch (mode_) {
    case Mode::THREE_FOUR:
      return "3/4";
    case Mode::FOUR_FOUR:
      return "4/4";
    case Mode::FIVE_FOUR:
      return "5/4";
    case Mode::SIX_EIGHT:
      return "6/8";
    case Mode::SEVEN_EIGHT:
      return "7/8";
  }
  return "4/4";
}

}  // namespace metronome
