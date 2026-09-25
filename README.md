# Visual Metronome

An Arduino Uno visual metronome for musicians who cannot rely on headphone
monitoring. The current prototype has four beat LEDs, potentiometer BPM control,
Start/Stop, Tap Tempo, and 3/4 / 4/4 mode selection.

## Current status

G0 through G5 are complete and physically validated.

- G0 - Repository/bootstrap and four-LED hardware smoke test
- G1 - Deterministic tempo clock
- G2 - Four-LED visual 4/4 sequence
- G3 - Potentiometer BPM control (40-240 BPM)
- G4 - Start/Stop and Tap Tempo controls
- G5 - Time-signature mode selection between 4/4 and 3/4

G6 is next and is planned to expand the hardware to eight LEDs before adding
5/4, 6/8, 7/8, accents, and grouping patterns.

## Hardware

Current prototype:

- Arduino Uno
- 4 beat LEDs on D2-D5, each with its own current-limiting resistor
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

In 4/4, the visual sequence is:

```text
D2 -> D3 -> D4 -> D5 -> D2
```

In 3/4:

```text
D2 -> D3 -> D4 -> D2
```

D5 remains off in 3/4.

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
[architecture](docs/architecture.md) for implementation details.

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

## Roadmap

Full status: [roadmap](docs/roadmap.md).
