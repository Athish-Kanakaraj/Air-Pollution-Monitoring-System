# 🍁 Air Pollution Monitoring & IoT Reporting System

This project is a smart environmental monitoring system built using an **Arduino UNO (SMD)** and an **ESP-01 WiFi module**.  
The system continuously measures **temperature**, **humidity**, **air quality**, **carbon monoxide**, and **dust concentration**, then uploads the data to **ThingSpeak** for real-time monitoring.

This setup helps track indoor or outdoor pollution levels and provides live environmental insights.

---

## 🌟 What This System Does

### ✔ Monitors air quality parameters  
- Temperature & Humidity (DHT11)  
- Air Quality Index (MQ-135)  
- Carbon Monoxide Levels (MQ-7)  
- Dust Density (Optical Dust Sensor)

### ✔ Uploads live IoT data  
- Sends all sensor readings to **ThingSpeak**  
- Updates occur every **20 seconds**  
- Uses **ESP-01 with AT commands** for Wi-Fi communication  

### ✔ Controls environment (optional)  
- Built-in fan control pin for ventilation  
- Can be expanded to include alarms or auto-control systems  

---

## 🧠 Why This Project Exists

Air pollution affects millions of people every day, yet real-time monitoring is often not accessible or affordable.  
This project provides a simple, low-cost solution that:

- Monitors essential air parameters  
- Alerts users of rising pollution  
- Allows remote monitoring via IoT  

It is ideal for homes, classrooms, labs, and small industries.

---

## 🔌 Hardware Used

- **Arduino UNO (SMD Version)**  
- **ESP-01 WiFi Module**  
- **DHT11 Temperature & Humidity Sensor**  
- **MQ-135 Air Quality Sensor**  
- **MQ-7 Carbon Monoxide Sensor**  
- **Dust Sensor (Optical / Sharp GP2Y1010)**  
- **16×2 LCD (optional)**  
- **Relay / Fan Module (optional ventilation control)**  
- **Jumper wires, breadboard, power supply**

---

## 🛠 How It Works

### 1. Environment Monitoring  
The Arduino continuously reads values from:  
- DHT11 for temperature and humidity  
- MQ135 for general air pollutants  
- MQ7 for carbon monoxide  
- Optical dust sensor for particulate matter  

### 2. Data Processing  
Each sensor value is:  
- Read and validated  
- Printed to Serial Monitor  
- Optionally displayed on LCD  

### 3. IoT Uploading  
Every cycle, the ESP-01 sends a **HTTP GET request** to ThingSpeak containing:  
- Temperature  
- Humidity  
- Air quality  
- CO level  
- Dust concentration  

### 4. Control Logic
A fan or exhaust system can be activated when pollution levels rise.

---

## 📡 IoT Connectivity

- The ESP-01 connects to Wi-Fi using AT commands  
- Data is uploaded using a ThingSpeak channel  
- Updates appear on graphs for remote monitoring
