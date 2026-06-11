
#include <Arduino.h>
#include <IRremote.hpp>
#include "RFIDIRCloner.h"

decode_type_t IR_Received_Protocol;

// Static member definitions
unsigned long RFIDIRCloner::_firstMillis = 0;
bool RFIDIRCloner::_longPressDetected = false;

RFIDIRCloner::RFIDIRCloner(byte ss_pin, byte rst_pin, byte ir_receive_pin, byte ir_send_pin) {
  _ss_pin         = ss_pin;
  _rst_pin        = rst_pin;
  _ir_receive_pin = ir_receive_pin;
  _ir_send_pin = ir_send_pin;

  IR_Received_Value    = 0;
  IR_Received_Command  = 0;
  IR_Repeats           = 1;
  UID_Size             = 0;
}
 
void RFIDIRCloner::begin() {
  // RFID SETUP
  SPI.begin();
  _mfrc522 = new MFRC522(_ss_pin, _rst_pin);
  _mfrc522->PCD_Init();

  // IR SETUP
  IrReceiver.begin(_ir_receive_pin, ENABLE_LED_FEEDBACK);
  IrSender.begin(_ir_send_pin);
  Serial.println(F("IR Receiver and Sender Ready"));
}

// ── RFID ──────────────────────────────────────────────────────────────────────

bool RFIDIRCloner::readUID() {
  if (!_mfrc522->PICC_IsNewCardPresent()) {
    return false;
  }
  if (!_mfrc522->PICC_ReadCardSerial()) {
    return false;
  }

  UID_Size = _mfrc522->uid.size;
  for (int i = 0; i < UID_Size; i++) {
    UID[i] = _mfrc522->uid.uidByte[i];
  }

  for (int i = 0; i < UID_Size; i++) {
    Serial.print(UID[i]);
    Serial.print(F(" "));
  }
  Serial.println(F(""));

  return true;
}

bool RFIDIRCloner::changeUID() {
  if (!_mfrc522->PICC_IsNewCardPresent() || !_mfrc522->PICC_ReadCardSerial()) {
    delay(50);
    return false;
  }

  Serial.print(F("Card UID:"));
  for (byte i = 0; i < _mfrc522->uid.size; i++) {
    Serial.print(_mfrc522->uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(_mfrc522->uid.uidByte[i], HEX);
  }
  Serial.println();

  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;

  MFRC522::StatusCode status = _mfrc522->PCD_Authenticate(
    MFRC522::PICC_CMD_MF_AUTH_KEY_A, 0, &key, &(_mfrc522->uid)
  );

  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Auth failed: "));
    Serial.println(_mfrc522->GetStatusCodeName(status));
    _mfrc522->PICC_HaltA();
    _mfrc522->PCD_StopCrypto1();
    delay(2000);
    return false;
  }

  byte block0[16] = {
    UID[0], UID[1], UID[2], UID[3],          
    UID[0]^UID[1]^UID[2]^UID[3],             
    0x08,                                      
    0x04, 0x00,                              
    0x00, 0x00, 0x00, 0x00,                   
    0x00, 0x00, 0x00, 0x00
  };

  status = _mfrc522->MIFARE_Write(0, block0, 16);
  if (status == MFRC522::STATUS_OK) {
    Serial.println(F("Wrote new UID to card."));
    _mfrc522->PICC_HaltA();
    _mfrc522->PCD_StopCrypto1();
    delay(2000);
    return true;
  }

  Serial.print(F("Write failed: "));
  Serial.println(_mfrc522->GetStatusCodeName(status));
  _mfrc522->PICC_HaltA();
  _mfrc522->PCD_StopCrypto1();
  delay(2000);
  return false;
}

// ── IR ────────────────────────────────────────────────────────────────────────

bool RFIDIRCloner::readIR() {
  if (IrReceiver.decode()) {
    if (IrReceiver.decodedIRData.protocol != UNKNOWN) {
      IR_Received_Value    = IrReceiver.decodedIRData.decodedRawData;
      IR_Received_Command  = IrReceiver.decodedIRData.command;
      IR_Received_Protocol = IrReceiver.decodedIRData.protocol;

      Serial.println(IR_Received_Value, HEX);
      Serial.println(IR_Received_Command, HEX);
      Serial.println(getProtocolString(IR_Received_Protocol));

      // Long press detection
      if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
        if (!_longPressDetected && (millis() - _firstMillis > 1000)) {
          _longPressDetected = true;
          Serial.println(F("Long press detected"));
        }
      } else {
        _firstMillis       = millis();
        _longPressDetected = false;
      }
    }
    IrReceiver.resume(); 
    return true;
  }
  return false;
}

void RFIDIRCloner::sendIR() {
  switch (IR_Received_Protocol) {

    case NEC:
      IrSender.sendNEC(IR_Received_Value, IR_Received_Command, IR_Repeats); 
      Serial.println(F("Sent NEC IR signal"));
      break;

    case SONY:
      IrSender.sendSony(IR_Received_Value, IR_Received_Command, IR_Repeats, SIRCS_12_PROTOCOL);
      Serial.println(F("Sent SONY IR signal"));
      break;

    case RC5:
      IrSender.sendRC5(IR_Received_Value, IR_Received_Command, IR_Repeats);
      Serial.println(F("Sent RC5 IR signal"));
      break;

    case RC6:
      IrSender.sendRC6(IR_Received_Value, IR_Received_Command, IR_Repeats);
      Serial.println(F("Sent RC6 IR signal"));
      break;

    case SAMSUNG:
      IrSender.sendSamsung(IR_Received_Value, IR_Received_Command, IR_Repeats);
      Serial.println(F("Sent SAMSUNG IR signal"));
      break;

    case LG:
      IrSender.sendLG(IR_Received_Value, IR_Received_Command, IR_Repeats);
      Serial.println(F("Sent LG IR signal"));
      break;

    case PANASONIC:
      IrSender.sendPanasonic(IR_Received_Value, IR_Received_Command, IR_Repeats);
      Serial.println(F("Sent PANASONIC IR signal"));
      break;

    case DENON:
      IrSender.sendDenon(IR_Received_Value, IR_Received_Command, IR_Repeats);
      Serial.println(F("Sent DENON IR signal"));
      break;

    case SHARP:
      IrSender.sendSharp(IR_Received_Value, IR_Received_Command, IR_Repeats);
      Serial.println(F("Sent SHARP IR signal"));
      break;

    default:
      Serial.print(F("Unknown protocol: "));
      Serial.println(IR_Received_Protocol);
      break;
  }
}

