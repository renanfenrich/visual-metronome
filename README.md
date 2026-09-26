# Visual Metronome

An Arduino Uno visual metronome for musicians who cannot rely on headphone
monitoring. The current prototype has eight beat LEDs, potentiometer BPM control,
Start/Stop, Tap Tempo, and 3/4, 4/4, 5/4, 6/8, and 7/8 mode selection.

## Current status

G0 through G6C are implemented. G0 hardware validation covered the original
four-LED wiring; the D9-D12 expansion still needs visual confirmation on the
assembled eight-LED prototype.

- G0 - Repository/bootstrap and four-LED hardware smoke test
- G1 - Deterministic tempo clock
- G2 - Four-LED visual 4/4 sequence
- G3 - Potentiometer BPM control (40-240 BPM)
- G4 - Start/Stop and Tap Tempo controls
- G5 - Time-signature mode selection
- G6A - Grouped rhythmic patterns for 3/4, 4/4, 5/4, 6/8, and 7/8
- G6B - Eight-LED hardware and binary beat rendering
- G6C - Non-blocking accent-strength rendering (implementation and software
  validation complete; physical visual validation pending human verification)

## Hardware

Current prototype:

- Arduino Uno
- 8 beat LEDs: D2-D5 and D9-D12, each with its own current-limiting resistor
- Start/Stop button on D6
- Tap Tempo button on D7
- Mode button on D8
- BPM potentiometer on A0
- breadboard and jumper wires

The three buttons use active-low `INPUT_PULLUP` wiring to GND. See
[wiring](docs/wiring.md) for details.

## Behavior

At startup the firmware performs a brief non-blocking LED sanity check, then
starts the metronome automatically.

Each clock event lights exactly one LED. A `VisualRenderer` keeps that LED on
for a non-blocking accent pulse: PRIMARY is 150 ms, SECONDARY is 100 ms, and
NONE is 60 ms. PWM brightness is deliberately not used because D2-D5 and
D9-D12 are not all PWM-capable. Every pulse is capped at half the current beat
interval, so it cannot extend into the next rhythmic position.

The supported sequences are:

```text
D2 -> D3 -> D4 -> D5 -> D2     (4/4)
D2 -> D3 -> D4 -> D2           (3/4)
D2 -> D3 -> D4 -> D5 -> D9 -> D2 (5/4)
D2 -> D3 -> D4 -> D5 -> D9 -> D10 -> D2 (6/8)
D2 -> D3 -> D4 -> D5 -> D9 -> D10 -> D11 -> D2 (7/8)
```

LED 8 (D12) remains off for all current signatures, while still taking part in
the startup sanity check and global clear operations. If a delayed loop catches
up across multiple beats, the sequence catches up musically and only its final
current position is rendered; historical pulses are not replayed.

The potentiometer maps A0 to 40-240 BPM with hysteresis. Tap Tempo can take
temporary ownership of tempo, and the potentiometer resumes control only after
meaningful physical movement.

## Architecture

The musical and control logic is kept in Arduino-independent C++ modules so it
can be tested natively. Current domain modules include `Tempo`, `Clock`,
`Pattern`, `BeatSequence`, `BpmInput`, `Debouncer`, `TapTempo`,
`TempoControl`, `Transport`, and `TimeSignature`.

`main.cpp` owns Arduino-specific GPIO, ADC sampling, and calls to `micros()`
and `millis()`.

The clock uses integer timing, rollover-safe deadline comparisons, and
accumulated deadlines rather than `delay()`. See
[architecture](docs/architecture.md),
[technical reference](docs/technical-reference.md), and [wiring](docs/wiring.md)
for implementation details.

## Local use

Prerequisite: [PlatformIO Core](https://docs.platformio.org/en/latest/core/).

```sh
platformio run
platformio test -e native
platformio run -t upload
platformio device monitor -b 9600
```

The first command builds Uno firmware, the second runs host tests, the third
uploads it, and the last opens the serial monitor.

## Development

See the [development workflow](docs/development-workflow.md) for the required
goal-branch, validation, pull-request, review, and merge process.

## Roadmap

Full status: [roadmap](docs/roadmap.md).
