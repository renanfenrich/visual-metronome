# G6B wiring

Use one current-limiting resistor for each LED:

| Arduino pin | Connection |
| --- | --- |
| D2 | Beat 1 LED -> resistor -> GND |
| D3 | Beat 2 LED -> resistor -> GND |
| D4 | Beat 3 LED -> resistor -> GND |
| D5 | Beat 4 LED -> resistor -> GND |
| D9 | Beat 5 LED -> resistor -> GND |
| D10 | Beat 6 LED -> resistor -> GND |
| D11 | Beat 7 LED -> resistor -> GND |
| D12 | Beat 8 LED -> resistor -> GND |

Use approximately 220-470 ohm resistors for this prototype. Respect LED
polarity: the anode connects toward the Arduino pin and the cathode toward the
resistor/GND path. Verify the exact resistor values before assembly; they must
not be inferred from photographs.

After the startup sanity check, all eight LEDs are off until the first clock
deadline. Then exactly one LED is on at a time. 3/4 uses LEDs 1-3, 4/4 uses
LEDs 1-4, 5/4 uses LEDs 1-5, 6/8 uses LEDs 1-6, and 7/8 uses LEDs 1-7. LED 8
(D12) remains off for every current signature.

| Arduino pin | Connection |
| --- | --- |
| A0 | BPM potentiometer wiper; ends to 5V and GND |
| D6 | Start/Stop push button -> GND |
| D7 | Tap Tempo push button -> GND |
| D8 | Mode push button -> GND |

D6, D7, and D8 use `INPUT_PULLUP`: an unpressed button reads HIGH and a press
reads LOW. Firmware debounces each input for 30 ms without `delay()`. D8 cycles
through 4/4, 3/4, 5/4, 6/8, and 7/8. It resets the sequence while preserving
transport state, tempo, and the pending clock deadline.
