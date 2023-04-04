// --- Mapeamento de Hardware ---
#define sensor A7    // Saída do sensor de corrente ACS712
#define motor1 5     // Controle IN1 da ponte H L298N
#define motor2 6     // Controle IN2 da ponte H L298N
#define encoder_C1 2 // Canal A do encoder
#define encoder_C2 3 // Canal B do encoder
//====================================================================
// --- Parâmetros identificados empíricamente ---
// Sinal mínimo necessário para o movimento do motor partindo da inércia
#define zona_morta 180 
// Sinal referente à velocidade mínima obtenível, com o motor em movimento
#define PWM_minimo 140  
// Número de pulsos do encoder por revolução do eixo do motor
#define pulsos_revolucao 1030.0 
// Deslocamento linear dos dedos para cada rotação completa do motor
// Corresponde a 2x o passo do fuso, em milímetros
#define passo_por_volta 16.0 
//====================================================================
// --- Variáveis Globais ---

// Movimento & Força
// Normalização dos sinais para escala de 0 a 100
float coeficiente_velocidade = (255 - zona_morta) / 100.0,
      coeficiente_forca = (255 - PWM_minimo) / 100.0,
      coeficiente_posicao = pulsos_revolucao / passo_por_volta;
// Registros de posição e erro de posição
int abertura_atual = 0;
long int erro_posicao = 0;

// Encoder
volatile long contador_posicao = 0; // Registra os pulsos detectados
byte estado_anterior_C1;
boolean sentido = false;

// ACS712 - Sensor de corrente
float valorSensor = 0.0,
      valorCorrente = 0.0,
      // Conversão AD de 10 bits: 5V/1023
      voltsporUnidade = 0.0048875855327468, 
      sensibilidade = 0.066, // Saída do sensor: 66 mV/A
      ruido = 0.26;          // Identificado empíricamente
//====================================================================
// --- Leitura do sensor de corrente ---
float lerSensor() {
  valorCorrente = 0;

  for (int i = 0; i < 1000; i++){
    // Leitura do sinal do sensor na entrada analogica A7
    valorSensor = analogRead(sensor);
    // A saída do sensor é VCC/2 para corrente = 0A
    // Ajuste da escala de tensão para de 0 a 5V
    valorSensor = (valorSensor - 510);
    // Cálculo da corrente com base na resolução do sensor
    valorCorrente += (valorSensor * voltsporUnidade) / sensibilidade;
  }
  // Valor médio de 1000 leituras
  valorCorrente = (valorCorrente / 1000) - ruido;
  return valorCorrente;
}
//====================================================================
// --- Função de acionamento do motor e controle de velocidade ---
void acionarMotor(double sinal_controle) {

  // Converte a escala de 0 a 100 para de 0 a 255 com base na entrada
  int PWM = abs(sinal_controle) * coeficiente_velocidade + zona_morta;
  if (PWM >= 255) PWM = 255; // Saturação do sinal de entrada

  // Tratamento da zona morta por aproximação linear simples
  if (PWM == zona_morta) {
    digitalWrite(motor1, LOW);
    digitalWrite(motor2, LOW);
  }
  if (sinal_controle > 0) {   // Sentido de rotação positivo
    analogWrite(motor1, PWM);
    digitalWrite(motor2, LOW);
  }
  if (sinal_controle < 0) {   // Sentido de rotação negativo
    digitalWrite(motor1, LOW);
    analogWrite(motor2, PWM);
  }
}
// --- Função para desligar o motor ---
void pararGarra() {
  acionarMotor(0);
}
//====================================================================
// ----- ALGORITMO DE CONTROLE DE POSIÇÃO EM MALHA ABERTA ----- v2.0
void abrirGarra(int abertura = 0) {
  // Tratamento de excessão. A abertura máxima é de 100 mm
  if (abertura > 0 && abertura <= 100){ 
  
    contador_posicao = 0;
    unsigned long tempo_anterior = 0; // Variáveis locais
    int teste_travamento = 0;
  // Converte a escala de 0 a 100 para de 0 a 255 com base na entrada    
  int referencia_pulsos=(abertura-abertura_atual)*coeficiente_posicao;
    // Atualiza a referência com base no erro de posição registrado
    if (referencia_pulsos >= 0) referencia_pulsos -= erro_posicao;
    if (referencia_pulsos < 0)  referencia_pulsos += erro_posicao;
    int velocidade = 100;  // Velocidade inicial
    
    while (abs(contador_posicao) < abs(referencia_pulsos)){

      // Quando a contagem alcançar 80% da referência
      if (abs(contador_posicao) > abs(referencia_pulsos * 0.8)){
        // Reduz a velocidade, de forma inversamente proporcional
        // ao avanço na contagem dos pulsos
        if (velocidade > 20){
          velocidade = (referencia_pulsos-contador_posicao);
          velocidade = 100*abs(referencia_pulsos/velocidade);
        } 
        else velocidade = 20; // Satura a velocidade mínima em 20%
      } 
      // Aciona o motor no sentido de rotação adequado
      if (referencia_pulsos >= 0) acionarMotor(-velocidade);
      if (referencia_pulsos < 0)  acionarMotor(velocidade);
      // Tempo em ms desde o início da execução do programa
      unsigned long tempo_atual = millis();

      // A cada 20ms, verifica se ocorreu o travamento do motor
      if (tempo_atual - tempo_anterior >= 20){
        // Excluindo a primeira iteração
        if (tempo_anterior > 0){
          // Se a velocidade do motor se manteve em 0 durante 20ms 
          if (teste_travamento == contador_posicao){
            pararGarra();
            break;
          } else teste_travamento = contador_posicao;
        }
        tempo_anterior = tempo_atual;
      }
    }
    pararGarra();
    abertura_atual = abertura;  // Atualiza o registro da abertura
    delay(100);
    // Registra o erro de posição, com base nos pulsos contados
    // após a instrução de parada do motor
    erro_posicao = abs(abs(contador_posicao) - abs(referencia_pulsos));
  }
}
//====================================================================
//- ALGORITMO DE CONTROLE DE FORÇA DE PREENSÃO EM MALHA ABERTA - v2.0
void fecharGarra(int forca = 0){
   // Tratamento de excessão. A força varia entre 0 e 100%
   if (forca > 0 && forca <= 100){

    contador_posicao = 0;
    unsigned long tempo_anterior = 0; // Variáveis locais
    int teste_travamento = 0;
    bool travamento = false;
  // Converte a escala de 0 a 100 para de 0 a 255 com base na entrada
    int PWM_final = forca * coeficiente_forca + PWM_minimo;
    int PWM = 255;

    acionarMotor(100); // Fecha a garra na potência máxima por 500 ms
    delay(500);        // de forma a vencer a inércia
        
    while (travamento == false){ // Loop até o travamento do motor
      // Tempo em ms desde o início da execução do programa
      unsigned long tempo_atual = millis(); 
      
      // A cada 50ms, reduz a velocidade em aproximadamente 5% até
      // saturar na referência final
      if (tempo_atual - tempo_anterior >= 50){
        if (PWM < PWM_final) PWM = PWM_final;
        else PWM -= 10;
        analogWrite(motor1, PWM);
      }
      // A cada 20ms, verifica se ocorreu o travamento do motor
      if (tempo_atual - tempo_anterior >= 20){
        // Excluindo a primeira iteração
        if (tempo_anterior > 0){
          // Se a velocidade do motor se manteve em 0 durante 20ms 
          if (teste_travamento == contador_posicao){
            pararGarra();
            travamento = true;
          } else teste_travamento = contador_posicao;
        }
        tempo_anterior = tempo_atual;
      }
    }
    // Atualiza o registro da abertura com base na contagem de pulsos
    abertura_atual -= abs(contador_posicao) / coeficiente_posicao; 
    if (abertura_atual < 0) abertura_atual = 0;
  }
}
//====================================================================
// Desloca os dedos até a origem e redefine os contadores
void redefinirPosicao() {
  fecharGarra(40);
  contador_posicao = 0;
  abertura_atual = 0;
}
//====================================================================
// --- Função para contagem de pulsos do encoder por interrupção ---
void contar_pulsos() {
  // Lê o estado de encoder_C1 e armazena em estado_C1
  int estado_C1 = digitalRead(encoder_C1); 
  // Identifica uma borda de subida
  if (!estado_anterior_C1 && estado_C1){
    // Lê o estado de encoder_C2 e armazena em estado_C2
    int estado_C2 = digitalRead(encoder_C2);
    // Identifica o sentido de rotação do motor
    if (!estado_C2 && sentido)      sentido = false; // Sentido reverso
    else if (estado_C2 && !sentido) sentido = true;  // Sentido direto
  }
  // Armazena o valor do último estado de encoder_C1
  estado_anterior_C1 = estado_C1; 
  // Soma ou subtrai o contador com base no sentido de rotação
  if (!sentido) contador_posicao++;
  else          contador_posicao--;
}
