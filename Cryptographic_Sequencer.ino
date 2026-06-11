#include <SPI.h>
#include <Wire.h>
#include <U8g2lib.h>

#define UP_BUTTON 5
#define DOWN_BUTTON 4
#define SELECT_BUTTON 6

// U8g2 single page buffer mode — I2C SSD1306 128x64
U8G2_SSD1306_128X64_NONAME_1_HW_I2C display(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// CLONER DECLARATION

#include <RFIDIRCloner.h>

RFIDIRCloner cloner(10,5, 2, 3);

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

// STORAGE VARIABLES

String IR_STORE_VAL_1; 

String RFID_STORE_VAL_1;

MenuState currentState = MAIN_MENU;

int selectedIndex = 0;
unsigned long lastSend = 0;
const long SendInterval = 2000;
unsigned long lastDebounce = 0;
const int debounceDelay = 150;

// MENU DRAWING

void drawMenu(const char* title, const char* options[], int optionCount) {
  display.setFont(u8g2_font_5x8_tr);

  display.firstPage();
  do {
    display.setCursor(0, 8);
    display.print(title);
    display.setCursor(0, 16);
    display.print("----------------");

    for (int i = 0; i < optionCount; i++) {
      int y = 26 + i * 10;
      if (i == selectedIndex) {
        display.setCursor(0, y);
        display.print("> ");
      } else {
        display.setCursor(0, y);
        display.print("  ");
      }
      display.print(options[i]);
    }
  } while (display.nextPage());
}

// BUTTON HANDLING

bool buttonPressed(int pin) {
  if (digitalRead(pin) == LOW && millis() - lastDebounce > debounceDelay) {
    lastDebounce = millis();
    return true;
  }
  return false;
}

// SETUP

void setup() {
  Serial.begin(9600);
  cloner.begin();

  display.begin();

  pinMode(UP_BUTTON, INPUT_PULLUP);
  pinMode(DOWN_BUTTON, INPUT_PULLUP);
  pinMode(SELECT_BUTTON, INPUT_PULLUP);

  display.clearDisplay();

}

// MENU VARIABLES

const char* MAIN_MENU_OPTIONS[]     = {"IR", "RFID"};
MenuState   MAIN_MENU_ELEMENTS[]    = {IR_MENU, RFID_MENU};

const char* RFID_STORAGE_OPTIONS[]  = {"Store 1", "Back"};
MenuState   RFID_STORAGE_ELEMENTS[] = {RFID_STORE_1, RFID_MENU};

const char* IR_STORAGE_OPTIONS[]    = {"Store 1", "Back"};
MenuState   IR_STORAGE_ELEMENTS[]   = {IR_STORE_1, IR_MENU};

// UNIVERSAL MENU FUNCTION

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



// RECEIVE / SEND SCREEN

void RECEIVE_OR_SEND_SCREEN(const char* text, MenuState backState) {
  display.setFont(u8g2_font_5x8_tr);

  display.firstPage();
  do {
    display.setCursor(0, 28);
    display.print(text);
  } while (display.nextPage());

  if (buttonPressed(SELECT_BUTTON)) {
    currentState = backState;
  }
}

// STORAGE SCREEN

void STORAGE_SCREEN(const char* title, String storageValue, MenuState backState) {
  display.setFont(u8g2_font_5x8_tr);

  display.firstPage();
  do {
    display.setCursor(0, 8);
    display.print(title);
    display.setCursor(0, 16);
    display.print("----------------");
    display.setCursor(0, 28);
    display.print(storageValue);
  } while (display.nextPage());

  if (buttonPressed(SELECT_BUTTON)) {
    currentState = backState;
  }
}

// LOOP

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

    case IR_SEND_1: {
      RECEIVE_OR_SEND_SCREEN("IR SEND", IR_MENU);

      unsigned long currentTime = millis();
      
      if (currentTime - lastSend >= SendInterval){
        cloner.sendIR();
        lastSend = currentTime;
      }
      break;
    }

    case IR_RECEIVE_1:
      RECEIVE_OR_SEND_SCREEN("IR Receiving (1)...", IR_MENU);
      cloner.readIR();
      break;

    case IR_STORAGE:
      MENU_SCREEN(2, IR_STORAGE_OPTIONS, "IR Storage", IR_STORAGE_ELEMENTS);
      break;

    // RFID ACTIONS

    case RFID_SEND_1:
      RECEIVE_OR_SEND_SCREEN("RFID Sending (1)...", RFID_MENU);
      cloner.changeUID();
      break;

    case RFID_RECEIVE_1:
      RECEIVE_OR_SEND_SCREEN("RFID Receiving (1)...", RFID_MENU);
      cloner.readUID();
      break;

    case RFID_STORAGE:
      MENU_SCREEN(2, RFID_STORAGE_OPTIONS, "RFID Storage", RFID_STORAGE_ELEMENTS);
      break;

    // STORAGE

    case IR_STORE_1:
      IR_STORE_VAL_1 = String("V: ") + cloner.IR_Received_Value + String(" C: ") + cloner.IR_Received_Command;
      STORAGE_SCREEN("IR Store 1", IR_STORE_VAL_1, IR_STORAGE);
      break;

    case RFID_STORE_1:
      RFID_STORE_VAL_1 = cloner.UID[0];
      STORAGE_SCREEN("RFID Store 1", RFID_STORE_VAL_1, RFID_STORAGE);
      break;
  }
}
