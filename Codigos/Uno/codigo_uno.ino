#include <LiquidCrystal.h> //biblioteca LCD
#include <string.h>
#include <EEPROM.h>

#define botao_vermelho 6 //botão de pular
#define botao_verde 10 //botão de descer
#define botao_reset 9 //botão de reset

unsigned long tempo; //tempo geral
unsigned long t_a_add = 0; //tempo anterior da adição de um novo obstáculo
unsigned long t_a_dino = 0; //tempo anterior do dino
unsigned long t_a_cenario = 0; //tempo anterior do cenário
long int score = 0; //Score do jogo iniciado
int pos_score;  //Variável utilizada para ajustar a posição do score no LCD
int x_cursor;   //Posição x do score
int best_score = EEPROM.read(0);  //EEPROM é a memória do arduino, que guarda o melhor score ja feito antes
int buzzer = 8; //Pino do buzzer

unsigned long tempo_dino = 250; //tempo de repetição do dino
unsigned long tempo_cenario = 500;//tempo de repetição do cenario
unsigned long tempo_add = 0; //tempo atual de criação do novo obstáculo
unsigned long tempo_pulo = 0; //tempo que o dinossauro fica no ar

int reset = 0; //quando vira 1 reseta
int botao_dino_voador = 0; //flag para resolver o bug do botão

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

byte core[8] = {
  0b00000,
  0b01010,
  0b11111,
  0b11111,
  0b11111,
  0b01110,
  0b00100,
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
  lcd.createChar(4, cactus);
  lcd.createChar(5, pite_0);
  lcd.createChar(6, pite_1);
  lcd.createChar(7, core);

  //inicializa a serial
  Serial.begin(9600);

  randomSeed(analogRead(0)); //possibilita o real random

  pinMode(botao_vermelho, INPUT);
  pinMode(botao_verde, INPUT);
  pinMode(botao_reset, INPUT);
  pinMode(buzzer,OUTPUT);   

  //declaração das posições e imagens iniciais do dino
  dino.pos_x = 0;
  dino.pos_y = 1;
  dino.desenho = 0;

  //Menu inicial do jogo, para que ele não comece automaticamente
  while(digitalRead(botao_reset) == LOW){
    tempo = millis();
    lcd.setCursor(1, 0);
    lcd.print("Best Score: ");
    lcd.print(best_score);
    lcd.setCursor(3,1);
    lcd.print("PRESS START");
    lcd.setCursor(0,1);
    lcd.write(byte(1));
    lcd.setCursor(15,1);
    lcd.write(byte(4));
 }
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
    if(cenario[i].tipo == 1){ // se o tipo for pite
      if(cenario[i].desenho >= 6){     //reseta a imagem do pite
        cenario[i].desenho = 4; 
      }
      cenario[i].desenho++;
    }
    cenario[i].pos_x--; //todos os obstáculos andam um para esquerda na LCD
    i++;
  }
    delay(1);
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

void pulo(){ //função para o dino pular
  lcd.setCursor(0,1); //seta o cursor para posição atual do dino (em baixo)
  lcd.print(" "); //apaga o dino da posição anterior (em baixo)
  dino.pos_y = 0; //altera a posição do dino
}

void descida(){ //função para o dino descer
  lcd.setCursor(0,0); //seta o cursor para a posição atual do dino(
  lcd.print(" "); //apaga o dino da posição anterior (em cima)
  dino.pos_y = 1; //altera a posição do dino
}

void colisao(){ 
  if(cenario[0].pos_x == dino.pos_x && cenario[0].pos_y == dino.pos_y &&cenario[0].desenho != 0){ //se a posição do dino coincidir com a posição inicial do vetor cenario 
    //cenario[0].desenho != 0 - para evitar o bug que acontece quando o desenho não muda a tempo e é igualado a zero

    //Som feito quando há uma colisão
    tone(buzzer, 110); //A - lá          
    delay(200);
    noTone(buzzer);

    //If utilizado para calcular qual o best score, atualizando-o
    if(score > best_score){
      best_score = score;
      EEPROM.write(0, best_score);
    }
    reset = 0; //mudamos a flag do reset
    lcd.clear(); //limpamos a telaa
    lcd.setCursor(1,0);
    lcd.print("PERDEU, OTARIO!"); //printamos a mensagem de "perdeu"
    lcd.setCursor(4,1);
    lcd.print("BS: ");  //Printa o best score
    lcd.print(best_score); 
    
    while(reset != 1){ //enquanto a flag do botão não virar 1 fica num lopping infinito com a mensagem de perda
      if(digitalRead(botao_reset)==HIGH){ //se o botão for apertado
        reset = 1; //flag do botão é 1;
        for(int i = 0; i< tam_vet; i++){ //anulamos todos as posições do vetor (tipo = -1)
          cenario[i].tipo = -1;
        }
        tam_vet = 0; //zeramos o tamanho do vetor
        tempo_cenario = 500; //para o jogo recomeçar na mesma velocidade
        score = 0;
      }
    }
  }
}

//Função que printa o score na tela
void print_score(){
  pos_score = score;
  x_cursor = 15;
  while(pos_score > 9){ //Loop utilizado para ajustar o score sempre no canto direito, para que ele nao suma da tela
    x_cursor--;
    pos_score = pos_score/10;
  }
  lcd.setCursor(x_cursor,0);
  lcd.print(score);
}

void loop(){

  tempo = millis(); //tempo atual de execução
  //Score é aumentado a cada 0,5 segundo
  if(tempo%500 <= 0){
      score++;
      print_score();
    }

  //a flag dino_voador é uma flag que permite que o botão de subir fique pressionado e mesma assim o dino caia
  if(digitalRead(botao_vermelho) == HIGH && botao_dino_voador == 0){ //se o botão vermelho por pressionado
    //Som do pulo do dinossauro
    tone(buzzer, 2637); //E7 - mi            
    delay(200);
    noTone(buzzer);
    pulo(); //chama a função pulo
    botao_dino_voador = 100; //colamos ela em 100 para que dê um tempo bom para o dino cair antes dele subir novamente se o botão ficar pressionado
    tempo_pulo = tempo; //tempo do pulo é alterado
  }

  if((dino.pos_y==0 || botao_dino_voador != 0) && tempo-tempo_pulo>=1000){ //se o dino ta em cima ou se a flag dino voador não dor zero (que é quando ele pode pular novamente) e passa-se 1 segundo
    descida(); //chama a função de descida
    botao_dino_voador--; //decrescemos a flag
  }
  
  if(digitalRead(botao_verde)== HIGH){ //se o botão verde for pressionado
    descida(); //chama a função descida
    botao_dino_voador = 0; 
  }

  if((tempo-t_a_add)>=tempo_add && tam_vet < 15){ //random para criar um novo obstáculo
    novo_obstaculo();
    t_a_add = tempo;
    if(tempo_cenario>100){
      tempo_add = random(2000, 3500); //tempo entre 2 e 3.5 segundos
    }else{
      tempo_add = random(1000, 2000);
    }
  }
  
  if((tempo-t_a_dino)>=tempo_dino){ //se o (tempo atual - o tempo anterior do dino) for maior ou igual ao tempo estipulado de repetição do dino
    t_a_dino = tempo; // o tempo anterior é atualizado

    if(dino.desenho>2){    //reseta a imagem do dino
      dino.desenho = 0;
    }else{
      dino.desenho++;       //modifica a imagem do dino
    }
    
    print_tela(); //imprime cenario
    print_dino(); //imprime dino
    print_score();
   
  }


  if((tempo-t_a_cenario)>tempo_cenario){ //se o (tempo atual - o tempo anterior do cenário) for maior ou igual ao tempo estipulado de repetição do cenário
    
    atualizar(); //atualiza o elementos do cenario (pites e cactos)

    lcd.clear(); //limpa tela

    print_tela(); //imprime cenario
    print_dino(); //imprime dino
    print_score();
  
    t_a_cenario = tempo; //atualiza o tempo anterior do cenário

    colisao();
  }

  if (tempo%1000<1){ //a cada um segundo a velocidade do cenário aumenta
      if(tempo_cenario>80){ //enquanto o tempo de repetição for maior que 80 
        tempo_cenario-=5; //diminuimos 5 do tempo de repetição (aumentamos assim a velocidade)
      }
  }
  delay(1); //arduino respirar (e não dar erros)
}
