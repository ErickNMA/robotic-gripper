// --- Mapeamento de Hardware ---
#define motor1 5     // Controle IN1 da ponte H L298N
#define motor2 6     // Controle IN1 da ponte H L298N
#define SD1 9       // Sinais Digitais
#define SD2 10
// --- Configurações Iniciais ---
void setup() {
  pinMode(motor1, OUTPUT);
  pinMode(motor2, OUTPUT);
  pinMode(SD1, INPUT);
  pinMode(SD2, INPUT);
}
// --- Loop Infinito ---
void loop() {
  
  bool s1 = digitalRead(SD1); // Leitura das entradas digitais
  bool s2 = digitalRead(SD2);
  
  if((s1 && s2) || (!s1 && !s2)){ // Parar Garra
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);
  }
  if(s1 && !s2){ // Fechar  Garra
    analogWrite(motor1, 220);
    digitalWrite(motor2, LOW);
  }
  if(s1! && s2){ // Abrir  Garra
    digitalWrite(motor1, LOW);
    analogWrite(motor2, 220);
  }
}
