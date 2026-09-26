#pragma once

#include <stdint.h>

#include "preset_codec.h"

namespace metronome {

class EepromJournal {
 public:
  static constexpr uint16_t kSlotSize = PresetCodec::kRecordSize;
  static constexpr uint16_t kQuietPeriodMs = 2000;

  void begin(Preset& preset);
  void markDirty(const Preset& preset, uint32_t nowMs);
  void update(uint32_t nowMs);
  bool futureSchemaDetected() const;

 private:
  void readSlot(uint8_t slot, uint8_t record[kSlotSize]) const;
  bool commit();

  Preset saved_ = PresetCodec::defaults();
  Preset pending_ = PresetCodec::defaults();
  uint32_t generation_ = 0;
  uint32_t dirtyAtMs_ = 0;
  uint8_t activeSlot_ = 0;
  bool hasSaved_ = false;
  bool dirty_ = false;
  bool readOnly_ = false;
};

}  // namespace metronome
