# Visual Metronome

An Arduino Uno visual metronome foundation for musicians who cannot rely on
headphone monitoring. G0 uses an Uno, four LEDs, four current-limiting
resistors, a breadboard, and jumper wires.

## G0 scope

G0 establishes PlatformIO, a host-testable musical domain, Uno firmware that
initializes D2-D5, a brief non-blocking startup LED check, and documentation.
It deliberately does not implement a metronome clock, controls, audio, MIDI,
EEPROM, or LED patterns.

## Architecture

`Tempo` and `Pattern` are Arduino-independent C++ domain modules. `main.cpp`
owns GPIO. Tempo clamps values to 40-240 BPM; Pattern clamps step counts to
1-16 and resets an out-of-range downbeat to zero. See
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

G0 is complete in source form. G1 is limited to a deterministic tempo clock;
it will not add the G2 LED sequence or later input, persistence, microphone,
or MIDI features. Full status: [roadmap](docs/roadmap.md).
