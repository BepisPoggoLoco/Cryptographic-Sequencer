# Basis

Named "Cryptographic Sequencer" as a reference to Batman's hacking device in the Arkham games. 

In actuality, this is an arduino based IR and RFID signal reader/cloner.

![Tuff Batman pic](https://teamliquid.com/staff/shiroiusagi/tlpro/0207_g2a/batrmanscreen03.jpg)

# Core Components

+ Arduino of any kind (I used an uno) <sub>(x1)</sub>

+ RC522 RFID reader/writer module <sub>(x1)</sub>

+ IR receiver module <sub>(x1)</sub>

+ IR LED <sub>(x1)</sub>

+ Pushbuttons <sub>(x3)</sub>

+ 0.96 inch I2C OLED <sub>(x1)</sub>

*I sourced components 1,2,3, and 5 from my ELEGOO UNO R3 Super Starter Kit and the rest from AliExpress*

# Schematic

# Code Structure 

The main file <sub>**Crytographic_Sequencer.ino**</sub> contains the OLED GUI code and calls functions from the RFID and IR cloner library <sub>**RFIDIRCloner.cpp** and **RFIDIRCloner.h**</sub>

# Code Logic 

All files are thoroughly commented so each script's core components are explained





