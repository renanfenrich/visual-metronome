#include "pattern.h"

namespace metronome {

Pattern::Pattern(uint8_t steps, uint8_t downbeat, const Accent* accents,
                 const uint8_t* groupLengths, uint8_t groupCount)
    : steps_(steps < kMinSteps ? kMinSteps
                               : (steps > kMaxSteps ? kMaxSteps : steps)),
      downbeat_(downbeat < steps_ ? downbeat : 0) {
  for (uint8_t position = 0; position < kMaxSteps; ++position) {
    accents_[position] = Accent::NONE;
    groupBoundaries_[position] = false;
  }
  accents_[downbeat_] = Accent::PRIMARY;
  groupBoundaries_[0] = true;

  if (accents != nullptr) {
    for (uint8_t position = 0; position < steps_; ++position) {
      accents_[position] = accents[position];
    }
  }

  if (groupLengths == nullptr || groupCount == 0) {
    return;
  }

  uint8_t position = 0;
  for (uint8_t group = 0; group < groupCount; ++group) {
    const uint8_t length = groupLengths[group];
    if (length == 0 || position + length > steps_) {
      return;
    }
    groupBoundaries_[position] = true;
    position = static_cast<uint8_t>(position + length);
  }
}

uint8_t Pattern::steps() const {
  return steps_;
}

uint8_t Pattern::downbeat() const {
  return downbeat_;
}

Accent Pattern::accentAt(uint8_t position) const {
  return position < steps_ ? accents_[position] : Accent::NONE;
}

bool Pattern::isGroupBoundary(uint8_t position) const {
  return position < steps_ && groupBoundaries_[position];
}

}  // namespace metronome
