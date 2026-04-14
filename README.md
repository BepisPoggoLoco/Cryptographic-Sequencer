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

## Code Structure

1. The main file <sub>(**Crytographic_Sequencer.ino**)</sub> contains the OLED GUI code and calls functions from the RFID and IR cloner library <sub>(**RFIDIRCloner.cpp** and **RFIDIRCloner.h**)</sub>

## IR Submenu

**Receive**

- Await the presence of an IR signal; Decode and store signal's raw data, command, and protocol in 3 corresponding variables<sub> IR_Received_Value, IR_Received_Command, IR_Received_Protocol</sub>
  
**Send**

- Echo back received signal
  
**Storage**

- View metadata about stored IR signal 

## RFID Submenu

**Receive**

- Await the presence of an RFID device; Decode and store device's Unique Identifier (UID)
  
**Send**

- Await the presence of an RFID device and write the currently stored UID into presented device
  
**Storage**

- View currently stored UID

## Electronic Structure

I used the following diagrams to hookup all the components in the core components list
**PS: I hooked up all VCC/GND pins to desginated 5V/GND rails**

![RC522](https://peppe8o.com/wp-content/uploads/2022/04/RFID-connection-diagram.jpg)

![IR Receiver](https://www.circuitbasics.com/wp-content/uploads/2017/05/Arduino-IR-Remote-Receiver-Breakout-Board-Wiring-Diagram.png)

![Pushbuttons](pushbuttons.png)

![OLED](.jpg)

# Code Logic 

All files are thoroughly commented with explanations of each script's core components 





