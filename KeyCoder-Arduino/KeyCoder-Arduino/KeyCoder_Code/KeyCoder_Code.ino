#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <EEPROM.h> // сохранение в EEPROM(после перезапуска)

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);


struct keyData {
  String name;
  byte code[8];
};


keyData keyList[10] = {
  {"8klass",    {0x01, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77}},
  {"9klass",    {0x01, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x00}},
  {"10klass",   {0x01, 0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE}},
  {"10klass+", {0x01, 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32}},
  {"Avtobus",    {0x01, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55, 0x55}},
  {"Uchitel",    {0x01, 0x99, 0x88, 0x77, 0x66, 0x55, 0x44, 0x33}},
  {"Empty 7",    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {"Empty 8",    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {"Empty 9",    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}},
  {"Empty 10",   {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}}
};

String Smiles[3] = {"\\(^w^)\\", "\\(^o^)/", "/(o_o)\\"}; //можно было и не создавать массив

int selectedKey;
int smoothVoltage = 0;
//int emptyCell = 0;

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

void setup() {
  selectedKey = EEPROM.read(0); // читаем данные из EEPROM последний выбранный ключ
  smoothVoltage = analogRead(A0);
  
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  display.setTextColor(SSD1306_WHITE);
  
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
        display.display();
        delay(2000); // это переделать так как бозвращение на экран будет по окончанию процедуры а не по истечению 2000 мс
        break;
        
      case mode2: 
        podMode2(); 
        display.display();
        delay(2000); // это переделать так как бозвращение на экран будет по окончанию процедуры а не по истечению 2000 мс
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
  header();
  footer();
  display.setCursor(15, 24);
  display.setTextSize(2);
  display.print(F("podMode1"));
}

void podMode2() {
  header();
  footer();

 // display.setCursor(15, 24);
 // display.setTextSize(2);
 // display.print(F("podMode2"));
}

void podMode3() {
  int keyIndex = 0; 

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
for(int i = 0; i <= 8; i++){
    display.print(keyList[keyIndex].code[i]);
}
  //  for(int i = 0; i < 4; i++) {
    //  if(keyList[keyIndex].code[i] < 0x10) display.print("0"); 
     // display.print(keyList[keyIndex].code[i], HEX);
   // }

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

void header() {
  display.setTextSize(1);
  display.setCursor(53, 0);
  if(nowmode == mode1) display.print(Smiles[0]);
  else if(nowmode == mode2) display.print(Smiles[1]);
  else if(nowmode == mode3) display.print(Smiles[2]);
  
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
  
  int percent = map(smoothVoltage, 0, 1023, 0, 100); // при подключении делалей заменить на map(rawVoltage, 614, 860, 0, 100));
  percent = constrain(percent, 0, 100);

  int xPosition = 100;
  if (percent < 100) xPosition = 106;
  if (percent < 10)  xPosition = 112;

  display.setCursor(xPosition, 56);
  display.print(percent);
  display.print(F("%"));
}