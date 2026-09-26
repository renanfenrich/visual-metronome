# Roadmap

- [x] G0 - Repository/bootstrap
- [x] G1 - Deterministic tempo clock
- [x] G2 - Four-LED visual 4/4 sequence
- [x] G3 - Potentiometer BPM control
- [x] G4 - Start/Stop and Tap Tempo
- [x] G5 - Time-signature mode selection
- [x] G6A - Grouped rhythmic patterns
- [x] G6B - Eight LEDs and binary beat rendering
- [x] G6C - Accent-strength rendering (implementation, software validation,
  firmware upload, and human physical visual validation complete)
- [x] G7 - EEPROM presets (implementation, software validation, and
  human-observed physical validation complete)
- [ ] G8 - Experimental microphone tap detection
- [ ] G9 - MIDI Clock

G0 through G7 are complete. G6A adds grouped 3/4, 4/4, 5/4, 6/8, and 7/8
patterns; G6B maps their positions onto eight LEDs; G6C uses non-blocking
150/100/60 ms PRIMARY/SECONDARY/NONE pulses, capped at half of the current beat
interval. Human observation passed every G6C acceptance check, including all
eight LEDs at startup, accent differentiation, grouped meters, high-BPM pulse
separation, control responsiveness, mode/stop clearing, and LED 8 reservation.
G6 is complete.

G7's [EEPROM persistence design](g7-eeprom-persistence-design.md) is
implemented and passed deterministic native validation plus human-observed
physical validation on the Arduino Uno. The confirmed hardware behavior is:
BPM and time-signature restoration after the two-second quiet period, combined
restoration, preservation of the prior preset when power interrupts a pending
change, transient transport and visual state, responsive rapid input, and
working Tap Tempo and potentiometer pickup after restore. Hardware corruption
injection and torn-record manipulation were not performed.
