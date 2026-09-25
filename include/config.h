#pragma once

#include <stdint.h>

namespace metronome {

constexpr uint8_t kLedPins[] = {2, 3, 4, 5};
constexpr uint8_t kLedCount = sizeof(kLedPins) / sizeof(kLedPins[0]);

}  // namespace metronome
