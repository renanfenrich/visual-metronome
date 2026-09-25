# Architecture

The `Tempo`, `Clock`, and `Pattern` modules are small C++ domain models with
no Arduino headers. `main.cpp` owns GPIO and calls `Clock::update(micros())`.
This keeps musical rules testable on the host and prevents them from being
coupled to `digitalWrite()`.

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
inside that half-wrap window. The pattern model remains independent of clock
events until G2.
