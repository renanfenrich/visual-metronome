#include "tempo.h"

namespace metronome {

Tempo::Tempo(uint16_t bpm) : bpm_(kDefaultBpm) {
  setBpm(bpm);
}

uint16_t Tempo::bpm() const {
  return bpm_;
}

void Tempo::setBpm(uint16_t bpm) {
  if (bpm < kMinBpm) {
    bpm_ = kMinBpm;
  } else if (bpm > kMaxBpm) {
    bpm_ = kMaxBpm;
  } else {
    bpm_ = bpm;
  }
}

}  // namespace metronome
