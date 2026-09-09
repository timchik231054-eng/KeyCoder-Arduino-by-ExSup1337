#include <Wire.h>
#include <Adafruit_GFX.h> // библиотеки для работы с дисплеем
#include <Adafruit_SSD1306.h>

#include <EEPROM.h> // сохранение в EEPROM(после перезапуска)

#include <SPI.h>  // библиотеки для работы с RC522
#include <MFRC522.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

#define SS_PIN 8
#define RST_PIN 9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1); // инициализируем дисплей

MFRC522 mfrc522(SS_PIN, RST_PIN); // инициализируем RC522

struct keyData {
  char name[12];
  byte code[8];
};


keyData keyList[10] = {
  {"Empty 1",   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {"Empty 2",   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {"Empty 3",   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {"Empty 4",   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {"Empty 5",   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {"Empty 6",   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {"Empty 7",   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {"Empty 8",   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {"Empty 9",   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {"Empty 10",  {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}
};

byte selectedKey;
int smoothVoltage = 0;

enum Mode {
  mode1,
  mode2,
  mode3
};

Mode nowmode = mode1;

void Mode1();
void Mode2();
void Mode3();

void podMode1();
void podMode2();
void podMode3();

void header();
void footer();

void consoleKeyRename();

void setup() {

  Serial.begin(9600); 

  selectedKey = EEPROM.read(0);
  if (selectedKey >= 10 || selectedKey < 0) selectedKey = 0; // читаем данные из EEPROM последний выбранный ключ

  if (EEPROM.read(10) == 0xA5) { 
      EEPROM.get(11, keyList);
  }

  smoothVoltage = analogRead(A0);
  
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setTextColor(SSD1306_WHITE);

  SPI.begin();
  mfrc522.PCD_Init();

  delay(100);

Serial.println(F("RC522 test"));

byte version = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);

Serial.print(F("RC522 Version: 0x"));
Serial.println(version, HEX);

if (version == 0x00 || version == 0xFF) {
  Serial.println(F("ERROR: RC522 not detected!"));
} else {
  Serial.println(F("RC522 detected!"));
}
  
  display.clearDisplay();
  display.display();
  
  pinMode(2, INPUT_PULLUP); 
  pinMode(3, INPUT_PULLUP); 
  pinMode(4, OUTPUT);
  pinMode(A0, INPUT);

  display.setCursor(13, 19);
  display.setTextSize(2);
  display.print(F("Welcome"));
  display.setCursor(36, 39);
  display.setTextSize(2);
  display.print(F("\\(^-^)/"));
  display.display();
  delay(1000);
  display.clearDisplay();

  display.setTextSize(1);
  display.setCursor(28, 19);
  display.print(F("Made by"));
  display.setCursor(43, 32);
  display.print(F("ExSup1337"));
  display.display();
  delay(1200);
  display.clearDisplay();
}

void loop() {
 
  consoleKeyRename();

  if (digitalRead(2) == LOW) {
    digitalWrite(4, 1);
    delay(200);
    digitalWrite(4, 0);
    if (nowmode == mode1) nowmode = mode2;
    else if (nowmode == mode2) nowmode = mode3;
    else if (nowmode == mode3) nowmode = mode1;
    
    delay(200);
  }

  if (digitalRead(3) == LOW) {
    digitalWrite(4, 1);
    delay(200);
    digitalWrite(4, 0);
    display.clearDisplay();
    
    switch (nowmode) {
      case mode1: 
        podMode1(); 
        break;
        
      case mode2: 
        podMode2(); 
        break;
        
      case mode3: 
        podMode3();
        break;
    }
  }

  display.clearDisplay();

  switch (nowmode) {
    case mode1: Mode1(); break;
    case mode2: Mode2(); break;
    case mode3: Mode3(); break;
  }

  display.display();
}

void Mode1() {
  header();
  footer();
  display.setCursor(30, 24);
  display.setTextSize(2);
  display.print(F("Read"));
}

void Mode2() {
  header();
  footer();
  display.setCursor(30, 24);
  display.setTextSize(2);
  display.print(F("Rewrite"));
}

void Mode3() {
  header();
  footer();
  display.setCursor(30, 24);
  display.setTextSize(2);
  display.print(F("List"));
}

void podMode1() {
  display.clearDisplay();
  header();
  footer();

  display.setCursor(15, 24);
  display.setTextSize(2);
  display.print(F("You sure?"));
  display.setCursor(46, 40);
  display.setTextSize(1);
  display.print(F("Yes/No"));
  display.display();

 while(true){
  if(digitalRead(3) == LOW){
    break;
  }
  if(digitalRead(2) == LOW){
    return;
  }
  delay(200);
 }
  display.clearDisplay();
  display.setCursor(15, 20);
  display.setTextSize(1);
  display.print(F("Scan card..."));
  display.display();

  bool cardRead = false;
  unsigned long start = millis();

  while (millis() - start < 4000) {
    // Инициализируем карту перед каждой проверкой
    if (mfrc522.PICC_IsNewCardPresent() == 1) {
      if (mfrc522.PICC_ReadCardSerial() == 1) {
        cardRead = true;
        break;
      }
    }
    delay(50); // Увеличена задержка, чтобы дать шине I2C (дисплею) «подышать»
  }

  display.clearDisplay();
  header();
  footer();
  display.setCursor(20, 24);
  display.setTextSize(2);

  if (cardRead == true) {
    for (byte i = 0; i < 4; i++) {
      keyList[selectedKey].code[i] = mfrc522.uid.uidByte[i];
    }
    sprintf(keyList[selectedKey].name, "Key %d", selectedKey + 1);

    EEPROM.write(10, 0xA5);
    EEPROM.put(11, keyList);

    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

    display.print(F("Saved!"));
  } else {
    display.print(F("Timeout!"));
  }

  display.display();
  delay(1200);
}



void podMode2() {
  display.clearDisplay();
  header();
  footer();

  display.setCursor(8, 18);
  display.setTextSize(1);
  display.print(F("Rewrite: "));
  display.print(keyList[selectedKey].name);

  display.setCursor(8, 32);
  display.print(F("Scan new card..."));

  display.display();

  bool cardRead = false;
  unsigned long start = millis();

  // Ждём карту 4 секунды
  while (millis() - start < 4000) {

    if (mfrc522.PICC_IsNewCardPresent()) {
      if (mfrc522.PICC_ReadCardSerial()) {
        cardRead = true;
        break;
      }
    }

    delay(50);
  }

  display.clearDisplay();
  header();
  footer();

  if (cardRead) {

    // Перезаписываем UID выбранного ключа
    for (byte i = 0; i < 4; i++) {
      keyList[selectedKey].code[i] = mfrc522.uid.uidByte[i];
    }

    
    EEPROM.write(10, 0xA5); // сохранение данные в EEPROM
    EEPROM.put(11, keyList);

    
    mfrc522.PICC_HaltA(); // завершение работу с картой
    mfrc522.PCD_StopCrypto1();

    display.setCursor(20, 24);
    display.setTextSize(2);
    display.print(F("Done!"));

  } else {

    display.setCursor(20, 24);
    display.setTextSize(2);
    display.print(F("Timeout!"));
  }

  display.display();
  delay(1200);
}

void podMode3() {
  byte keyIndex = 0; 

  while(true) {
    display.clearDisplay();
    header();
    footer();

    display.setCursor(10, 18);// вывод названия ключа 
    display.setTextSize(2);
    display.print(keyList[keyIndex].name);


    display.setCursor(10, 38);// вывод ID  кода
    display.setTextSize(1);
    display.print(F("ID:"));
  for(byte i = 0; i < 4; i++){ 
   if(keyList[keyIndex].code[i] < 0x10) display.print(F("0"));
   display.print(keyList[keyIndex].code[i], HEX);
   if(i < 3) display.print(F(":"));
  }

    display.display();


    if (digitalRead(2) == LOW) {// переключение ключа D2
      digitalWrite(4, 1);
      delay(200);
      digitalWrite(4, 0);

      keyIndex++;
      if (keyIndex >= 10) keyIndex = 0; 
      
      delay(200); 
    }

    if (digitalRead(3) == LOW) { //выбор ключа D3
      digitalWrite(4, 1);
      delay(500);
      digitalWrite(4, 0);
    
      selectedKey = keyIndex; 
      EEPROM.update(0, selectedKey);

      display.clearDisplay();
      display.setCursor(10, 24);
      display.setTextSize(2);
      display.print(F("Selected!"));
      display.display();
      delay(1000);
      display.clearDisplay();
      break;
    }
  }
}
void consoleKeyRename() {
  if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == F("/list")) {
      Serial.println(F("--- KEY LIST ---"));
      for (byte i = 0; i < 10; i++) {
        Serial.print(i);
        Serial.print(F(": "));
        Serial.print(keyList[i].name);
        Serial.print(F(" | ID: "));
        for (byte j = 0; j < 4; j++) {
          if (keyList[i].code[j] < 0x10) Serial.print(F("0"));
          Serial.print(keyList[i].code[j], HEX);
          if (j < 3) Serial.print(F(":"));
        }
        Serial.println();
      }
      Serial.println(F("----------------"));
    }
    else if (input.startsWith("/rename=")) {
      int commaIndex = input.indexOf(',');
      if (commaIndex > 4) {
        byte index = input.substring(4, commaIndex).toInt();
        String newName = input.substring(commaIndex + 1);

        if (index < 10 && newName.length() > 0) {
          newName.toCharArray(keyList[index].name, sizeof(keyList[index].name));
          
          EEPROM.write(10, 0xA5);
          EEPROM.put(11, keyList);

          Serial.print(F("Key "));
          Serial.print(index);
          Serial.print(F(" renamed to: "));
          Serial.println(keyList[index].name);
        } else {
          Serial.println(F("Error: Index out of range (0-9)"));
        }
      }
    }
  }
}

void header() {
  display.setTextSize(1);
  display.setCursor(53, 0);
  if(nowmode == mode1) display.print(F("\\(^w^)\\"));
  else if(nowmode == mode2) display.print(F("\\(^o^)/"));
  else if(nowmode == mode3) display.print(F("/(o_o)\\"));
  
  display.setCursor(0, 0);
  display.print(F("KeyCoder         v1.0"));
  display.drawFastHLine(0, 10, 128, SSD1306_WHITE);
}

void footer() {
  display.setTextSize(1);
  display.setCursor(0, 56);
  display.print(F("Key:"));

  display.print(keyList[selectedKey].name);
  display.drawFastHLine(0, 54, 128, SSD1306_WHITE);

  int rawVoltage = analogRead(A0);
  smoothVoltage = (smoothVoltage * 9 + rawVoltage) / 10;
  
  byte percent = map(smoothVoltage, 0, 1023, 0, 100); // при подключении делалей заменить на map(rawVoltage, 614, 860, 0, 100));
  percent = constrain(percent, 0, 100);

  byte xPosition = 100;
  if (percent < 100) xPosition = 106;
  if (percent < 10)  xPosition = 112;

  display.setCursor(xPosition, 56);
  display.print(percent);
  display.print(F("%"));
}