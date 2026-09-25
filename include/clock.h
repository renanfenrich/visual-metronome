#pragma once

#include <stdint.h>

#include "tempo.h"

namespace metronome {

class Clock {
 public:
  explicit Clock(const Tempo& tempo);

  void start(uint32_t nowUs);
  void stop();
  bool running() const;

  // Returns the number of elapsed beat deadlines since the previous update.
  uint16_t update(uint32_t nowUs);

  uint32_t intervalUs() const;
  uint32_t nextDeadlineUs() const;

 private:
  static bool reached(uint32_t nowUs, uint32_t deadlineUs);

  uint32_t intervalUs_;
  uint32_t nextDeadlineUs_;
  bool running_;
};

}  // namespace metronome
