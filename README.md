# ATtiny45-IR-Projector-Remote

> **Target Device:** Casio Projectors (NEC Protocol)
>
> **A minimalist, custom-designed IR remote control built on the ATtiny45 microcontroller.**

## 📌 Project Overview
This project replaces bulky commercial remotes with a compact, PCB-based solution specifically designed to control **Casio Projectors**. It utilizes the **ATtiny45** microcontroller to generate raw infrared signals. The system is designed with **Altium Designer** and optimized for low power consumption using internal pull-up resistors and efficient bit-banging for the 38kHz carrier frequency.

### 🚀 Universal Application (Adaptable for Any Device)
While this specific firmware is hardcoded for Casio projectors, the hardware and logic are **universal**. You can adapt this project to control **TVs, Air Conditioners, Stereos, or any IR-controlled device**.

**How to adapt it to your device:**
1.  **Sniff the Codes:** Use an Arduino Uno and an **IR Receiver (e.g., TSOP4838)** with the standard `IRremote` library's "ReceiveDump" example.
2.  **Record the Data:** Point your original remote at the Arduino and press the buttons to see the HEX codes or Raw Timing data.
3.  **Update the Code:** Replace the `0x0B`, `0x4A` etc. commands in the `.ino` file with your device's codes.

---

## 🛠️ Hardware Specifications
* **Microcontroller:** ATtiny45 (Running at 8MHz Internal Oscillator)
* **IR Emitter:** 940nm Infrared LED driven by a transistor (NPN BC547 or 2N2222)
* **Input:** 4x Tactile Buttons (Power, Input, Up, Down)
* **Power Source:** 3V Coin Cell (CR2032) or External 5V

### 🔌 Pin Mapping (Physical Pins)
| Function | ATtiny Pin | Physical Pin |
| :--- | :--- | :--- |
| **IR Output** | PB0 | Pin 5 |
| **BTN_POWER** | PB2 | Pin 7 |
| **BTN_INPUT** | PB1 | Pin 6 |
| **BTN_DOWN** | PB3 | Pin 2 |
| **BTN_UP** | PB4 | Pin 3 |
| **VCC** | VCC | Pin 8 |
| **GND** | GND | Pin 4 |

## 📂 PCB & Schematics
The hardware design files (Schematic and PCB layout) created in **Altium Designer** are included in the `/Hardware` folder of this repository.

* `Projector_Remote_Schematic.Pdf`
* `Projector_Remote_PCB.PcbDoc`

> **Note:** The design uses internal pull-up resistors on the ATtiny45, simplifying the PCB routing by removing the need for external resistors on the buttons.

---

## 💻 Code & Logic
The firmware implements a raw IR protocol specific to Casio devices.

* **Carrier Frequency:** The **38kHz** carrier wave is generated manually using `delayMicroseconds()` tuned for the 8MHz clock cycle overhead.
* **Protocol Structure:** `[Leader Code] + [Address 1] + [Address 2] + [Command] + [Inverted Command] + [Stop Bit]`
* **Error Checking:** The system sends `~cmd` (inverted command) to allow the receiver to verify data integrity.

---

## ⚙️ How to Program (Flashing the ATtiny45)

Since the ATtiny45 does not have a USB port, you need an ISP (In-System Programmer). The most common method is using an **Arduino Uno as ISP**.

### Prerequisites
1.  **Arduino IDE** installed.
2.  **ATTinyCore** installed in Arduino Board Manager.
    * *URL:* `http://drazzy.com/package_drazzy.com_index.json`

### Step-by-Step Instructions
1.  **Prepare Arduino Uno:**
    * Open Arduino IDE -> `File` -> `Examples` -> `11.ArduinoISP` -> `ArduinoISP`.
    * Upload this sketch to your Arduino Uno.
    * *Note:* Add a 10uF capacitor between Reset and GND on the Uno (Stripe to GND) to prevent it from resetting itself.

2.  **Wiring (Uno -> ATtiny45):**
    * Uno **5V** -> ATtiny **Pin 8 (VCC)**
    * Uno **GND** -> ATtiny **Pin 4 (GND)**
    * Uno **Pin 13** -> ATtiny **Pin 7 (SCK)**
    * Uno **Pin 12** -> ATtiny **Pin 6 (MISO)**
    * Uno **Pin 11** -> ATtiny **Pin 5 (MOSI)**
    * Uno **Pin 10** -> ATtiny **Pin 1 (RESET)**

3.  **Burn Bootloader (First Time Only):**
    * Tools -> Board: **"ATtiny25/45/85"**
    * Tools -> Chip: **"ATtiny45"**
    * Tools -> Clock: **"8 MHz (Internal)"** <--- *Crucial for timing!*
    * Tools -> Programmer: **"Arduino as ISP"**
    * Click **"Burn Bootloader"**.

4.  **Upload Code:**
    * Open the `.ino` file from this repository.
    * Click **Upload**.

---

## 🧪 Testing the Circuit

Once the code is uploaded and the PCB is assembled:

1.  **Camera Test:**
    * Since human eyes cannot see IR light, use a smartphone camera.
    * Point the remote's LED at the camera lens.
    * Press a button. You should see a purple/bluish light flickering on the phone screen. This confirms the code is running and the LED is firing.

2.  **Frequency Check (Optional - Oscilloscope):**
    * Connect an oscilloscope probe to **Pin 5 (PB0)**.
    * Press a button and measure the burst frequency. It should be approximately **38kHz** (Period ~26µs).

3.  **Functional Test:**
    * Aim at the target **Casio Projector**.
    * Ensure the buttons perform the correct actions (Power On/Off, Menu, Up, Down).
