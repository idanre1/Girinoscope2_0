# Girinoscope2_0

To the circuit itself:
The third op-amp stage isn't really needed. Did you try to run without it? (make sure the unused op-amps inputs are tied to VCC and VSS, respectively, to avoid oscillations).
Second, to protect the input stage against overvoltage I'd add clamping diodes to VCC and VSS, respectively, right at the place where the 1 MOhm resistor is. Good, cheap, fast, low-capacitance diodes would be e.g. 1N914.


LXARDOSCOPE
