#pragma once

#include <stdint.h>

#include "time_signature.h"

namespace metronome {

struct Preset {
  uint16_t bpm;
  TimeSignature::Mode mode;
};

enum class PresetSlotStatus : uint8_t {
  VALID,
  INVALID_MAGIC,
  INVALID_VERSION,
  FUTURE_VERSION,
  INVALID_LENGTH,
  INVALID_FLAGS,
  INVALID_CRC,
  INVALID_COMMIT,
  INVALID_RESERVED,
  INVALID_BPM,
  INVALID_MODE,
};

struct DecodedPreset {
  PresetSlotStatus status;
  Preset preset;
  uint32_t generation;

  bool valid() const { return status == PresetSlotStatus::VALID; }
};

struct PresetSelection {
  bool found;
  bool futureSchema;
  uint8_t slot;
  Preset preset;
  uint32_t generation;
};

class PresetCodec {
 public:
  static constexpr uint8_t kRecordSize = 16;
  static constexpr uint8_t kSchemaVersion = 1;
  static constexpr uint8_t kCommitMarker = 0xa5;

  static Preset defaults();
  static void encode(const Preset& preset, uint32_t generation,
                     uint8_t record[kRecordSize]);
  static void updateChecksum(uint8_t record[kRecordSize]);
  static DecodedPreset decode(const uint8_t record[kRecordSize]);
  static PresetSelection selectNewest(const DecodedPreset& first,
                                      const DecodedPreset& second);
  static bool isNewerGeneration(uint32_t candidate, uint32_t current);
  static bool same(const Preset& left, const Preset& right);

 private:
  static uint16_t crc16(const uint8_t* bytes, uint8_t length);
  static bool validMode(uint8_t mode);
};

}  // namespace metronome
