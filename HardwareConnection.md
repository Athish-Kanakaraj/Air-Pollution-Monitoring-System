# 🔌 Hardware Connections – Air Pollution Monitoring System

Below are the wiring details for all sensors, modules, and components used in the system.

---

## 🧠 Arduino UNO Pin Mapping

### 🌡 DHT11 (Temperature & Humidity)
| DHT11 Pin | Arduino UNO Pin |
|-----------|-----------------|
| VCC       | 5V              |
| DATA      | D2              |
| GND       | GND             |

---

## 🧪 MQ-135 (Air Quality Sensor)
| MQ-135 Pin | Arduino UNO Pin |
|------------|------------------|
| AO         | A0               |
| VCC        | 5V               |
| GND        | GND              |

---

## 🚨 MQ-7 (Carbon Monoxide Sensor)
| MQ-7 Pin | Arduino UNO Pin |
|----------|------------------|
| AO       | A1               |
| VCC      | 5V               |
| GND      | GND              |

---

## 🌫 Dust Sensor (GP2Y1010 or Similar)
| Dust Sensor Pin | Arduino UNO Pin |
|------------------|-----------------|
| LED Control      | D3 (PWM Pulse)  |
| AO               | A2              |
| VCC              | 5V              |
| GND              | GND             |

⚠ **Note:**  
The LED pin is pulsed LOW and HIGH for dust sampling timing.

---

## 📡 ESP-01 WiFi Module (SoftwareSerial)
| ESP-01 Pin | Arduino UNO Pin |
|------------|------------------|
| TX →       | D10 (SoftwareSerial RX) |
| RX ←       | D11 (SoftwareSerial TX, use 3.3V level shifting) |
| VCC        | 3.3V (External Regulator Recommended) |
| CH_PD / EN | 3.3V |
| GND        | GND |

⚠ **Important:**  
- Do **NOT** power ESP-01 directly from Arduino 3.3V pin if unstable.  
- Use a proper **3.3V 300–500mA** regulator for safe operation.

---

## 🌀 Fan / Ventilation Control (Optional Relay)
| Function | Arduino UNO Pin | Relay Input |
|----------|------------------|-------------|
| Fan      | D4               | IN1         |

### Relay Power:
- VCC → 5V  
- GND → GND  
- **Active-low logic:**  
  - `LOW` → ON  
  - `HIGH` → OFF  

---

## 🧷 Power Notes
- Arduino UNO powered via USB or 5V adapter  
- ESP-01 requires **3.3V stable supply**  
- All sensors should share a **common ground**  
- MQ sensors require **warming time** for accurate readings  
