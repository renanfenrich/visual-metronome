#pragma once

#include <stdint.h>

namespace metronome {

constexpr uint8_t kLedPins[] = {2, 3, 4, 5, 9, 10, 11, 12};
constexpr uint8_t kLedCount = sizeof(kLedPins) / sizeof(kLedPins[0]);
constexpr uint8_t kStartStopPin = 6;
constexpr uint8_t kTapTempoPin = 7;
constexpr uint8_t kModePin = 8;
#ifdef ARDUINO
constexpr uint8_t kBpmPin = A0;
#else
constexpr uint8_t kBpmPin = 14;  // Uno's digital alias for A0.
#endif

}  // namespace metronome
