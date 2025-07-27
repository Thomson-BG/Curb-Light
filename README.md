# Curb-Light
Arduino controlled curb light - Uses GPS and turn signal input to control added curb lights

# 🚗 Arduino Curb Light System

This project adds BMW-style "curb/parking lights" to your vehicle using an Arduino, a GPS module, and a few simple components. Curb lights illuminate when the turn signal is active and vehicle speed is low, or by driver override with a dash button. A dash LED indicates when either curb light is on.

---

## ✨ Features

- **Automatic curb lights:** Activate when speed < 20 mph and turn signal is on
- **Smooth fade on/off:** No harsh flashes (PWM fade)
- **5 second timeout:** Lights stay on for 5 seconds after last signal
- **Dash override button:** Turns both lights on at any speed
- **Dash indicator LED:** Lights up when either curb light is on
- **Safe opto-isolated inputs:** For reliable signal detection

---

## 🛠️ Hardware Required

- Arduino Uno/Nano
- NEO-6M GPS module
- 2x Optocouplers (e.g. 4N35)
- 2x N-channel MOSFETs (e.g. IRLZ44N)
- 2x Curb lights (LED or bulb)
- 12V to 5V buck converter
- Dash toggle button
- Dash indicator LED + 220Ω resistor
- 2x 1kΩ resistors (for optocouplers)
- Breadboard/jumper wires/heat shrink

---

## 🧰 Directory Structure

```
arduino-curb-light-system/
│
├── README.md
├── LICENSE
├── wiring_guide.pdf
├── CURB_LIGHT_INSTALL_GUIDE.md
├── code/
│   └── curb_light.ino
├── fritzing/
│   └── curb_light_schematic.fzz
│   └── curb_light_schematic.png
├── images/
│   └── dash_button_example.jpg
│   └── curb_light_mounted.jpg
└── docs/
    └── troubleshooting.md
```

---

## 📖 Documentation

- [Installation & Wiring Guide (Markdown)](CURB_LIGHT_INSTALL_GUIDE.md)
- [Wiring Guide PDF](wiring_guide.pdf)
- [Fritzing Schematic](fritzing/curb_light_schematic.fzz)
- [Troubleshooting](docs/troubleshooting.md)

---

## 💾 Arduino Code

See [`code/curb_light.ino`](code/curb_light.ino)

---

## 🚘 Installation

See [CURB_LIGHT_INSTALL_GUIDE.md](CURB_LIGHT_INSTALL_GUIDE.md) for a complete step-by-step guide with diagrams and tips.

---

## 🖼️ Visuals

- ![Wiring Diagram](fritzing/curb_light_schematic.png)
- ![Dash Example](images/dash_button_example.jpg)

---

## 📄 License

MIT License (see LICENSE)
