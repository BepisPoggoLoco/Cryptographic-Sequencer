// LIBRARIES-----------------------------------------------------

//          GENERAL

#include <SPI.h>
#include <Wire.h>

//            CLONER

#include <MFRC522.h>
#include <IRremote.hpp>

//            OLED

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// PIN DEFINITIONS-----------------------------------

//                OLED

#define UP_BUTTON 5
#define DOWN_BUTTON 4
#define SELECT_BUTTON 3

//                CLONER

#define RST_PIN         9         
#define SS_PIN          10    
#define IR_RECEIVE_PIN 2

//OLED GUI CODEBASE----------------------------------------------

Adafruit_SSD1306 display(128, 64, &Wire);

enum MenuState {
  MAIN_MENU,
  IR_MENU,
  RFID_MENU,
  IR_SEND_1,
  IR_RECEIVE_1,
  RFID_SEND_1,
  RFID_RECEIVE_1,
  IR_STORAGE,
  RFID_STORAGE,
  STORAGE_INTERFACE,
  IR_STORE_1,
  RFID_STORE_1
};

//                              STORAGE VARIABLES

char IR_STORE_VAL_1[]   = "Sup";
char RFID_STORE_VAL_1[] = "";

MenuState currentState = MAIN_MENU;

int selectedIndex = 0;
unsigned long lastDebounce = 0;
const int debounceDelay = 150;

//                              MENU DRAWING

void drawMenu(const char* title, const char* options[], int optionCount) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.println(title);
  display.println("----------------");

  for (int i = 0; i < optionCount; i++) {
    if (i == selectedIndex) {
      display.print("> ");
    } else {
      display.print("  ");
    }
    display.println(options[i]);
  }

  display.display();
}

//                              BUTTON HANDLING

bool buttonPressed(int pin) {
  if (digitalRead(pin) == LOW && millis() - lastDebounce > debounceDelay) {
    lastDebounce = millis();
    return true;
  }
  return false;
}

//                                MENU VARIABLES

const char* MAIN_MENU_OPTIONS[]     = {"IR", "RFID"};
MenuState   MAIN_MENU_ELEMENTS[]    = {IR_MENU, RFID_MENU};

const char* RFID_STORAGE_OPTIONS[]  = {"Store 1", "Back"};
MenuState   RFID_STORAGE_ELEMENTS[] = {RFID_STORE_1, RFID_MENU};

const char* IR_STORAGE_OPTIONS[]    = {"Store 1", "Back"};
MenuState   IR_STORAGE_ELEMENTS[]   = {IR_STORE_1, IR_MENU};

//                                  UNIVERSAL MENU FUNCTION

void MENU_SCREEN(int optionCount, const char* menuOptions[], const char* menuTitle, MenuState menuElements[]) {
  drawMenu(menuTitle, menuOptions, optionCount);

  if (buttonPressed(UP_BUTTON)) {
    selectedIndex = (selectedIndex - 1 + optionCount) % optionCount;
  }
  if (buttonPressed(DOWN_BUTTON)) {
    selectedIndex = (selectedIndex + 1) % optionCount;
  }
  if (buttonPressed(SELECT_BUTTON)) {
    currentState = menuElements[selectedIndex];
    selectedIndex = 0;
  }
}

//                                RECEIVE / SEND SCREEN

void RECEIVE_OR_SEND_SCREEN(const char* text, MenuState backState) {
  display.clearDisplay();
  display.setCursor(0, 20);
  display.println(text);

  if (buttonPressed(SELECT_BUTTON)) {
    currentState = backState;
  }

  display.display();
}

//                                STORAGE SCREEN

void STORAGE_SCREEN(const char* title, char* storageValue, MenuState backState) {
  display.clearDisplay();
  display.setCursor(0, 0);

  display.println(title);
  display.println("----------------");
  display.println(storageValue);

  if (buttonPressed(SELECT_BUTTON)) {
    currentState = backState;
  }

  display.display();
}

// CLONER CODEBASE-------------------------------------------------------------------------------

byte UID;
String User_Input;

byte IR_Received_Value;
byte  IR_Received_Command;
decode_type_t IR_Received_Protocol;
byte IR_Repeats = 3;

MFRC522 mfrc522(SS_PIN, RST_PIN);

//                      CLONER FUNCTIONS
 
  
void READ_UID(){
  
    // End loop if no card present
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
      return;
    }

    // End loop is card read error
    if ( ! mfrc522.PICC_ReadCardSerial()) {
      return;
    }

    byte UID[mfrc522.uid.size];
    for (int i = 0; i < mfrc522.uid.size; i++){
      UID[i] = mfrc522.uid.uidByte[i];
    }
    
    for (int i = 0; i < sizeof(UID); i++){
      Serial.print(UID[i]);
      Serial.print(F(" "));
    }
    Serial.println(F(""));
    return;
}

void CHANGE_UID(){
  if ( ! mfrc522.PICC_IsNewCardPresent() || ! mfrc522.PICC_ReadCardSerial() ) {
    delay(50);
    return;
  }

  Serial.print(F("Card UID:"));
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  } 
  Serial.println();
  
  if ( mfrc522.MIFARE_SetUid(UID, (byte)sizeof(UID), true) ) {
    Serial.println(F("Wrote new UID to card."));
  }
  delay(2000);
}

void READ_IR(){
  if (IrReceiver.decode()) {

      IR_Received_Value = IrReceiver.decodedIRData.decodedRawData;
      IR_Received_Command = IrReceiver.decodedIRData.command;
      IR_Received_Protocol = IrReceiver.decodedIRData.protocol;

      Serial.println(IR_Received_Value, HEX);
      Serial.println(IR_Received_Command, HEX);
      Serial.println(getProtocolString(IR_Received_Protocol));
      
      // Long press detection
      static unsigned long firstMillis = 0;
      static bool longPressDetected = false;

      if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
          if (!longPressDetected && (millis() - firstMillis > 1000)) {
              longPressDetected = true;
              Serial.println(F("Long press detected"));
          }
      } else {
          firstMillis = millis();
          longPressDetected = false;
      }

      IrReceiver.resume(); // Important: enable next receive
  }
}
void SEND_IR(){  
  
  //REMOVED JVC and KASEIKYO PROTOCOLS
  
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

// SETUP-----------------------------------------------------

void setup() {
  
  // OLED SETUP 
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(SELECT_BUTTON, INPUT_PULLUP);

  display.clearDisplay();
  display.display();

  Serial.begin(9600);   // Initialize serial communications with the PC

  //RFID SETUP
  
  SPI.begin();      // Init SPI bus
  mfrc522.PCD_Init();   // Init MFRC522
 
  //IR SETUP
  
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
  //IrSender.begin();
  Serial.println(F("IR Receiver Ready"));
}



// LOOP--------------------------------------------------------

void loop() {

  switch (currentState) {

    case MAIN_MENU:
      MENU_SCREEN(2, MAIN_MENU_OPTIONS, "Main Menu", MAIN_MENU_ELEMENTS);
      break;

    // IR MENU

    case IR_MENU: {
      const char* options[] = {"Send 1", "Receive 1", "Storage", "Back"};
      MenuState elements[]  = {IR_SEND_1, IR_RECEIVE_1, IR_STORAGE, MAIN_MENU};
      MENU_SCREEN(4, options, "IR MENU", elements);
      break;
    }

    // RFID MENU

    case RFID_MENU: {
      const char* options[] = {"Send 1", "Receive 1", "Storage", "Back"};
      MenuState elements[]  = {RFID_SEND_1, RFID_RECEIVE_1, RFID_STORAGE, MAIN_MENU};
      MENU_SCREEN(4, options, "RFID MENU", elements);
      break;
    }

    // IR ACTIONS

    case IR_SEND_1:
      RECEIVE_OR_SEND_SCREEN("IR Sending (1)...", IR_MENU);
      break;

    case IR_RECEIVE_1:
      RECEIVE_OR_SEND_SCREEN("IR Receiving (1)...", IR_MENU);
      break;

    case IR_STORAGE:
      MENU_SCREEN(2, IR_STORAGE_OPTIONS, "IR Storage", IR_STORAGE_ELEMENTS);
      break;

    // RFID ACTIONS

    case RFID_SEND_1:
      RECEIVE_OR_SEND_SCREEN("RFID Sending (1)...", RFID_MENU);
      break;

    case RFID_RECEIVE_1:
      RECEIVE_OR_SEND_SCREEN("RFID Receiving (1)...", RFID_MENU);
      break;

    case RFID_STORAGE:
      MENU_SCREEN(2, RFID_STORAGE_OPTIONS, "RFID Storage", RFID_STORAGE_ELEMENTS);
      break;

    // STORAGE

    case IR_STORE_1:
      STORAGE_SCREEN("IR Store 1", IR_STORE_VAL_1, IR_STORAGE);
      break;

    case RFID_STORE_1:
      STORAGE_SCREEN("RFID Store 1", RFID_STORE_VAL_1, RFID_STORAGE);
      break;
  }
}
