#include <unity.h>

#include "preset_codec.h"

namespace {

using metronome::DecodedPreset;
using metronome::Preset;
using metronome::PresetCodec;
using metronome::PresetSlotStatus;
using metronome::PresetSelection;
using metronome::TimeSignature;

void encoded(Preset preset, uint32_t generation,
             uint8_t record[PresetCodec::kRecordSize]) {
  PresetCodec::encode(preset, generation, record);
}

void test_defaults_are_valid_four_four_at_120() {
  const Preset preset = PresetCodec::defaults();
  TEST_ASSERT_EQUAL_UINT16(120, preset.bpm);
  TEST_ASSERT_EQUAL(TimeSignature::Mode::FOUR_FOUR, preset.mode);
}

void test_erased_records_fall_back_to_no_preset() {
  uint8_t record[PresetCodec::kRecordSize];
  for (uint8_t index = 0; index < PresetCodec::kRecordSize; ++index) {
    record[index] = 0xff;
  }
  const DecodedPreset erased = PresetCodec::decode(record);
  TEST_ASSERT_EQUAL(PresetSlotStatus::INVALID_MAGIC, erased.status);
  const auto selection = PresetCodec::selectNewest(erased, erased);
  TEST_ASSERT_FALSE(selection.found);
  TEST_ASSERT_FALSE(selection.futureSchema);
}

void test_serialization_is_little_endian_and_round_trips() {
  uint8_t record[PresetCodec::kRecordSize];
  encoded({173, TimeSignature::Mode::SIX_EIGHT}, 0x10203040, record);
  const uint8_t expected[] = {0x4d, 0x50, 0x01, 0x04, 0x40, 0x30,
                              0x20, 0x10, 0xad, 0x00, 0x03, 0x00,
                              0x5e, 0xb6, 0xa5, 0xff};
  TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, record, sizeof(expected));

  const DecodedPreset decoded = PresetCodec::decode(record);
  TEST_ASSERT_TRUE(decoded.valid());
  TEST_ASSERT_EQUAL_UINT32(0x10203040, decoded.generation);
  TEST_ASSERT_EQUAL_UINT16(173, decoded.preset.bpm);
  TEST_ASSERT_EQUAL(TimeSignature::Mode::SIX_EIGHT, decoded.preset.mode);
}

void test_crc_and_torn_records_are_rejected() {
  uint8_t record[PresetCodec::kRecordSize];
  encoded({120, TimeSignature::Mode::FOUR_FOUR}, 1, record);
  record[8] ^= 1;
  TEST_ASSERT_EQUAL(PresetSlotStatus::INVALID_CRC,
                    PresetCodec::decode(record).status);

  encoded({120, TimeSignature::Mode::FOUR_FOUR}, 1, record);
  record[14] = 0;
  TEST_ASSERT_EQUAL(PresetSlotStatus::INVALID_COMMIT,
                    PresetCodec::decode(record).status);
}

void test_invalid_payload_fields_are_rejected() {
  uint8_t record[PresetCodec::kRecordSize];
  encoded({120, TimeSignature::Mode::FOUR_FOUR}, 1, record);
  record[8] = 39;
  record[9] = 0;
  PresetCodec::updateChecksum(record);
  TEST_ASSERT_EQUAL(PresetSlotStatus::INVALID_BPM,
                    PresetCodec::decode(record).status);

  encoded({120, TimeSignature::Mode::FOUR_FOUR}, 1, record);
  record[10] = 5;
  PresetCodec::updateChecksum(record);
  TEST_ASSERT_EQUAL(PresetSlotStatus::INVALID_MODE,
                    PresetCodec::decode(record).status);
}

void test_invalid_header_fields_are_rejected() {
  uint8_t record[PresetCodec::kRecordSize];
  encoded({120, TimeSignature::Mode::FOUR_FOUR}, 1, record);
  record[3] = 3;
  TEST_ASSERT_EQUAL(PresetSlotStatus::INVALID_LENGTH,
                    PresetCodec::decode(record).status);

  encoded({120, TimeSignature::Mode::FOUR_FOUR}, 1, record);
  record[11] = 1;
  TEST_ASSERT_EQUAL(PresetSlotStatus::INVALID_FLAGS,
                    PresetCodec::decode(record).status);

  encoded({120, TimeSignature::Mode::FOUR_FOUR}, 1, record);
  record[15] = 0;
  TEST_ASSERT_EQUAL(PresetSlotStatus::INVALID_RESERVED,
                    PresetCodec::decode(record).status);

  encoded({120, TimeSignature::Mode::FOUR_FOUR}, 1, record);
  record[2] = 0;
  TEST_ASSERT_EQUAL(PresetSlotStatus::INVALID_VERSION,
                    PresetCodec::decode(record).status);
}

void test_future_schema_is_protected() {
  uint8_t record[PresetCodec::kRecordSize];
  encoded({120, TimeSignature::Mode::FOUR_FOUR}, 1, record);
  record[2] = PresetCodec::kSchemaVersion + 1;
  TEST_ASSERT_EQUAL(PresetSlotStatus::FUTURE_VERSION,
                    PresetCodec::decode(record).status);
}

void test_generation_comparison_handles_rollover() {
  TEST_ASSERT_TRUE(PresetCodec::isNewerGeneration(0, 0xffffffffUL));
  TEST_ASSERT_TRUE(PresetCodec::isNewerGeneration(11, 10));
  TEST_ASSERT_FALSE(PresetCodec::isNewerGeneration(10, 10));
  TEST_ASSERT_FALSE(PresetCodec::isNewerGeneration(0xffffffffUL, 0));
}

void test_newest_valid_slot_is_selected_and_future_schema_blocks_writes() {
  uint8_t firstRecord[PresetCodec::kRecordSize];
  uint8_t secondRecord[PresetCodec::kRecordSize];
  encoded({110, TimeSignature::Mode::THREE_FOUR}, 0xffffffffUL, firstRecord);
  encoded({140, TimeSignature::Mode::SIX_EIGHT}, 0, secondRecord);
  const PresetSelection selection = PresetCodec::selectNewest(
      PresetCodec::decode(firstRecord), PresetCodec::decode(secondRecord));
  TEST_ASSERT_TRUE(selection.found);
  TEST_ASSERT_FALSE(selection.futureSchema);
  TEST_ASSERT_EQUAL_UINT8(1, selection.slot);
  TEST_ASSERT_EQUAL_UINT16(140, selection.preset.bpm);

  secondRecord[2] = PresetCodec::kSchemaVersion + 1;
  const PresetSelection future = PresetCodec::selectNewest(
      PresetCodec::decode(firstRecord), PresetCodec::decode(secondRecord));
  TEST_ASSERT_FALSE(future.found);
  TEST_ASSERT_TRUE(future.futureSchema);
}

void test_invalid_input_encodes_defaults() {
  uint8_t record[PresetCodec::kRecordSize];
  encoded({0, static_cast<TimeSignature::Mode>(9)}, 4, record);
  const DecodedPreset decoded = PresetCodec::decode(record);
  TEST_ASSERT_TRUE(decoded.valid());
  TEST_ASSERT_EQUAL_UINT16(120, decoded.preset.bpm);
  TEST_ASSERT_EQUAL(TimeSignature::Mode::FOUR_FOUR, decoded.preset.mode);
}

}  // namespace

void runTests() {
  RUN_TEST(test_defaults_are_valid_four_four_at_120);
  RUN_TEST(test_erased_records_fall_back_to_no_preset);
  RUN_TEST(test_serialization_is_little_endian_and_round_trips);
  RUN_TEST(test_crc_and_torn_records_are_rejected);
  RUN_TEST(test_invalid_payload_fields_are_rejected);
  RUN_TEST(test_invalid_header_fields_are_rejected);
  RUN_TEST(test_future_schema_is_protected);
  RUN_TEST(test_generation_comparison_handles_rollover);
  RUN_TEST(test_newest_valid_slot_is_selected_and_future_schema_blocks_writes);
  RUN_TEST(test_invalid_input_encodes_defaults);
}

#ifdef ARDUINO
void setup() {
  UNITY_BEGIN();
  runTests();
  UNITY_END();
}
void loop() {}
#else
int main() {
  UNITY_BEGIN();
  runTests();
  return UNITY_END();
}
#endif
