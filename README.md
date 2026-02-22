# AquaCare: Full-Stack IoT Smart Irrigation & Farm Monitoring System 



AquaCare is an end-to-end AgriTech platform designed to solve the lack of real-time visibility and equipment monitoring in modern farming. It bridges physical farm hardware with a cloud-based dashboard, enabling farmers to monitor soil vitals, track water reserves, and instantly detect hardware faults before they cause crop loss.



## System Architecture

The system is decoupled into three main layers:

### 1. The Edge (Hardware & Telemetry)
* **Microcontroller:** ESP32 (Programmed in C++)
* **Sensors:** DHT11 (Temperature/Humidity), Capacitive Soil Moisture Sensor, Ultrasonic Sensor (Reservoir level).
* **Actuator:** 1 single pump
* **Data Pipeline:** The ESP32 continuously polls the sensors, serializes the readings into a structured JSON payload, and pushes the data to the backend via HTTP POST requests.

### 2. The Brain (Backend API)
* **Framework:** FastAPI (Python)
* **Database:** SQLite (Ignored in version control for security)
* **Connectivity:** Exposed globally via a secure Ngrok tunnel, bypassing local firewall restrictions and allowing remote hardware to communicate with the local server.

### 3. The Dashboard (Frontend Interface)
* **Telemetry:** Real-time synchronized graphs charting temperature, humidity, and soil moisture.
* **Predictive Maintenance:** Automated fault detection for farm equipment (e.g., flagging when a water pump drops offline or fails).
* **Data Ownership:** One-click CSV export functionality allowing farmers to download historical data for offline analysis and record-keeping.

---

# Hardware Wiring

Components
1. ESP32 
2. DHT11  D4 (GPIO 4) for  Temp & Humidity Data 
3. Soil Moisture Sensor VP (GPIO 36) for Analog Soil Reading 
4. Ultarsonic Sensor -  Trigger_pin  on D5 (GPIO 5) and Echo_pin on D18(GPIO18)
5.Pump
6.Relay
7.Resistors
8.Vero board



# Local Setup & Installation

Follow these steps to run the backend and dashboard locally. 

# Prerequisites
* Python 3.9+
* Ngrok account

1. Clone the Repository

git clone [https://github.com/Opeyemi-Builds/Aqua_Care.git]
   

2. Setup the virtual environment
python -m venv venv
source venv/bin/activate  # On Windows use: venv\Scripts\activate

3. Install Dependencies
pip install -r requirements.txt

(Note: The database is intentionally excluded via .gitignore and .env securely manages the Weather API keys. Run the server to auto-generate local tables).


4. run the backend server
uvicorn main:app --reload

5.Expose via ngrok
ngrok http 8000

## The Team

* **ALEGBELEYE OPEYEMI MICHEAL** - System Architecture, Backend Engineering & ESP32 Integration [https://github.com/Opeyemi-Builds] 
  * [www.linkedin.com/in/opeyemi-alegbeleye-110891363]

* **Hakeem Oyebisi** - Hardware & Firmware Engineering [https://github.com/oyeniyihakeem08-blip]
  * [https://www.linkedin.com/in/hakeem-oyebisi-42b9a1333?utm_source=share&utm_campaign=share_via&utm_content=profile&utm_medium=ios_app]
