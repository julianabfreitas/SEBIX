#include <LiquidCrystal.h> //biblioteca LCD

#define cactus 4 

unsigned long tempo; //tempo geral
unsigned long t_a_dino = 0; //tempo anterior do dino
unsigned long t_a_cenario = 0; //tempo anterior do cenário

unsigned long tempo_dino = 250; //tempo de repetição do dino
unsigned long tempo_cenario = 500;//tempo de repetição do cenario


int pos_cactus = 15; //posição do cacto (começa no quinze porque ele surge no fim do cenário)

int pos_dino = 0; //posição do dino (começa no zero porque ele fica no início do cenário)
int dino = 0; //variável que armazena qual desenho do dino estamos usando (usado para dar movimento ao personagem)

int pos_pite = 8; //posição do pite (bicho que voa)
int pit = 5; //variável que armazena qual desenho do pite estamos usando (usado para dar movimento ao personagem)

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2; //explica as ligações do lc no arduino
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

byte Dino_andando_0[8] = { //desenho do dinossauro com a patinha direita no chão
  0b00111,
  0b00111,
  0b00111,
  0b10110,
  0b10111,
  0b01110,
  0b01110,
  0b00010
};

byte Dino_andando_1[8] = { //desenho do dinossauro com duas patinhas no chão
  0b00111,
  0b00111,
  0b00111,
  0b10110,
  0b10111,
  0b01110,
  0b01110,
  0b01010
};

byte Dino_andando_2[8] = { //desenho do dinossauro com patinha esquerda no chão 
  0b00111,
  0b00111,
  0b00111,
  0b10110,
  0b10111,
  0b01110,
  0b01110,
  0b01000
};

byte cacto[8] = { //desenho do cactus
  0b00101,
  0b10101,
  0b10101,
  0b10111,
  0b11100,
  0b00100,
  0b00100,
  0b00100
};

byte pite_0[8]{ //desenho do pite com asas abertas
  0b00000,
  0b00100,
  0b01100,
  0b11110,
  0b00111,
  0b00110,
  0b00100,
  0b00000
};

byte pite_1[8]{ //desenho do pite com asas fechas
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
  // inicializa o LCD com o numero de linhas e de colunas
  lcd.begin(16, 2);

  //associação do desenho a determinado número
  lcd.createChar(0, Dino_andando_0);  
  lcd.createChar(1, Dino_andando_1);
  lcd.createChar(2, Dino_andando_2);
  lcd.createChar(3, Dino_andando_1);
  lcd.createChar(4, cacto);
  lcd.createChar(5, pite_0);
  lcd.createChar(6, pite_1);

  //inicializa a serial
  Serial.begin(9600);
  
  //chama a função
  print_tela();

}

//função que imprime os desenhos na tela

void print_tela(){

    //desenha dino
    lcd.setCursor(0,1);
    lcd.write(byte(dino));
    
    //desenha cactus
    lcd.setCursor(pos_cactus, 1);
    lcd.write(byte(cactus));

    //desenha pite  
    lcd.setCursor(pos_pite, 0);
    lcd.write(byte(pit));
  
  
  }

void loop() {

  tempo = millis(); //tempo atual de execução
  
  if((tempo-t_a_dino)>=tempo_dino){ //se o (tempo atual - o tempo anterior do dino) for maior ou igual ao tempo estipulado de repetição do dino
    t_a_dino = tempo; // o tempo anterior é atualizado

    if(dino > 2){    //reseta a imagem do dino
      dino = 0;
    }else{
      dino++;       //modifica a imagem do dino
    }
    
    lcd.clear(); //limpa tela
    print_tela(); //chama a função de imprimir desenhos
    
  }


  if((tempo-t_a_cenario)>tempo_cenario){ //se o (tempo atual - o tempo anterior do cenário) for maior ou igual ao tempo estipulado de repetição do cenário
    
    if(pos_cactus<1){ //reseta a posição do cactus
      pos_cactus = 16; //colocamos 16 porque em seguida decretaremos a posição (se colocássemos 15 ele nunca chegaria no 15
    }
  
    if(pos_pite<1){  //reseta a posição do pite
      pos_pite = 16; //colocamos 16 porque em seguida decretaremos a posição (se colocássemos 15 ele nunca chegaria no 15
    }
  
    if(pit == 6){     //reseta a imagem do pite
      pit = 4;
    }
      
    pos_cactus--; //decrementa a posição do cactus
    pos_pite--;   //decrementa a posição do pite
  
    pit++;        //modifica a imagem do pite

    lcd.clear(); //limpa tela
    print_tela(); //imprime tela

    t_a_cenario = tempo; //atualiza o tempo anterior do cenário

    }

  if (tempo%1000<1){ //a cada um segundo a velocidade do cenário aumenta
      if(tempo_cenario>80){ //enquanto o tempo de repetição for maior que 80 
        Serial.println(tempo_cenario); 
        Serial.println(tempo);
        tempo_cenario-=5; //diminuimos 5 do tempo de repetição (aumentamos assim a velocidade)
      }
  }
  delay(1); //arduino respirar (e não dar erros)
}
