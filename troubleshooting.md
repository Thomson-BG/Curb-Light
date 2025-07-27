# 🚗 Curb Light System Troubleshooting Guide

---

## ⚡ No curb lights are turning on

- Verify that the curb lights are correctly wired to the MOSFETs and 12V power.
- Check that the MOSFET sources are grounded.
- Make sure your curb lights are compatible with PWM (for fading effect).
- Confirm that the 12V-to-5V buck converter is providing 5V to the Arduino.

---

## 📡 GPS module not working

- Check wiring: GPS TX → Arduino D2, VCC → 5V, GND → GND.
- Place the GPS antenna somewhere with a clear view of the sky (dashboard or rear shelf).
- Use Serial Monitor to check for GPS fix data.

---

## 🔘 Dash button does not work

- Confirm one side of the button is to GND, the other to D7.
- Use INPUT_PULLUP in code.
- For latching (toggle) behavior, verify code matches your button type.

---

## 💡 Dash indicator LED never lights up

- Ensure the long leg (anode) is to D8 and the short leg (cathode) is to GND.
- Use a 220Ω resistor in series with the LED.
- Confirm your curb light wiring is correct and that PWM pins are being used.

---

## 🟧 Turn signal activation not detected

- Confirm that the car's turn signal wire goes through a 1kΩ resistor into the optocoupler anode.
- Double-check optocoupler orientation (anode/cathode, collector/emitter).
- Ensure the optocoupler collector connects to D3 (left) or D4 (right), and emitter to GND.

---

## 🛠️ Fade effect is too slow or too fast

- Adjust `FADE_STEP` (higher is faster) and/or `FADE_DELAY` (lower is faster) in the Arduino code.

---

## 🚨 Still having issues?

- Post a photo of your wiring and code version to the repo Discussions or open an Issue.
- Double check all pin assignments in the code match your wiring.
- Make sure your Arduino is not being powered directly from 12V—always use a buck converter or regulator.

---
