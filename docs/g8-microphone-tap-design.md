# G8 microphone tap-detection design

## Status and scope

| Item | Status |
| --- | --- |
| G8A design | COMPLETE |
| Microphone hardware identity | PENDING |
| Signal characterization | PENDING HUMAN MEASUREMENT |
| G8B detector implementation | BLOCKED until characterization evidence exists |

G8A defines an experimental acoustic-transient detector. It does not add a
microphone, assign an Arduino pin, change EEPROM, or change normal metronome
behavior. In particular, A0 remains the BPM potentiometer input. No microphone
module, output voltage range, bias, or available analog input is documented in
this repository; the future input is therefore only a generic analog
microphone/envelope signal. Its electrical range and compatibility with the
Uno ADC must be confirmed on the installed hardware before connecting it.

The detector emits a logical tap, never BPM. Future integration is one
serialized source path, preserving manual tapping:

```text
manual button ----> TapTempo ----> TempoControl ----> Tempo / Transport
microphone ADC -> MicTapDetector --^
```

Conceptually, firmware will call `tapTempo.tap(nowUs)` only when the detector
reports one new transient. Both sources share the same `TapTempo` history and
interval validation; neither gets a separate BPM calculator. Simultaneous
sources are serialized in loop order and treated as taps in that one stream.

## Candidate detector shape (not implemented)

`MicTapDetector` will be an Arduino-independent, fixed-size integer state
machine. A small API such as `bool update(uint16_t sample, uint32_t nowUs)` is
appropriate, but G8B may refine it without changing this contract:

```text
ADC sample -> slow baseline -> absolute deviation -> short envelope
           -> trigger threshold -> refractory/release -> one tap event
```

For every sample, estimate the DC baseline with a slow integer exponential
moving average. Calculate the absolute sample-to-baseline deviation, then feed
it into a faster attack/slower release envelope. A tap requires the envelope to
cross a trigger threshold above the estimated noise floor. Do not trigger on
raw amplitude: a steady loud signal can have a high baseline but little
deviation and must not repeatedly tap.

Maintain separate values for:

- **trigger threshold**: noise floor plus a measured margin; crossing it may
  create one event only when armed and outside the refractory window.
- **release threshold**: lower than trigger (hysteresis); the detector re-arms
  only after the envelope falls below it.
- **refractory interval**: a time gate after an event, independent of release,
  which rejects immediate ringing and multiple peaks from one impact.

Baseline and noise estimators must adapt gradually during quiet operation and
must freeze or slow substantially while triggered/refractory, so a clap does
not become the new baseline. All arithmetic should use bounded integer state;
no allocation, FFT, external DSP library, or float-heavy processing is needed.
Saturated ADC readings (0 or 1023) must be counted/flagged for diagnostics and
must not establish a calibrated threshold. A clipped impact may emit at most
one event, subject to the same armed/refractory rules.

Initial constants are deliberately not specified as calibrated values. G8B may
use conservative, explicitly provisional defaults only after measurements show
the signal range, idle noise, attack, decay, and clipping behavior.

## Sampling and timing contract

Start characterization at a non-blocking 2 kHz cadence (one sample every
500 us), scheduled from `micros()` with rollover-safe subtraction. At this
cadence a human impact is sampled several times while retaining approximately
0.5 ms event timing granularity. It is far below audio-rate DSP, and a normal
Uno ADC conversion is short enough to leave the loop responsive, but G8B must
measure loop headroom on the actual wiring.

A lower cadence reduces CPU/ADC load but can miss narrow peaks and increases
timing jitter; a higher cadence improves peak capture and costs more loop time
and sensitivity to high-frequency noise. The sampler must take at most one due
sample per loop pass (or explicitly account for a bounded catch-up policy),
never use a blocking sampling loop or `delay()`.

The existing `TapTempo` accepts intervals from 250,000 to 1,500,000 us: 240
BPM is 250 ms and 40 BPM is 1,500 ms. Acoustic refractory is a separate,
shorter measured property that suppresses ringing from one sound while leaving
250 ms high-BPM taps possible. `TapTempo`, not the detector, remains the final
authority for useful tempo intervals, timeout, smoothing, and BPM calculation.

## Hardware characterization gate

G8B is blocked until a human records the microphone module identity, supply and
output range/bias, intended ADC connection, and the following results. Do not
claim these observations until they are measured.

For each condition, record idle ADC baseline/range, typical deviation, peak,
clipping, number of peaks per physical event, and approximate decay time:

1. quiet room baseline; normal speaking near the sensor; hand clap; finger tap;
2. kick/snare or practice-pad hit; bass/guitar amplifier sound; sustained loud sound;
3. repeated taps at about 60, 120, and 200--240 BPM; rapidly decaying impacts;
4. sensor at several distances from each source.

The operator should capture a bounded sample summary and event count for each
run, then select trigger margin, hysteresis, and refractory from the observed
worst-case ringing/noise rather than a guessed module specification. Drums,
amplified instruments, speech, room noise, feedback, vibration, and acoustic
echo can all create false positives; a microphone cannot reliably distinguish
intentional taps from those sources without measured limits or a later product
decision.

## Characterization diagnostics

Do not add diagnostic firmware in G8A: it would affect production firmware
before a documented input/pin exists. G8B may add a compile-time experimental
serial mode that reports a bounded line such as `raw`, `baseline`, `deviation`,
`envelope`, `threshold`, `trigger`, and saturation count at 10--20 Hz, plus an
immediate one-line event marker. It must never print each 2 kHz sample and must
remain disabled in normal firmware.

## Future deterministic native tests

G8B tests will feed fixed sample/time fixtures into the Arduino-independent
detector: stable quiet signal; fixed small noise pattern; one sharp transient;
ringing transient; two separated taps; peaks during refractory; sustained loud
signal; slow baseline drift; clipping; and 200--240 BPM separated taps. They
must assert event count and timestamps, including no more than one event for
each single transient/ringing/clipped physical event. No fixture may depend on
randomness or real ADC input.

## Persistence boundary

Do not persist baseline, noise, envelope, triggered state, refractory state, or
microphone state. G7's EEPROM schema remains unchanged. Future user-adjustable
microphone sensitivity needs a separate schema and persistence decision.
