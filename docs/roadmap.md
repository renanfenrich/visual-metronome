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
- [ ] G7 - EEPROM presets
- [ ] G8 - Experimental microphone tap detection
- [ ] G9 - MIDI Clock

G0 through G6C are implemented. G6A adds grouped 3/4, 4/4, 5/4, 6/8, and 7/8
patterns; G6B maps their positions onto eight LEDs; G6C uses non-blocking
150/100/60 ms PRIMARY/SECONDARY/NONE pulses, capped at half of the current beat
interval. Human observation passed every G6C acceptance check, including all
eight LEDs at startup, accent differentiation, grouped meters, high-BPM pulse
separation, control responsiveness, mode/stop clearing, and LED 8 reservation.
G6 is complete.
