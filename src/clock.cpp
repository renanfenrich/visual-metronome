#include "clock.h"

namespace metronome {

Clock::Clock(const Tempo& tempo)
    : intervalUs_(tempo.intervalUs()), nextDeadlineUs_(0), running_(false) {}

void Clock::start(uint32_t nowUs) {
  nextDeadlineUs_ = nowUs + intervalUs_;
  running_ = true;
}

void Clock::stop() {
  running_ = false;
}

bool Clock::running() const {
  return running_;
}

uint16_t Clock::update(uint32_t nowUs) {
  if (!running_ || !reached(nowUs, nextDeadlineUs_)) {
    return 0;
  }

  const uint32_t elapsed = (nowUs - nextDeadlineUs_) / intervalUs_ + 1;
  nextDeadlineUs_ += elapsed * intervalUs_;
  return static_cast<uint16_t>(elapsed);
}

uint32_t Clock::intervalUs() const {
  return intervalUs_;
}

uint32_t Clock::nextDeadlineUs() const {
  return nextDeadlineUs_;
}

bool Clock::reached(uint32_t nowUs, uint32_t deadlineUs) {
  return static_cast<int32_t>(nowUs - deadlineUs) >= 0;
}

}  // namespace metronome
