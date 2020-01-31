#include <LiquidCrystal.h> //biblioteca LCD
#include <string.h>

unsigned long tempo; //tempo geral
unsigned long t_a_add = 0; //tempo anterior da adição de um novo obstáculo
unsigned long t_a_dino = 0; //tempo anterior do dino
unsigned long t_a_cenario = 0; //tempo anterior do cenário

unsigned long tempo_dino = 250; //tempo de repetição do dino
unsigned long tempo_cenario = 500;//tempo de repetição do cenario
unsigned long tempo_add = 0; //tempo atual de criação do novo obstáculo

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

byte cactus[8] = { //desenho do cactus
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

int tam_vet = 0; //guarda a quantidade de elementos no cenário(pite e cacto)

typedef struct personagem{ 
  int pos_x; //guarda a posição em x
  int pos_y; //guarda a posição em y
  int desenho; //guarda qual imagem é (usado para movimentação)
  int tipo = -1; // tipo = 0 -> cacto
                 // tipo = 1 -> pite
                 // tipo = -1 -> nulo
};

//criação do personagem dino
personagem dino;

//cria um vetor que guardará os elementos do cenário (pite e cacto)
personagem cenario[15];

void setup() {
  // inicializa o LCD com o numero de linhas e de colunas
  lcd.begin(16, 2);
  
  //associação do desenho a determinado número
  lcd.createChar(0, Dino_andando_0);  
  lcd.createChar(1, Dino_andando_1);
  lcd.createChar(2, Dino_andando_2);
  lcd.createChar(3, Dino_andando_1);
  lcd.createChar(4, cactus);
  lcd.createChar(5, pite_0);
  lcd.createChar(6, pite_1);

  //inicializa a serial
  Serial.begin(9600);

  randomSeed(analogRead(0)); //possibilita o real random

  //declaração das posições e imagens iniciais do dino
  dino.pos_x = 0;
  dino.pos_y = 1;
  dino.desenho = 0;

  //Printa o cenário
  print_tela();
}

//função que imprime os desenhos na tela
void print_tela(){
    for(int i = 0; i<tam_vet; i++){//percorre todo vetor de elementos e os printa
      lcd.setCursor(cenario[i].pos_x, cenario[i].pos_y);
      lcd.write(byte(cenario[i].desenho)); 
    }

}

void print_dino(){
  //desenha dino
  lcd.setCursor(dino.pos_x,dino.pos_y);
  lcd.write(byte(dino.desenho));
}

void atualizar(){ //atualiza a posição e o desenho dos obstáculos
  
  int i;
  
  if(tam_vet == 0){ //se o vetor for nulo
    return; //não há o que atualizar
  }

  if(cenario[0].pos_x == 0){ //se o obstáculo estiver na posição zero
    for(i = 0; i<tam_vet; i++){
     cenario[i] = cenario[i+1]; //todos os elementos do vetor dão um shift para a esquerda
    }
    cenario[i].tipo = -1; //o ultimo elemento tem tipo -1 porque ele é nulo
    tam_vet--; //reduzimos o tamanho pois um elemento foi retirado do vetor
  }

  i = 0;
  
  while(i<tam_vet){ //atualiza a posição e o desenho dos obstáculos
    Serial.println(i);
    if(cenario[i].tipo == 1){ // se o tipo for pite
      if(cenario[i].desenho == 6){     //reseta a imagem do pite
        cenario[i].desenho = 4; 
      }
      cenario[i].desenho++;
    }
    cenario[i].pos_x--; //todos os obstáculos andam um para esquerda na LCD
    i++;
  }
}

//Essa função tem o objetivo de criar um elemento para o cenário
void novo_obstaculo(){
  
  int escolha = random(2); //Escolhe se será um cacto(0) ou um pite(1)
  
  //seta os valores padrões de cada elemento
  cenario[tam_vet].tipo = escolha;
  cenario[tam_vet].pos_x = 15;
  
  if(escolha == 0){//se for um cacto
    cenario[tam_vet].desenho = 4;
    cenario[tam_vet].pos_y = 1;
  }else{           //se for um pite
    cenario[tam_vet].desenho = 5;
    cenario[tam_vet].pos_y = random(2);
  }
  tam_vet++; //incrementa um na variável referente a quantidade de elementos no cenário
}

void loop() {

  tempo = millis(); //tempo atual de execução

  if((tempo-t_a_add)>=tempo_add){ //random para criar um novo obstáculo
    novo_obstaculo();
    t_a_add = tempo;
    tempo_add = random(2000, 3500); //tempo entre 2 e 3.5 segundos
  }
  
  if((tempo-t_a_dino)>=tempo_dino){ //se o (tempo atual - o tempo anterior do dino) for maior ou igual ao tempo estipulado de repetição do dino
    t_a_dino = tempo; // o tempo anterior é atualizado

    if(dino.desenho>2){    //reseta a imagem do dino
      dino.desenho = 0;
    }else{
      dino.desenho++;       //modifica a imagem do dino
    }

    print_dino(); //imprime dino
    print_tela(); //imprime cenario
  }


  if((tempo-t_a_cenario)>tempo_cenario){ //se o (tempo atual - o tempo anterior do cenário) for maior ou igual ao tempo estipulado de repetição do cenário
    
    atualizar(); //atualiza o elementos do cenario (pites e cactos)

    lcd.clear(); //limpa tela
    print_tela(); //imprime tela
    print_dino(); //imprime cenario

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
