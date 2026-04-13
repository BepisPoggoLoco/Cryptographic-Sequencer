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

Receive

- Await the presence of an IR signal; Decode and store signal's raw data, command, and protocol in 3 corresponding variables<sub> IR_Received_Value, IR_Received_Command, IR_Received_Protocol</sub>
  
Send

- Echo back received signal given the information stored in our 3 variables
  
Storage

- View metadata about stored IR signal 

## RFID Submenu

Receive

- Receive signal and store in local variable
  
Send

- Send IR signal stored in storage
  
Storage

- View currently stored IR signal 

## Electronic Structure 

# Code Logic 

All files are thoroughly commented with explanations of each script's core components 





