#include <NilRTOS.h>

const uint8_t sinal = A5; // Sinal analógico do sensor de umidade está settado no pino A0 do Arduino
const uint8_t led = 13; // Led está no pino 13
const uint8_t relay = A0;  // Sinal analógico do relé

SEMAPHORE_DECL(sem, 0);

NIL_WORKING_AREA(waThread1, 128);

NIL_THREAD(Thread1, arg) {  
  while (TRUE) {
    // Espera sinal da thread 2.
    nilSemWait(&sem);

    digitalWrite(led, LOW);
    digitalWrite(relay, LOW);
  }
}

NIL_WORKING_AREA(waThread2, 128);

// Declare the thread function for thread 2.
NIL_THREAD(Thread2, arg) {
  while (TRUE) {
    pinMode(sinal, INPUT);
    pinMode(relay, OUTPUT);
    pinMode(led, OUTPUT);
  
    int valorSinal;
    int umidade;
    boolean liga;
  
    valorSinal = analogRead(sinal);
    liga = verificaValorUmidade(valorSinal);
    if (!liga) {
      digitalWrite(led, HIGH);
      digitalWrite(relay, HIGH);
    } else {
      nilSemSignal(&sem);
    }
    //printaResultados(valorSinal, porcentagem);
    nilThdSleepMilliseconds(200);
  }
}

NIL_THREADS_TABLE_BEGIN()
NIL_THREADS_TABLE_ENTRY("thread1", Thread1, NULL, waThread1, sizeof(waThread1))
NIL_THREADS_TABLE_ENTRY("thread2", Thread2, NULL, waThread2, sizeof(waThread2))
NIL_THREADS_TABLE_END()

void setup()
{
  // Start Nil RTOS.
  Serial.begin(9600);
  nilSysBegin();
}
 
void loop(){
}

boolean verificaValorUmidade(int valor) {
  // Converte o valor do sinal do sensor de umidade para porcentagem (0% - 100%)
  int porcentagem = map(valor, 1023, 465, 0, 100);
  if (porcentagem >= 50) {
    Serial.println("\nStatus: Solo úmido");
    Serial.print(porcentagem);
    Serial.print("%");
    return true;
  }
  if (porcentagem >= 0 && porcentagem < 50){
    Serial.println("\nStatus: Solo seco");
    Serial.print(porcentagem);
    Serial.print("%");
    return false;
  }
}

// Mostra os resultados no log
void printaResultados(int valorSinal, int porcentagem){
  Serial.print("\n\nValor analógico: ");
  Serial.print(valorSinal);
  Serial.print("\nPorcentagem: "+ porcentagem);
  Serial.print(porcentagem);
  Serial.print("%");
}

