# 🚗⚡ Hardware Specifications & Upgrade Guide

## 📋 Component Specifications

### Core Components
| Component | Recommended Model | Specifications | Notes |
|-----------|------------------|----------------|-------|
| **Microcontroller** | Arduino Nano 33 IoT | 32-bit ARM, WiFi, 3.3V/5V | Enhanced processing, wireless capabilities |
| **GPS Module** | u-blox NEO-8M | High sensitivity, fast fix | Better than NEO-6M for automotive use |
| **Optocouplers** | PC817 or 4N35 | CTR > 50%, VCE(sat) < 0.4V | Automotive-grade isolation |
| **MOSFETs** | IRLZ44N or STP55NF06L | Logic-level, Id > 50A | Automotive-rated power switching |

### Lighting Components
| Component | Specification | Power Rating | Notes |
|-----------|---------------|--------------|-------|
| **LED Strips** | 5630 SMD, 12V | 24W/meter | High brightness, automotive-grade |
| **Puddle Lights** | 3W LED modules | 3W @ 12V | Ground projection pattern |
| **Resistors** | 1/4W Metal Film | ±1% tolerance | Temperature stable |

### Power & Protection
| Component | Rating | Purpose |
|-----------|--------|---------|
| **Buck Converter** | LM2596, 3A output | 12V to 5V conversion |
| **Fuses** | ATO 5A, 10A | Circuit protection |
| **TVS Diodes** | SMAJ12A | Transient voltage suppression |
| **Capacitors** | 1000µF, 25V | Power supply filtering |

## 🔧 Professional Installation Hardware

### Wiring Harness Components
- **Automotive Wire**: 18 AWG stranded copper, TXL rated
- **Connectors**: Deutsch DT series waterproof connectors  
- **Heat Shrink**: 3:1 adhesive-lined tubing
- **Conduit**: Split-loom tubing for wire protection
- **Grommets**: Rubber grommets for body panel penetrations

### Mounting Hardware
- **Enclosure**: IP67 rated ABS enclosure for Arduino
- **LED Mounting**: Aluminum angle brackets with vibration dampening
- **Cable Management**: P-clips and zip ties for clean installation

## 📊 Power Consumption Analysis

| Mode | Current Draw | Duration | Power (Watts) |
|------|-------------|----------|---------------|
| **Standby** | 150mA | Continuous | 1.8W |
| **Single Light** | 2.5A | 5-30 seconds | 30W |
| **Both Lights** | 5A | 5-30 seconds | 60W |
| **Welcome Sequence** | 3A | 3 seconds | 36W |
| **Diagnostic Mode** | 4A | Variable | 48W |

**Daily Energy Usage**: ~0.1-0.5 kWh (typical driving)

## 🛡️ Safety & EMI Considerations

### Electromagnetic Compatibility (EMC)
- **Ferrite cores** on power and signal lines
- **Shielded cables** for GPS and sensitive signals
- **Ground plane** design for stable operation
- **Input filtering** to prevent interference

### Automotive Safety Standards
- **ISO 26262** functional safety compliance considerations
- **IP67** ingress protection rating for outdoor components
- **Operating temperature**: -40°C to +85°C
- **Vibration resistance**: 10G acceleration, 5-2000Hz

## 🔧 Advanced Features Hardware

### CAN Bus Integration (Future Enhancement)
- **MCP2515 CAN Controller** + **TJA1050 Transceiver**
- Integration with vehicle's lighting control module
- Real-time vehicle data access (speed, doors, lights)

### Ambient Light Sensing
- **TSL2561 Light Sensor**: Automatic brightness adjustment
- **Mounting**: Behind windshield for optimal sensing
- **Calibration**: Automatic day/night brightness switching

### Door Position Sensing
- **Hall Effect Sensors**: Non-contact door position detection
- **Reed Switches**: Simple magnetic door sensors
- **Integration**: With existing door courtesy light circuits

## 🔌 Connector Pinouts

### Main Harness Connector (12-pin Deutsch DT)
| Pin | Function | Wire Color | Connection |
|-----|----------|------------|------------|
| 1 | +12V Power | Red | Vehicle ignition/battery |
| 2 | Ground | Black | Chassis ground |
| 3 | Left Turn Signal | Yellow/Black | Turn signal wire |
| 4 | Right Turn Signal | Green/Black | Turn signal wire |
| 5 | Left Light Output | Yellow | Left curb light |
| 6 | Right Light Output | Green | Right curb light |
| 7 | Dash Button | Blue | Dashboard switch |
| 8 | Dash LED | Orange | Dashboard indicator |
| 9 | Door Sensor | Purple | Door position (optional) |
| 10 | Hazard Input | Brown | Hazard light signal |
| 11 | CAN High | White/Blue | CAN bus (future) |
| 12 | CAN Low | White/Green | CAN bus (future) |

### GPS Module Connector (4-pin JST)
| Pin | Function | Wire | Connection |
|-----|----------|------|------------|
| 1 | VCC | Red | +5V |
| 2 | GND | Black | Ground |
| 3 | TX | Yellow | Arduino D2 (RX) |
| 4 | PPS | Blue | Pulse per second (optional) |

## 🛠️ PCB Design Recommendations

### Layout Considerations
- **Separate analog and digital ground planes**
- **Wide power traces** (min 50 mil for high current)
- **Proper decoupling capacitors** near ICs
- **Thermal vias** under MOSFETs for heat dissipation

### Component Placement
- **MOSFETs**: Near board edge for heatsink mounting
- **Optocouplers**: Away from high-frequency circuits
- **Crystal oscillator**: Short traces, ground guard ring
- **Connectors**: Mechanical stress relief

## 📈 Performance Optimization

### Thermal Management
- **Heatsinks**: 10°C/W thermal resistance for MOSFETs
- **Thermal interface material**: Between MOSFET and heatsink
- **Airflow**: Natural convection design considerations
- **Temperature monitoring**: NTC thermistor for protection

### Signal Integrity
- **Differential pairs** for high-speed signals
- **Controlled impedance** traces (50Ω single-ended)
- **Ground plane** for stable reference
- **EMI shielding** for sensitive circuits

## 🔍 Testing & Validation Equipment

### Required Test Equipment
- **Digital Multimeter**: Fluke 87V or equivalent
- **Oscilloscope**: 100MHz, 4-channel minimum
- **Function Generator**: For signal injection testing
- **EMI Analyzer**: For electromagnetic compliance
- **Thermal Camera**: For temperature profiling

### Test Procedures
1. **Power-on self-test** validation
2. **Signal timing** measurements
3. **EMI emission** testing
4. **Thermal cycling** tests (-40°C to +85°C)
5. **Vibration testing** per automotive standards

## 📦 Production-Ready Packaging

### Manufacturing Considerations
- **RoHS compliance** for all components
- **Automotive-grade** component selection
- **Pick-and-place** friendly component packages
- **Conformal coating** for environmental protection

### Quality Assurance
- **In-circuit testing** (ICT) for manufacturing
- **Functional testing** with automated test equipment
- **Burn-in testing** for reliability validation
- **Statistical process control** for consistency