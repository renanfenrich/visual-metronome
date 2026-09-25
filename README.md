# Visual Metronome

An Arduino Uno visual metronome foundation for musicians who cannot rely on
headphone monitoring. G0 uses an Uno, four LEDs, four current-limiting
resistors, a breadboard, and jumper wires.

## G1 scope

G0 establishes PlatformIO, a host-testable musical domain, Uno firmware that
initializes D2-D5, a brief non-blocking startup LED check, and documentation.
G1 adds a host-testable deterministic tempo clock. It does not implement LED
beat sequencing, controls, audio, MIDI, EEPROM, or patterns.

## Architecture

`Tempo`, `Clock`, and `Pattern` are Arduino-independent C++ domain modules.
`main.cpp` owns GPIO and supplies `micros()` to `Clock::update()`. Tempo clamps
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

G0 and G1 are complete in source form. G1 is limited to a deterministic tempo
clock; it does not add the G2 LED sequence or later input, persistence,
microphone, or MIDI features. Full status: [roadmap](docs/roadmap.md).
