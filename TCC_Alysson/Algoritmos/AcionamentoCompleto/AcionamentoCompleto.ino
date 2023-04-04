#include "atuadores_sensores.h" // Inclusão das bibliotecas
#include "comunicacao.h"
#define SD1 9       // Sinais digitais
#define SD2 10
#define SD3 11
//====================================================================
void setup() { // --- Configurações Iniciais ---
  pinMode(motor1, OUTPUT);  // Configura as entradas e saídas digitais
  pinMode(motor2, OUTPUT);
  pinMode(encoder_C1, INPUT);
  pinMode(encoder_C2, INPUT);
  pinMode(SD1, INPUT);            
  pinMode(SD2, INPUT);
  pinMode(SD3, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(encoder_C1), contar_pulsos, CHANGE); 
  // Interrupção por mudança de estado, acompanhando o canal A do encoder
  digitalWrite(SD3, LOW); // Condição inicial
  redefinirPosicao();     // Move os dedos para a origem
}
//====================================================================
void loop() { // --- Loop Infinito ---
  if(digitalRead(SD1) == HIGH){
    digitalWrite(SD3, HIGH); // Sinaliza que uma tarefa está em curso
    bool comando;            // Declaração de variáveis locais
    int ref = 0;
    // Se o primeiro bit for 0, controle de força
    if(!digitalRead(SD2)) comando = LOW; 
    // Se o primeiro bit for 1, controle de posição
    else comando = HIGH; 
    
    if(aguardarComando()){ // Aguarda o envio do próximo bit por 2s
      if(!digitalRead(SD2)){   // Se o segundo bit for 0
        if(!comando) ref = 25; // Referência de força padrão
        if(comando)  ref = 70; // Referência de abertura padrão
      }
      else{
        ref = referencia(); // Ler nova referência
      }
      
      if(!comando) fecharGarra(ref); // Executa a função identificada
      if(comando)  abrirGarra(ref);  // com base na referência definida
    }
    sinal = true;           // Redefine a condição inicial
    digitalWrite(SD3, LOW); // Sinaliza a conclusão da tarefa
  }
}
