#include "atuadores_sensores.h" // Inclusão das bibliotecas
#define SD1 9       // Recebe comandos
#define SD2 12      // Sinaliza tarefa em curso

#define timeout 500      //tempo de espera de um bit

//====================================================================

int getCommand()
{
  if(digitalRead(SD1))
  {
    delay(100);
    while(digitalRead(SD1))
    {

    }
    unsigned long t0 = millis();
    while((millis()-t0) < timeout)
    {
      if(digitalRead(SD1))
      {
        return 2;
      }
    }
    return 1;
  }else
  {
    return 0;
  }
}

//====================================================================

void setup() 
{ // --- Configurações Iniciais ---
  Serial.begin(115200);
  pinMode(motor1, OUTPUT);  // Configura as entradas e saídas digitais
  pinMode(motor2, OUTPUT);
  pinMode(encoder_C1, INPUT);
  pinMode(encoder_C2, INPUT);
  pinMode(SD1, INPUT);            
  pinMode(SD2, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(encoder_C1), contar_pulsos, CHANGE); 
  // Interrupção por mudança de estado, acompanhando o canal A do encoder
  digitalWrite(SD2, HIGH);
  Serial.println("! GOING HOME !");
  redefinirPosicao();     // Move os dedos para a origem
  digitalWrite(SD2, LOW);
  Serial.println("* PRONTO *");
}

//====================================================================

void loop() 
{ // --- Loop Infinito ---
  switch(getCommand())
  {
    case 1:
      digitalWrite(SD2, HIGH);
      Serial.println("! ABRINDO !");
      abrirGarra(70);
      digitalWrite(SD2, LOW);
      Serial.println("* PRONTO *");
    break;
    case 2:
      digitalWrite(SD2, HIGH);
      Serial.println("! FECHANDO !");
      fecharGarra(40);
      digitalWrite(SD2, LOW);
      Serial.println("* PRONTO *");
    break;
  }
}