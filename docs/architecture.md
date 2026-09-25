# Architecture

The `Tempo` and `Pattern` modules are small C++ domain models with no Arduino
headers. `main.cpp` is the only G0 code that knows about GPIO. This keeps
musical rules testable on the host and prevents musical concepts from being
coupled to `digitalWrite()`.

G0 has no scheduler. Its one-time LED indication is non-blocking and separate
from any future clock. G1 will use `micros()` and deadline accumulation:

```text
next_deadline += interval
```

rather than recomputing from the current time:

```text
next_deadline = micros() + interval
```

Accumulation avoids adding execution-time drift on every beat. `delay()` will
not be used for the metronome because it would block future input and display
work. The pattern model stores a step count and downbeat independently, leaving
room for irregular meters, tap tempo, and MIDI Clock without adding those
features now.
