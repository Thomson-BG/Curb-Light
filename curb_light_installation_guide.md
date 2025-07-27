# 🚗✨ Arduino Smart Curb Light System – Installation Guide

---

## 📦 Parts List

- **Arduino Uno/Nano** (or compatible)
- **NEO-6M GPS Module**
- **2x Optocoupler modules** (PC817, 4N35, etc.)
- **2x N-channel MOSFETs** (IRLZ44N, IRF540N, etc.)
- **2x Side lights** (LED modules or automotive bulbs)
- **12V to 5V Buck Converter** (for Arduino power)
- **Dash Toggle Button** (momentary or latching)
- **Dash Indicator LED** (plus 220Ω resistor)
- **2x 1kΩ resistors** (for optocoupler input)
- **Jumper wires**
- **Breadboard or protoboard**
- **Heat shrink tubing, electrical tape, zip ties**

---

## 🛡️ Safety First!

- **Disconnect your vehicle battery** before wiring. 🔋❌
- Use fuses on power lines for extra safety. ⚡
- Double-check connections and insulation to prevent shorts. 🔍

---

## 🛠️ Step-by-Step Installation

### 1️⃣ Prepare Work Area

- Park your vehicle in a well-lit, safe location. 🚗🔦
- Gather all parts and tools. 🧰

---

### 2️⃣ Wiring Overview

#### **Main Diagram**

```
[Car 12V]----[Buck Converter]---(5V to Arduino)
         |                      |
         +--[12V]--[Curb Lights]----[MOSFETs]----[Arduino D5/D6]
         |                      |
         +--[Turn Signal Wires]----[Optocouplers]----[Arduino D3/D4]
         |                      |
         +--[Dash Button]----[Arduino D7]
         +--[Dash LED]----[Arduino D8]
         +--[GPS TX]----[Arduino D2]
```

---

#### **Pin Map**

| Function                   | Arduino Pin | Connection                      |
|----------------------------|-------------|---------------------------------|
| GPS TX                     | D2          | GPS Module TX                   |
| Left Turn Signal           | D3          | Optocoupler 1 output            |
| Right Turn Signal          | D4          | Optocoupler 2 output            |
| Left Curb Light            | D5 (PWM)    | MOSFET 1 gate                   |
| Right Curb Light           | D6 (PWM)    | MOSFET 2 gate                   |
| Dash Toggle Button         | D7          | One side to GND, one to D7      |
| Dash Indicator LED         | D8          | Anode to D8, cathode to GND     |

---

### 3️⃣ Detailed Wiring Steps

#### 🛰️ **GPS Module**

- GPS **TX** → Arduino **D2**
- GPS **VCC** → Arduino **5V**
- GPS **GND** → Arduino **GND**
- Mount the GPS antenna on the dashboard for sky visibility. 🌤️

---

#### 💡 **Curb Lights (Side Lights)**

- **Left Light:**  
  - **Anode (+)** → Car **12V**  
  - **Cathode (-)** → **Drain** on MOSFET 1  
  - **MOSFET 1 Source** → **GND**  
  - **MOSFET 1 Gate** → **D5** (Arduino PWM pin)

- **Right Light:**  
  - **Anode (+)** → Car **12V**  
  - **Cathode (-)** → **Drain** on MOSFET 2  
  - **MOSFET 2 Source** → **GND**  
  - **MOSFET 2 Gate** → **D6** (Arduino PWM pin)

---

#### 🟧 **Optocoupler (Turn Signal Detection)**

- **Left Turn Signal Wire**  
  - Tap into the car’s left signal wire  
  - Signal → **1kΩ resistor** → optocoupler **anode (+)**  
  - Optocoupler **cathode (-)** → **GND**  
  - Optocoupler **collector** → Arduino **D3**  
  - Optocoupler **emitter** → **GND**

- **Right Turn Signal Wire**  
  - Same as above to **D4**

---

#### 🔘 **Dash Toggle Button**

- One side → **GND**
- Other side → **D7**
- Use `INPUT_PULLUP` in code (button is **active LOW**)

---

#### 🟢 **Dash Indicator LED**

- **Anode (+)** → **D8** (through 220Ω resistor)
- **Cathode (-)** → **GND**

---

#### 🔋 **Power**

- Use a **12V-to-5V buck converter** to power Arduino from car’s 12V supply.
- Connect buck converter **output** to Arduino **5V** and **GND**.

---

### 4️⃣ Mounting Components

- **Arduino:** Secure under dash or in electronics compartment. 🛠️
- **GPS Module:** Place for clear sky view (dash or rear shelf). 🛰️
- **Optocouplers/MOSFETs:** Mount on a protoboard in a dry area.
- **Curb Lights:** Secure to the underside of the mirrors or on the rocker panels.
- **Dash Switch & LED:** Drill small holes in blank dash area for a tidy look. 🔩

---

### 5️⃣ Final Steps

- Double-check all connections with a multimeter! 🧲
- Insulate all exposed wires with heat shrink or tape.
- Secure harnesses with zip ties.
- **Reconnect battery.** 🔋✅
- Upload the Arduino code and test the system.
- Verify:
  - Curb lights fade on with turn signals (under 20mph), and fade off 5s after.
  - Dash button turns both curb lights on.
  - Dash LED is ON when either curb light is lit.

---

### 6️⃣ Troubleshooting

- **No curb light?**  
  - Check MOSFET wiring, Arduino pin assignment, and optocoupler orientation.
- **Dash LED not working?**  
  - Confirm polarity and resistor value.
- **GPS not locking?**  
  - Move antenna for better sky view.
- **No response to toggle button?**  
  - Verify ground and correct pin in code.

---

## 🎉 Enjoy Your BMW-Style Curb Light System!

Safer, smarter turns, and a clean professional dash look.  
Happy modding! 🚗💡🛠️

---
