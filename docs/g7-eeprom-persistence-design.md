# G7 EEPROM persistence design

## Scope

G7 persists the last user-selected musical configuration, called the active
preset in this goal. Version 1 does not add named preset slots or any new
button gesture: the existing controls continue to change the active preset.

Persisted fields are:

| Field | Default | Reason |
| --- | --- | --- |
| BPM | 120 | User-selected tempo. |
| time-signature mode | 4/4 | User-selected rhythmic pattern. |

Transport running state, current beat position, an active LED pulse, tap-tempo
history, potentiometer ADC reading, and tempo-control ownership are transient.
They are not restored. Startup retains the current behavior: after the sanity
check, the transport starts from beat one using the loaded configuration.

## Version-1 EEPROM record

The Uno provides 1 KiB of EEPROM. G7 reserves the first 32 bytes as two
16-byte journal slots. The remaining EEPROM stays unowned for later goals.
Slots are at addresses 0 and 16 and have this little-endian layout:

| Offset | Size | Field | Value / rule |
| --- | ---: | --- | --- |
| 0 | 2 | magic | `0x4d, 0x50` (`MP`) |
| 2 | 1 | schema version | `1` |
| 3 | 1 | payload length | `4` |
| 4 | 4 | generation | Unsigned, incremented for each committed save. |
| 8 | 2 | BPM | 40--240 inclusive. |
| 10 | 1 | mode | Encoded `TimeSignature::Mode` (0--4). |
| 11 | 1 | flags | Must be zero in version 1. |
| 12 | 2 | CRC-16 | CRC-16/CCITT-FALSE of offsets 0--11. |
| 14 | 1 | commit marker | `0xa5`; written last. |
| 15 | 1 | reserved | Must be `0xff` in version 1. |

The fixed magic, payload length, flags, and reserved byte make accidental or
partially compatible data invalid rather than silently interpreted. The
generation comparison is rollover-safe: a generation is newer only when its
unsigned difference is nonzero and less than `2^31`.

To commit a record, firmware invalidates the target slot's marker, writes
offsets 0--13 and 15 with `EEPROM.update`, verifies the bytes by reading them
back, then writes the commit marker last. It never changes the currently valid
slot in place. The next save targets the other slot. A reset or power loss can
therefore leave at most the new slot invalid, while the prior slot remains
loadable.

## Loading, validation, and fallback

Each slot must pass all of the following before it is eligible:

1. magic, version, payload length, flags, reserved byte, and commit marker
   match exactly;
2. the CRC matches;
3. BPM and mode are within their defined domains.

When both records are eligible, load the newer generation. When one is
eligible, load it. When neither is eligible--including erased EEPROM,
truncation, a bad CRC, or invalid fields--use the defaults above. Invalid data
is never applied, and boot does not write defaults back; a later deliberate
setting change creates the first valid record.

## Wear and write timing

The EEPROM adapter uses `EEPROM.update`, so unchanged bytes are not rewritten,
and alternates between two slots. Persistence is dirty-only:

- mark dirty after an accepted potentiometer BPM change, an accepted tap-tempo
  BPM change, or a mode-button change;
- reset a 2-second quiet timer after each such change;
- commit once the timer expires and the candidate differs from the last
  successfully saved preset;
- never write for 25 ms input samples that do not change BPM, transport
  start/stop, visual events, or boot.

This debounces active knob adjustment into one commit and avoids a write on
every loop or beat. If power is removed before the quiet timer completes, the
previous valid preset is retained; immediate durability is intentionally not
traded for EEPROM wear.

## Future schema migration

A reader only decodes its own schema version. A future version must first
validate a known older record, convert it in a pure migration function, and
commit the new schema into the alternate slot only after that conversion
succeeds. It must retain the older record until the new record has been
verified and committed.

If firmware encounters a valid record with an unknown newer schema, it loads
the G7 defaults in read-only persistence mode and emits a serial diagnostic.
It must not overwrite that record, even after a user setting change; this
prevents an older firmware image from destroying newer settings. A future
firmware version owns the explicit migration path.

## Testable boundary

The record format belongs in an Arduino-independent `PresetCodec` module. It
accepts and produces fixed 16-byte arrays and exposes pure operations to:

- encode a candidate preset and generation;
- validate and decode a slot with a failure reason;
- choose the newest valid pair of slots;
- decide whether a candidate needs saving; and
- migrate a validated prior schema to the current model.

Native tests cover byte-exact serialization, CRC failures, invalid fields,
erased and torn records, newest-slot selection including generation rollover,
defaults, and future-schema protection. `EepromJournal` is the thin
Arduino-only adapter that provides EEPROM read/update/read-back and the quiet
timer; it contains no serialization or validation rules. Physical persistence
validation remains required before G7 is complete.
