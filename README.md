# Visual Metronome

An Arduino Uno visual metronome foundation for musicians who cannot rely on
headphone monitoring. G0 uses an Uno, four LEDs, four current-limiting
resistors, a breadboard, and jumper wires.

## G4 scope

G0 establishes PlatformIO, a host-testable musical domain, Uno firmware that
initializes D2-D5, a brief non-blocking startup LED check, and documentation.
G1 adds a host-testable deterministic tempo clock. G2 adds a four-step visual
sequence: after the sanity check, the first clock event lights D2, followed by
D3, D4, D5, then D2 again. G3 samples the A0 potentiometer every 25 ms and
maps it to a stable 40--240 BPM range. G4 adds active-low D6 Start/Stop and D7
Tap Tempo buttons with 30 ms non-blocking debounce. It does not add D8,
alternate time signatures, persistence, audio, MIDI, or gestures.

## Architecture

`Tempo`, `Clock`, `Pattern`, `BeatSequence`, and `BpmInput` are Arduino-independent C++
domain modules. `main.cpp` owns GPIO and supplies `micros()` to `Clock::update()`.
Tempo clamps
values to 40-240 BPM and calculates `60,000,000 / BPM` microseconds per beat.
The clock starts with its first deadline one full interval later and advances
deadlines by whole intervals. See
[architecture](docs/architecture.md) and [wiring](docs/wiring.md).

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

G0 through G3 are complete in source form. G3 is limited to A0 potentiometer
tempo control; it does not add buttons, persistence, microphone, or MIDI.
Full status: [roadmap](docs/roadmap.md).
