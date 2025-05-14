🕒 AT89S52 Digital Clock with DS1307 RTC Module
This project is a simple 6-digit digital clock built using the AT89S52 microcontroller (8051 family) and the DS1307 Real-Time Clock (RTC) module. It displays time in HH:MM:SS format on a 6-digit 7-segment display and includes buttons to set the time manually.

🔧 Features
-- Real-time timekeeping using DS1307 RTC (I²C communication)

-- 6-digit 7-segment display for HH:MM:SS

-- Button interface for time setting (Hours, Minutes, Mode select)

-- Port 0 used for segment data

-- Port 2 used for digit control

-- Keeps time even when powered off using a CR2032 coin cell

🛠️ Hardware Used
** AT89S52 microcontroller (8051-based)

** DS1307 RTC module

** 6 x common cathode 7-segment displays

** Push buttons for time setting

** 3V coin cell battery

** External 12MHz crystal oscillator

** Resistors, capacitors, jumper wires

🖥️ Software & Tools
Embedded C

== Keil µVision IDE

== Proteus (for simulation)

Programmer (e.g., USBasp) for flashing the microcontroller
