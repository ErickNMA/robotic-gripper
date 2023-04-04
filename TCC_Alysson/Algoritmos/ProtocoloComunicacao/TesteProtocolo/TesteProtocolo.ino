#define controle 7 // SD1
#define valor 8    // SD2
bool sinal = false; // Variável global
void setup() {                    // Configurações Iniciais
  pinMode(controle, OUTPUT);      // Configura as saídas digitais
  pinMode(valor, OUTPUT);
  Serial.begin(9600);             // Inicia a comunicação serial
  digitalWrite(valor, LOW);       // Condições iniciais
  digitalWrite(controle, LOW);
}
void enviarComando(){        //  Muda o estado de SD1 a cada execução
  if (sinal == true){
    digitalWrite(controle,LOW);          // Aplica nível baixo em SD1
    sinal = false;
  }
  else {
    digitalWrite(controle,HIGH);         // Aplica nível alto em SD1
    sinal = true;
  }
  delay(25);                   // Intervalo entre o envio de cada bit
}
void enviarSinal(bool comando, int referencia = 0){
// Transmite uma sequência de 2 ou 10 bits representando uma instrução
  // Identificar o comando: Abrir garra (1) ou Fechar garra (0)?
  if(comando == false) digitalWrite(valor, LOW);
  else  digitalWrite(valor, HIGH);
  enviarComando();             // Envia o primeiro bit
  
// Utilizar referência predefinida? Sim (0) ou Não, definir nova (1)?
  if(referencia == 0) digitalWrite(valor, LOW);
  else digitalWrite(valor, HIGH);
  enviarComando();             // Envia o segundo bit

  // Defininir uma nova referência 
  if(referencia > 0 && referencia <= 100){
    //byte referencia_bits = referencia;
    for(int i=0; i<7; i++){         // Conversão: inteiro -> binário
// Verifica se o bit na posição i da variável "referencia" vale 0
// e representa o resultado em SD2
      if (bitRead(referencia, i) == 0) digitalWrite(valor, LOW);
      else digitalWrite(valor, HIGH);
      enviarComando();              // Envia os bits de 3 a 9
    }    
    digitalWrite(valor, LOW);       // Bit de verificação (0)
    enviarComando();                // Envia o bit de verificação (10)
  }
  digitalWrite(valor, LOW);         // Redefine as condições iniciais
  digitalWrite(controle, LOW);
  sinal = false;
}
void loop() {                       // Exemplo de Programa Principal
    char c = Serial.read();       // Lê um caractere pelo canal serial
    if (c == 'a') enviarSinal(HIGH, Serial.parseInt());
    if (c == 'f') enviarSinal(LOW, Serial.parseInt());
// Exemplos: ao enviar no monitor serial
// -> a50 -> Abre/Move a garra até 50 mm de abertura
// -> f25 -> Fecha a garra com 25% da força máxima
}
