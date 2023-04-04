// Comunicação utilizando apenas entradas e saídas digitais
bool sinal = true;
//====================================================================
bool aguardarComando(){
  int SD1 = 9;                            // Variáveis locais
  int verificador = 0;
  unsigned long tempo_anterior = 0;
  
// Aguarda a mudança de estado de SD1 com um tempo limite de 1 segundo
  while(digitalRead(SD1) == sinal){ 
    // Tempo em ms desde o início da execução do programa
    unsigned long tempo_atual = millis();
    
    // Verifica a cada 100ms se um bit foi recebido
    if (tempo_atual - tempo_anterior >= 100){
      verificador++;
      if(verificador == 10) return false; // Cancela a operação
      tempo_anterior = tempo_atual;
    }
  }
  if (sinal == false)     sinal = true;
  else if (sinal == true) sinal = false;
  return true;                            // Bit recebido
}
//====================================================================
// Recebe, converte e armazena uma nova referência
float referencia (){
  int SD2 = 10;                           // Variáveis locais
  float ref = 0.0;

// Leitura sequencial de 8 bits
// Referência binária de 7 bits e um bit de verificação (0)

// Os bits são recebidos em ordem do bit menos significativo
// para o mais significativo
  for(int i=0; i<7; i++){

      // Se um bit for recebido
      if(aguardarComando()) {
        // Caso seu valor seja 1
        if (digitalRead(SD2)){
          // Adiciona ao resultado da conversão a potência de base 2
          // referente à ordem do bit atual
          ref = ref + pow(2, (i));
        }
      }
      else return 0;
      }  
      
// Confere se bit de verificação foi recebido
  if(aguardarComando()){
    if (!digitalRead(SD2)){
      return ceil(ref);
    }
    else return 0;
  }
  // Cancela a operação
  else return 0;
}
