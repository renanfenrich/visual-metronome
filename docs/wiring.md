# G0 wiring

Use one current-limiting resistor for each LED:

| Arduino pin | Connection |
| --- | --- |
| D2 | LED 1 -> resistor -> GND |
| D3 | LED 2 -> resistor -> GND |
| D4 | LED 3 -> resistor -> GND |
| D5 | LED 4 -> resistor -> GND |

Use approximately 220-470 ohm resistors for this prototype. Respect LED
polarity: the anode connects toward the Arduino pin and the cathode toward the
resistor/GND path. Verify the exact resistor values before assembly; they must
not be inferred from photographs.
