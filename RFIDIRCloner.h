#ifndef RFIDIRCloner_h
#define RFIDIRCloner_h

#include <SPI.h>
#include <MFRC522.h>


class RFIDIRCloner {
  public:
    // Constructor
    RFIDIRCloner(byte ss_pin, byte rst_pin, byte ir_receive_pin);

    // Setup
    void begin();

    // RFID
    bool readUID();
    bool changeUID();

    // IR
    bool readIR();
    void sendIR();

    // Public state (mirrors original sketch globals)
    byte UID[10];        // holds last read UID bytes (up to 10 bytes)
    byte UID_Size;       // number of bytes in last read UID

    byte IR_Received_Value;
    byte IR_Received_Command;
    byte IR_Repeats;

  private:
    byte _ss_pin;
    byte _rst_pin;
    byte _ir_receive_pin;

    MFRC522 *_mfrc522;

    static unsigned long _firstMillis;
    static bool _longPressDetected;
};

#endif
