#include <NilRTOS.h>

const uint8_t sinal = A0; // Sinal analógico do sensor de umidade está settado no pino A0 do Arduino
const uint8_t led = 13; // Led está no pino 13
const uint8_t sinalRele = A5;  // Sinal analógico do relé

SEMAPHORE_DECL(sem, 0);

NIL_WORKING_AREA(waThread1, 128);

NIL_THREAD(Thread1, arg) {
  while (TRUE) {
    // Espera sinal da thread 2.
    nilSemWait(&sem);

    digitalWrite(led, HIGH);
    digitalWrite(sinalRele, HIGH);
    nilThdSleepMilliseconds(2000);
  }
}

NIL_WORKING_AREA(waThread2, 128);

// Declare the thread function for thread 2.
NIL_THREAD(Thread2, arg) {

  pinMode(sinal, INPUT);
  pinMode(sinalRele, OUTPUT);
  pinMode(led, OUTPUT);

  int valorSinal;
  int umidade;
  int porcentagem;
  boolean liga;

  valorSinal = analogRead(sinal);
  porcentagem = converteParaPorcentagem(valorSinal);
  liga = verificaValorUmidade(porcentagem);
  
  acionaLedRele(liga);
  
  printaResultados(valorSinal, porcentagem);
  nilThdSleepMilliseconds(1000);
  
}

NIL_THREADS_TABLE_BEGIN()
NIL_THREADS_TABLE_ENTRY("thread1", Thread1, NULL, waThread1, sizeof(waThread1))
NIL_THREADS_TABLE_ENTRY("thread2", Thread2, NULL, waThread2, sizeof(waThread2))
NIL_THREADS_TABLE_END()

void setup()
{
  // Start Nil RTOS.
  nilSysBegin();
}
 
void loop(){
}

boolean verificaValorUmidade(int porcentagem) {
  if (porcentagem >= 50) {
    Serial.println("Status: Solo úmido");
    return true;
  }
  if (porcentagem > 0 && porcentagem < 50){
    Serial.println("Status: Solo seco");
    return false;
  }
  delay(1000);
}

void acionaLedRele(boolean valor){
  if (valor) {
    digitalWrite(led, LOW);
    digitalWrite(sinalRele, LOW);
  } else {
    nilSemSignal(&sem);
  }
}

// Converte o valor do sinal do sensor de umidade para porcentagem (0% - 100%)
int converteParaPorcentagem(int valor) {
  int porcentagem = 0;
  porcentagem = map(valor, 1023, 465, 0, 100);
  return porcentagem;
}

// Mostra os resultados no log
void printaResultados(int valorSinal, int porcentagem){
  Serial.print("\n\nAnalog Value: ");
  Serial.print(valorSinal);
  Serial.print("\nPercent: ");
  Serial.print(porcentagem);
  Serial.print("%");
}

