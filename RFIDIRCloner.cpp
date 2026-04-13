
#include <Arduino.h>
#include <IRremote.hpp>
#include "RFIDIRCloner.h"

decode_type_t IR_Received_Protocol;

// Static member definitions
unsigned long RFIDIRCloner::_firstMillis = 0;
bool RFIDIRCloner::_longPressDetected = false;

RFIDIRCloner::RFIDIRCloner(byte ss_pin, byte rst_pin, byte ir_receive_pin) {
  _ss_pin         = ss_pin;
  _rst_pin        = rst_pin;
  _ir_receive_pin = ir_receive_pin;

  IR_Received_Value    = 0;
  IR_Received_Command  = 0;
  IR_Repeats           = 3;
  UID_Size             = 0;
}

void RFIDIRCloner::begin() {
  // RFID SETUP
  SPI.begin();
  _mfrc522 = new MFRC522(_ss_pin, _rst_pin);
  _mfrc522->PCD_Init();

  // IR SETUP
  IrReceiver.begin(_ir_receive_pin, ENABLE_LED_FEEDBACK);
  Serial.println(F("IR Receiver Ready"));
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

  if (_mfrc522->MIFARE_SetUid(UID, (byte)UID_Size, true)) {
    Serial.println(F("Wrote new UID to card."));
    delay(2000);
    return true;
  }

  delay(2000);
  return false;
}

// ── IR ────────────────────────────────────────────────────────────────────────

bool RFIDIRCloner::readIR() {
  if (IrReceiver.decode()) {
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

    IrReceiver.resume(); 
    return true;
  }
  return false;
}

void RFIDIRCloner::sendIR() {
  switch (IR_Received_Protocol) {

    case NEC:
      IrSender.sendNEC(IR_Received_Value, IR_Received_Command, IR_Repeats);
      break;

    case SONY:
      IrSender.sendSony(IR_Received_Value, IR_Received_Command, IR_Repeats, SIRCS_12_PROTOCOL);
      break;

    case RC5:
      IrSender.sendRC5(IR_Received_Value, IR_Received_Command, IR_Repeats);
      break;

    case RC6:
      IrSender.sendRC6(IR_Received_Value, IR_Received_Command, IR_Repeats);
      break;

    case SAMSUNG:
      IrSender.sendSamsung(IR_Received_Value, IR_Received_Command, IR_Repeats);
      break;

    case LG:
      IrSender.sendLG(IR_Received_Value, IR_Received_Command, IR_Repeats);
      break;

    case PANASONIC:
      IrSender.sendPanasonic(IR_Received_Value, IR_Received_Command, IR_Repeats);
      break;

    case DENON:
      IrSender.sendDenon(IR_Received_Value, IR_Received_Command, IR_Repeats);
      break;

    case SHARP:
      IrSender.sendSharp(IR_Received_Value, IR_Received_Command, IR_Repeats);
      break;

    default:
      Serial.print(F("Unknown protocol: "));
      Serial.println(IR_Received_Protocol);
      break;
  }
  delay(2000);
}
