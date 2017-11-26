#include <NilRTOS.h>  // Inclusão da biblioteca NilRTOS

const uint8_t sinal = A5; // Sinal analógico do sensor de umidade está settado no pino A5 do Arduino
const uint8_t led = 13; // Led está no pino 13
const uint8_t relay = A0;  // Sinal analógico do relé

SEMAPHORE_DECL(sem, 0); // Declaração do semáforo para uso de threads

NIL_WORKING_AREA(waThread1, 128);  // Definição do workspace da thread 1

//  Thread 1: aguarda sinal da thread 2 e dispara sinal ao relé e ao led, ativando o sistema de irrigação
NIL_THREAD(Thread1, arg) {  
  while (TRUE) {
    // Espera sinal da thread 2.
    nilSemWait(&sem);

    digitalWrite(led, HIGH); // Liga o LED
    digitalWrite(relay, LOW); // Liga o Relé
    nilThdSleepMilliseconds(500);
  }
}

NIL_WORKING_AREA(waThread2, 128); // Workspace da thread 2

// Thread 2: envia sinal para thread 1 e lê os valores analógicos dos sensores e atuadores
NIL_THREAD(Thread2, arg) {
  while (TRUE) {
    pinMode(sinal, INPUT);  // Recupera a entrada do sensor de umidade
    pinMode(relay, OUTPUT);
    pinMode(led, OUTPUT);
  
    int valorSinal;
    int umidade;
    boolean liga;

    valorSinal = analogRead(sinal);  // lê sinal analógico e guarda na variável iniciada anterioremente
    liga = verificaValorUmidade(valorSinal);  // envia valor para a função que calcula e envia resposta de acordo com o valor do resultado
    if (liga) {
      digitalWrite(led, LOW);  // condição como true, o LED continua desligado assim como o relé
      digitalWrite(relay, HIGH);
    } else {
      nilSemSignal(&sem); // condição como false, envia sinal para tread 2, ativando o sistema de irrigação
    }
    nilThdSleepMilliseconds(1000);
  }
}

// Iniciação das threads e workspaces
NIL_THREADS_TABLE_BEGIN()
NIL_THREADS_TABLE_ENTRY("thread1", Thread1, NULL, waThread1, sizeof(waThread1))
NIL_THREADS_TABLE_ENTRY("thread2", Thread2, NULL, waThread2, sizeof(waThread2))
NIL_THREADS_TABLE_END()

void setup()
{
  // Setup necessário para as threads e biblioteca do nilrtos
  Serial.begin(9600);
  nilSysBegin();
}
 
void loop(){
}

boolean verificaValorUmidade(int valor) {
  // Converte o valor do sinal do sensor de umidade para porcentagem (0% - 100%)
  int porcentagem = map(valor, 1023, 0, 0, 100);
  if (porcentagem >= 50) {
    Serial.print("\nValor analógico: ");
    Serial.print(valor);
    Serial.println("\nStatus: Solo úmido");
    Serial.print(porcentagem);
    Serial.print("%");
    return true;
  }
  if (porcentagem >= 0 && porcentagem < 50){
    Serial.print("\nValor analógico: ");
    Serial.print(valor);
    Serial.println("\nStatus: Solo seco");
    Serial.print(porcentagem);
    Serial.print("%");
    return false;
  }
}