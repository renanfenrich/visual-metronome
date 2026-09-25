#pragma once

#include <stdint.h>

namespace metronome {

constexpr uint8_t kLedPins[] = {2, 3, 4, 5};
constexpr uint8_t kLedCount = sizeof(kLedPins) / sizeof(kLedPins[0]);
constexpr uint8_t kStartStopPin = 6;
constexpr uint8_t kTapTempoPin = 7;
constexpr uint8_t kModePin = 8;
constexpr uint8_t kBpmPin = A0;

}  // namespace metronome
