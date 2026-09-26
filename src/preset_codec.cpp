#include "preset_codec.h"

#include "tempo.h"

namespace metronome {
namespace {
constexpr uint8_t kMagic0 = 0x4d;
constexpr uint8_t kMagic1 = 0x50;
constexpr uint8_t kPayloadLength = 4;
constexpr uint8_t kOffsetGeneration = 4;
constexpr uint8_t kOffsetBpm = 8;
constexpr uint8_t kOffsetMode = 10;
constexpr uint8_t kOffsetFlags = 11;
constexpr uint8_t kOffsetCrc = 12;
constexpr uint8_t kOffsetCommit = 14;
constexpr uint8_t kOffsetReserved = 15;
}

Preset PresetCodec::defaults() {
  return {Tempo::kDefaultBpm, TimeSignature::Mode::FOUR_FOUR};
}

void PresetCodec::encode(const Preset& preset, uint32_t generation,
                         uint8_t record[kRecordSize]) {
  const Preset safe = (preset.bpm >= Tempo::kMinBpm &&
                       preset.bpm <= Tempo::kMaxBpm &&
                       validMode(static_cast<uint8_t>(preset.mode)))
                          ? preset
                          : defaults();
  record[0] = kMagic0;
  record[1] = kMagic1;
  record[2] = kSchemaVersion;
  record[3] = kPayloadLength;
  for (uint8_t index = 0; index < 4; ++index) {
    record[kOffsetGeneration + index] = generation >> (index * 8);
  }
  record[kOffsetBpm] = safe.bpm;
  record[kOffsetBpm + 1] = safe.bpm >> 8;
  record[kOffsetMode] = static_cast<uint8_t>(safe.mode);
  record[kOffsetFlags] = 0;
  updateChecksum(record);
  record[kOffsetCommit] = kCommitMarker;
  record[kOffsetReserved] = 0xff;
}

void PresetCodec::updateChecksum(uint8_t record[kRecordSize]) {
  const uint16_t crc = crc16(record, kOffsetCrc);
  record[kOffsetCrc] = crc;
  record[kOffsetCrc + 1] = crc >> 8;
}

DecodedPreset PresetCodec::decode(const uint8_t record[kRecordSize]) {
  DecodedPreset decoded = {PresetSlotStatus::INVALID_MAGIC, defaults(), 0};
  if (record[0] != kMagic0 || record[1] != kMagic1) {
    return decoded;
  }
  if (record[kOffsetCommit] != kCommitMarker) {
    decoded.status = PresetSlotStatus::INVALID_COMMIT;
    return decoded;
  }
  if (record[2] > kSchemaVersion) {
    decoded.status = PresetSlotStatus::FUTURE_VERSION;
    return decoded;
  }
  if (record[2] != kSchemaVersion) {
    decoded.status = PresetSlotStatus::INVALID_VERSION;
    return decoded;
  }
  if (record[3] != kPayloadLength) {
    decoded.status = PresetSlotStatus::INVALID_LENGTH;
    return decoded;
  }
  if (record[kOffsetFlags] != 0) {
    decoded.status = PresetSlotStatus::INVALID_FLAGS;
    return decoded;
  }
  if (record[kOffsetReserved] != 0xff) {
    decoded.status = PresetSlotStatus::INVALID_RESERVED;
    return decoded;
  }
  const uint16_t storedCrc = static_cast<uint16_t>(record[kOffsetCrc]) |
                             static_cast<uint16_t>(record[kOffsetCrc + 1]) << 8;
  if (crc16(record, kOffsetCrc) != storedCrc) {
    decoded.status = PresetSlotStatus::INVALID_CRC;
    return decoded;
  }
  const uint16_t bpm = static_cast<uint16_t>(record[kOffsetBpm]) |
                       static_cast<uint16_t>(record[kOffsetBpm + 1]) << 8;
  if (bpm < Tempo::kMinBpm || bpm > Tempo::kMaxBpm) {
    decoded.status = PresetSlotStatus::INVALID_BPM;
    return decoded;
  }
  if (!validMode(record[kOffsetMode])) {
    decoded.status = PresetSlotStatus::INVALID_MODE;
    return decoded;
  }

  decoded.status = PresetSlotStatus::VALID;
  decoded.preset = {bpm, static_cast<TimeSignature::Mode>(record[kOffsetMode])};
  for (uint8_t index = 0; index < 4; ++index) {
    decoded.generation |= static_cast<uint32_t>(record[kOffsetGeneration + index])
                          << (index * 8);
  }
  return decoded;
}

PresetSelection PresetCodec::selectNewest(const DecodedPreset& first,
                                          const DecodedPreset& second) {
  PresetSelection selection = {false,
                               first.status == PresetSlotStatus::FUTURE_VERSION ||
                                   second.status == PresetSlotStatus::FUTURE_VERSION,
                               0,
                               defaults(),
                               0};
  if (selection.futureSchema) {
    return selection;
  }
  if (!first.valid() && !second.valid()) {
    return selection;
  }
  const bool useSecond = second.valid() &&
                         (!first.valid() ||
                          isNewerGeneration(second.generation, first.generation));
  const DecodedPreset& selected = useSecond ? second : first;
  selection.found = true;
  selection.slot = useSecond ? 1 : 0;
  selection.preset = selected.preset;
  selection.generation = selected.generation;
  return selection;
}

bool PresetCodec::isNewerGeneration(uint32_t candidate, uint32_t current) {
  const uint32_t difference = candidate - current;
  return difference != 0 && difference < 0x80000000UL;
}

bool PresetCodec::same(const Preset& left, const Preset& right) {
  return left.bpm == right.bpm && left.mode == right.mode;
}

uint16_t PresetCodec::crc16(const uint8_t* bytes, uint8_t length) {
  uint16_t crc = 0xffff;
  for (uint8_t index = 0; index < length; ++index) {
    crc ^= static_cast<uint16_t>(bytes[index]) << 8;
    for (uint8_t bit = 0; bit < 8; ++bit) {
      crc = (crc & 0x8000) ? static_cast<uint16_t>((crc << 1) ^ 0x1021)
                            : static_cast<uint16_t>(crc << 1);
    }
  }
  return crc;
}

bool PresetCodec::validMode(uint8_t mode) {
  return mode <= static_cast<uint8_t>(TimeSignature::Mode::SEVEN_EIGHT);
}

}  // namespace metronome
