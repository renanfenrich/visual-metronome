# Technical reference

This document is a quick orientation for contributors. It describes the G6C
implementation; planned roadmap items are not part of the current contract.

## Technology stack

| Layer | Choice | Responsibility |
| --- | --- | --- |
| Target hardware | Arduino Uno (ATmega328P) | Reads controls and drives eight LEDs. |
| Firmware framework | Arduino | Provides GPIO, ADC, `millis()`, `micros()`, and serial output. |
| Build system | PlatformIO Core | Builds the Uno firmware and host-test environment. |
| Application language | C++ | Implements firmware orchestration and hardware-independent domain models. |
| Host tests | PlatformIO `native` environment | Exercises domain logic without Arduino hardware. |
| CI | GitHub Actions | Runs native tests and compiles the Uno target. |

The production target is configured as `uno`; `native` is a test-only target.
Both environments deliberately compile the same domain sources. `main.cpp` is
excluded from host tests because it owns Arduino APIs.

## Architectural boundary

`src/main.cpp` is the hardware adapter. It reads GPIO and A0, obtains time,
and writes LEDs. Classes such as `Tempo`, `Clock`, `Pattern`, `BeatSequence`,
`Debouncer`, `TapTempo`, `TempoControl`, `Transport`, `TimeSignature`, and
`VisualRenderer`, and `PresetCodec` do
not include Arduino headers. Keep new musical rules in those testable classes;
keep pin access and Arduino-specific presentation in `main.cpp`.

## Nomenclature

| Term | Meaning in this project |
| --- | --- |
| BPM | Beats per minute, clamped to 40--240. |
| Interval | Duration of one beat in microseconds: `60,000,000 / BPM`. |
| Deadline | The timestamp at which the next beat is due. |
| Beat | One elapsed clock interval, not necessarily one firmware loop pass. |
| Downbeat | The first position in a `Pattern`; currently position zero/D2. |
| Pattern | The number of visual positions and its downbeat. |
| Beat sequence | The active pattern position that selects an LED. |
| Visual renderer | Hardware-independent state machine for one accent pulse. |
| Transport | The paired clock and beat sequence; it is running or stopped. |
| Tap tempo | BPM derived from recent valid button-tap intervals. |
| Potentiometer takeover | Returning control from tap tempo to the physical knob after meaningful movement. |
| ADC | The Uno's 0--1023 analog reading from A0. |
| Active-low | A pressed button reads `LOW` because it uses `INPUT_PULLUP`. |
| Debounce | Requiring a GPIO level to remain stable for 30 ms before treating it as a press. |

## Runtime flow

Each `loop()` pass samples the potentiometer every 25 ms, completes the
non-blocking startup LED check when due, processes debounced Start/Stop, Tap,
and mode presses, then advances the transport. Processing controls before the
clock tick ensures a state change cannot immediately display a beat from the
old state.

When the clock reports one or more elapsed deadlines, `BeatSequence` advances
by that exact count. `VisualRenderer` receives only the final current position
and its `Pattern` accent, so delayed polling preserves musical position without
rapidly replaying historical pulses.

G6C represents accents through duration rather than PWM brightness: PRIMARY
uses 150 ms, SECONDARY 100 ms, and NONE 60 ms. This works uniformly on all
eight pins, unlike PWM. The renderer caps every duration at half the current
beat interval and expires pulses through a rollover-safe timestamp comparison.
Start/Stop, mode changes, and tempo changes cancel an active pulse and clear
the LEDs; a mode change keeps its existing clock deadline and BPM. LED 8/D12
is reserved: it participates in startup and global clears but never receives a
current rhythmic pulse.

G7 loads the newest valid EEPROM journal slot before the startup sanity check.
The active preset contains BPM and time signature only. A changed BPM or mode
is committed after two seconds of quiet time; transport events never write
EEPROM. Invalid data falls back to defaults, while a newer schema disables
writes to avoid destructive downgrade.

## Deliberate trade-offs

| Decision | Benefit | Cost / implication |
| --- | --- | --- |
| Polling, not interrupts | Small, deterministic firmware with host-testable clock logic. | The loop must run at least once per half `micros()` wrap window. |
| Integer microseconds | Predictable on an 8-bit MCU; no floating-point dependency. | BPM intervals are rounded by integer division. |
| Accumulated deadlines | Avoids adding loop execution time to every beat. | Delayed polls must catch up, so `Clock::update()` can return multiple beats. |
| Three tap intervals | Smooths an uneven tap without retaining stale tempo history. | A new sequence needs two taps before it produces BPM. |
| ADC hysteresis | Reduces analog-input jitter. | Small knob movements do not change BPM. |
| Potentiometer pickup | Prevents a parked knob from instantly undoing tap tempo. | The knob must move 12 ADC counts before it retakes control. |
| Eight duration-coded LEDs | Covers current 3- through 7-step patterns with visible accents. | LED 8 remains reserved; all G6C physical visual acceptance checks passed by human observation. |
| Two-slot EEPROM journal | Survives a torn save and reduces wear through alternating slots and `EEPROM.update`. | Only the last BPM and signature persist; no MIDI/audio integration. |

## Contributor guidance

- Treat `Clock::update()`'s return value as a count, not a boolean.
- Do not use `delay()` for timing or debounce; it would block controls and
  distort scheduling.
- Preserve `uint32_t` subtraction for timestamp comparisons so `micros()` and
  `millis()` wraparound remains safe.
- Add host tests for changes to domain behavior, then run both `platformio test
  -e native` and `platformio run -e uno`.
- Keep hardware limits explicit. Do not claim support for a signature that
  requires more visual positions than the installed LEDs.

For pin-level assembly details, see [wiring](wiring.md). For deeper timing and
behavioral contracts, see [architecture](architecture.md).
