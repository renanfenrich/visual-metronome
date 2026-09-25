#pragma once

#include <stdint.h>

namespace metronome {

class Tempo {
 public:
  static constexpr uint16_t kMinBpm = 40;
  static constexpr uint16_t kMaxBpm = 240;
  static constexpr uint16_t kDefaultBpm = 120;

  explicit Tempo(uint16_t bpm = kDefaultBpm);

  uint16_t bpm() const;
  void setBpm(uint16_t bpm);

 private:
  uint16_t bpm_;
};

}  // namespace metronome
