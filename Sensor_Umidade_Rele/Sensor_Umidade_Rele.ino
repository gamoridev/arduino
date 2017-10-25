#define sinal A0 // Sinal analógico do sensor de umidade está settado no pino A0 do Arduino
#define led 13 // Led está no pino 13
#define sinalRele A5  // Sinal analógico do relé

int valorSinal;
int umidade;
int porcentagem;
 
void setup()
{
  Serial.begin(9600);
  pinMode(sinal, INPUT);
  pinMode(sinalRele, OUTPUT);
  pinMode(led, OUTPUT);
}
 
void loop()
{
  // Lê o valor do pino A0 do sensor
  // Leitura do sinal analógico
  valorSinal = analogRead(sinal);
  porcentagem = converteParaPorcentagem(valorSinal);
  printaResultados();
  delay(1000);
 
  // Sinal do valor do sensor.
  // Led e relé liga a partir da porcentagem
  if (porcentagem > 0 && porcentagem < 50){
    Serial.println("Status: Solo seco");
    digitalWrite(led, HIGH);
    digitalWrite(sinalRele, HIGH);
  }

  // Sinal do valor do sensor.
  // Led e relé desligam a partir da porcentagem
  if (porcentagem >= 50)
  {
    Serial.println("Status: Solo úmido");
    digitalWrite(led, LOW);
    digitalWrite(sinalRele, HIGH);
  }
  delay(100);
}

// Converte o valor do sinal do sensor de umidade para porcentagem (0% - 100%)
int converteParaPorcentagem(int valor)
{
  int porcentagem = 0;
  porcentagem = map(valor, 1023, 465, 0, 100);
  return porcentagem;
}

// Mostra os resultados no log
void printaResultados()
{
  Serial.print("\n\nAnalog Value: ");
  Serial.print(valorSinal);
  Serial.print("\nPercent: ");
  Serial.print(porcentagem);
  Serial.print("%");
}

