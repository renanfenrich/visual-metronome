# Architecture

The `Tempo`, `Clock`, `Pattern`, and `BeatSequence` modules are small C++
domain models with no Arduino headers. `main.cpp` owns GPIO and calls
`Clock::update(micros())`. This keeps musical rules testable on the host and
prevents them from being coupled to `digitalWrite()`.

G0's one-time LED indication remains non-blocking and separate from the G1
clock. `Tempo::intervalUs()` uses integer arithmetic: `60,000,000 / BPM`.

`Clock::start(now_us)` sets `next_deadline = now_us + interval_us`, so it
reports no immediate beat; the first event is due one full interval later.
Each `update(now_us)` returns the number of elapsed deadlines, then advances
the deadline by that many whole intervals. It uses deadline accumulation:

```text
next_deadline += interval
```

rather than recomputing from the current time:

```text
next_deadline = micros() + interval
```

Accumulation avoids adding execution-time drift on every beat. Delayed polling
catches up by returning all elapsed beats without emitting visual events. The
count is `uint16_t`; rollover-safe signed-delta comparisons limit one poll
window to less than 2^31 microseconds, which bounds G1's 240 BPM catch-up to
8,590 beats. `delay()` is not used.

Deadline comparisons use `static_cast<int32_t>(now_us - deadline_us) >= 0`,
which is safe across the `uint32_t micros()` rollover as long as polling stays
inside that half-wrap window.

G3 adds a pure `BpmInput` mapper: it converts the Arduino ADC range 0--1023
to 40--240 BPM using integer arithmetic and only accepts changes of at least
2 BPM. `main.cpp` samples A0 every 25 ms without blocking. When a value is
accepted, it updates `Tempo` and calls `Clock::setTempo(tempo, micros())`.
While running, that replaces the interval and schedules the next beat one new
interval after the current time. It emits no synthetic beat; subsequent beats
again use accumulated deadlines.

`BeatSequence` starts inactive at its downbeat position. After G0 turns every
LED off, `main.cpp` starts the clock; its first event activates position zero
(D2). Subsequent events advance D3, D4, D5 and wrap to D2. A delayed clock
poll advances the sequence with the full returned count using modular
arithmetic, so a four-beat catch-up preserves the current position.

G4 adds pure `Debouncer`, `TapTempo`, `TempoControl`, and `Transport` domain
models. Buttons are active-low `INPUT_PULLUP` GPIO inputs, and each stable LOW
transition after 30 ms yields one press event; release re-arms it. Start/Stop
stops the clock and leaves LEDs off. Restart resets the sequence and schedules
Beat 1/D2 one complete interval later. Startup remains automatic after the G0
sanity indication.

Tap Tempo stores up to three valid intervals (four taps) and applies integer
`60,000,000 / average_interval_us`. The first tap only starts a sequence. A
gap over 2000 ms, or an interval below 250 ms or above 1500 ms, restarts the
sequence at the current tap without changing BPM. Accepted taps set TAP_CONTROL
and capture the current A0 ADC reading. Potentiometer samples continue, but
only movement of at least 12 ADC counts returns POTENTIOMETER_CONTROL; its
existing 2-BPM hysteresis then applies.
