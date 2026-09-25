#include "pattern.h"

namespace metronome {

Pattern::Pattern(uint8_t steps, uint8_t downbeat)
    : steps_(steps < kMinSteps ? kMinSteps
                               : (steps > kMaxSteps ? kMaxSteps : steps)),
      downbeat_(downbeat < steps_ ? downbeat : 0) {}

uint8_t Pattern::steps() const {
  return steps_;
}

uint8_t Pattern::downbeat() const {
  return downbeat_;
}

}  // namespace metronome
