# G0 wiring

Use one current-limiting resistor for each LED:

| Arduino pin | Connection |
| --- | --- |
| D2 | Beat 1 LED -> resistor -> GND |
| D3 | Beat 2 LED -> resistor -> GND |
| D4 | Beat 3 LED -> resistor -> GND |
| D5 | Beat 4 LED -> resistor -> GND |

Use approximately 220-470 ohm resistors for this prototype. Respect LED
polarity: the anode connects toward the Arduino pin and the cathode toward the
resistor/GND path. Verify the exact resistor values before assembly; they must
not be inferred from photographs.

After the startup sanity check, all four LEDs are off until the first clock
deadline. Then exactly one LED is on at a time in D2, D3, D4, D5 order.
