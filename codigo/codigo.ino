#include <LiquidCrystal.h>

#define cactus 4

unsigned long tempo; //tempo 
unsigned long t_a_dino = 0;
unsigned long t_a_cenario = 0;

unsigned long tempo_dino = 250; //tempo de repetição do dino
unsigned long tempo_cenario = 500;//tempo de repetição do cenario


int pos_cactus = 15;

int pos_dino = 0;
int dino = 0; 

int pos_pite = 8;
int pit = 5;

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte Dino_andando_1[8] = {
  0b00111,
  0b00111,
  0b00111,
  0b10110,
  0b10111,
  0b01110,
  0b01110,
  0b01010
};

byte Dino_andando_0[8] = {
  0b00111,
  0b00111,
  0b00111,
  0b10110,
  0b10111,
  0b01110,
  0b01110,
  0b00010
};

byte Dino_andando_2[8] = {
  0b00111,
  0b00111,
  0b00111,
  0b10110,
  0b10111,
  0b01110,
  0b01110,
  0b01000
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

byte pite_0[8]{
  0b00000,
  0b00100,
  0b01100,
  0b11110,
  0b00111,
  0b00110,
  0b00100,
  0b00000
};

byte pite_1[8]{
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

  lcd.createChar(0, Dino_andando_0);
  lcd.createChar(1, Dino_andando_1);
  lcd.createChar(2, Dino_andando_2);
  lcd.createChar(3, Dino_andando_1);
  lcd.createChar(4, cacto);
  lcd.createChar(5, pite_0);
  lcd.createChar(6, pite_1);

  Serial.begin(9600);
  print_tela();

}


void print_tela(){

    //dino
    lcd.setCursor(0,1);
    lcd.write(byte(dino));
    
    //cactus
    lcd.setCursor(pos_cactus, 1);
    lcd.write(byte(cactus));

    //pite  
    lcd.setCursor(pos_pite, 0);
    lcd.write(byte(pit));
  
  
  }

void loop() {

  tempo = millis(); //tempo atual de execução
  
  if((tempo-t_a_dino)>=tempo_dino){    

    t_a_dino = tempo;

    if(dino > 2 ){    //reseta a imagem do dino
      dino = 0;
    }else{
      dino++;       //modifica a imagem do dino
    }
    lcd.clear();
    print_tela();

    }


  if(tempo-t_a_cenario>tempo_cenario){
    
    if(pos_cactus<1){ //reseta a posição do cactus
      pos_cactus = 16;
    }
  
    if(pos_pite<1){  //reseta a posição do pite
      pos_pite = 16;
    }
  
    if(pit == 6)     //reseta a imagem do pite
      pit = 4;
      
    pos_cactus--; //decrementa a posição do cactus
    pos_pite--;   //decrementa a posição do pite
  
    pit++;        //modifica a imagem do pite

    lcd.clear();
    print_tela();

    t_a_cenario = tempo;

    }

  if (tempo%1000<1){
      if (tempo_cenario>80){
        Serial.println(tempo_cenario);
        Serial.println(tempo);

        tempo_cenario-=10;
      }
  }
  delay(1);
}
