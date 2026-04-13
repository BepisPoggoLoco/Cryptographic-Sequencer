# Basis

Named "Cryptographic Sequencer" as a reference to Batman's hacking device in the Arkham games. 

In actuality, this is an arduino based IR and RFID signal reader/cloner.

![Tuff Batman pic](https://teamliquid.com/staff/shiroiusagi/tlpro/0207_g2a/batrmanscreen03.jpg)

# Core Components

1. Arduino of any kind (I used an uno) <sub>(x1)</sub>

2. RC522 RFID reader/writer module **(x1)**

3. IR receiver module **(x1)**

4. IR LED **(x1)**

5. Pushbuttons **(x1)**

6. 0.96 inch I2C OLED **(x1)**

*I sourced components 1,2,3, and 5 from my ELEGOO UNO R3 Super Starter Kit and the rest from AliExpress*

# Schematic

# Code Structure 

The main file (*Crytographic_Sequencer.ino*) contains the OLED GUI code and draws functions from the RFID and IR cloner library (*RFIDIRCloner.cpp and RFIDIRCloner.h*)

# Code Logic 

All files are thoroughly commented so each script's core components are explained





