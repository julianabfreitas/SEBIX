#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte Dino[8] = {
  0b00111,
  0b00111,
  0b00111,
  0b10110,
  0b10111,
  0b01110,
  0b01110,
  0b01010
};

byte cacto[8] = {
  0b00101,
  0b10101,
  0b10101,
  0b10111,
  0b11100,
  0b00100,
  0b00100,
  0b00100
};

byte pite[8]{
  0b00000,
  0b00100,
  0b01100,
  0b11110,
  0b00111,
  0b00110,
  0b00100,
  0b00000
};

byte pite_asas [8]{
  0b00000,
  0b00100,
  0b01100,
  0b11111,
  0b00111,
  0b00000,
  0b00000,
  0b00000
};


void setup() {
  // initialize LCD and set up the number of columns and rows:
  lcd.begin(16, 2);

  lcd.createChar(0, Dino);
  lcd.createChar(1, cacto);
  lcd.createChar(2, pite);
  lcd.createChar(3, pite_asas);

  lcd.setCursor(0, 1);

  lcd.write(byte(0)); 
  lcd.setCursor(4, 1);
  lcd.write(byte(1)); 
}

void loop() {
  lcd.setCursor(8, 0);
  lcd.write(byte(2));
  delay(500); 
  lcd.setCursor(8, 0);
  lcd.write(byte(3));
  delay(500); 

}
