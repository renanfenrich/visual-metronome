#include "eeprom_journal.h"

#include <EEPROM.h>

namespace metronome {

void EepromJournal::begin(Preset& preset) {
  uint8_t first[kSlotSize];
  uint8_t second[kSlotSize];
  readSlot(0, first);
  readSlot(1, second);
  const DecodedPreset firstDecoded = PresetCodec::decode(first);
  const DecodedPreset secondDecoded = PresetCodec::decode(second);
  const PresetSelection selection =
      PresetCodec::selectNewest(firstDecoded, secondDecoded);
  readOnly_ = selection.futureSchema;
  if (readOnly_) {
    preset = PresetCodec::defaults();
    return;
  }

  if (selection.found) {
    preset = selection.preset;
    saved_ = selection.preset;
    generation_ = selection.generation;
    activeSlot_ = selection.slot;
    hasSaved_ = true;
    return;
  }
  preset = PresetCodec::defaults();
}

void EepromJournal::markDirty(const Preset& preset, uint32_t nowMs) {
  if (readOnly_) {
    return;
  }
  pending_ = preset;
  dirtyAtMs_ = nowMs;
  dirty_ = !hasSaved_ || !PresetCodec::same(pending_, saved_);
}

void EepromJournal::update(uint32_t nowMs) {
  if (!dirty_ || static_cast<uint32_t>(nowMs - dirtyAtMs_) < kQuietPeriodMs) {
    return;
  }
  if (!commit()) {
    dirtyAtMs_ = nowMs;
  }
}

bool EepromJournal::futureSchemaDetected() const { return readOnly_; }

void EepromJournal::readSlot(uint8_t slot, uint8_t record[kSlotSize]) const {
  const uint16_t address = static_cast<uint16_t>(slot) * kSlotSize;
  for (uint8_t index = 0; index < kSlotSize; ++index) {
    record[index] = EEPROM.read(address + index);
  }
}

bool EepromJournal::commit() {
  const uint8_t targetSlot = hasSaved_ ? static_cast<uint8_t>(1 - activeSlot_) : 0;
  const uint16_t address = static_cast<uint16_t>(targetSlot) * kSlotSize;
  uint8_t record[kSlotSize];
  PresetCodec::encode(pending_, hasSaved_ ? generation_ + 1 : 0, record);

  EEPROM.update(address + 14, 0);
  for (uint8_t index = 0; index < kSlotSize; ++index) {
    if (index != 14) {
      EEPROM.update(address + index, record[index]);
    }
  }
  for (uint8_t index = 0; index < kSlotSize; ++index) {
    if (index != 14 && EEPROM.read(address + index) != record[index]) {
      return false;
    }
  }
  EEPROM.update(address + 14, PresetCodec::kCommitMarker);
  if (EEPROM.read(address + 14) != PresetCodec::kCommitMarker) {
    return false;
  }

  saved_ = pending_;
  generation_ = hasSaved_ ? generation_ + 1 : 0;
  activeSlot_ = targetSlot;
  hasSaved_ = true;
  dirty_ = false;
  return true;
}

}  // namespace metronome
