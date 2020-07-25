/* ======================================================================================================
   
   Usina Info & WR Kits
   
   Medindo RPM de um Motor DC com Arduino


   Compilador: Arduino IDE 1.8.4

   Autor: Eng. Wagner Rambo  Data: Março de 2018
   
   www.wrkits.com.br | facebook.com/wrkits | youtube.com/canalwrkits


   HARDWARE:
 

   ARDUINO    ENCODER

   GND          GND
   5V            5V
   D2           OUT

   
====================================================================================================== */

 
// ========================================================================================================
// --- Variáveis Globais --- 
int           rpm;
volatile byte pulsos;
unsigned long timeold;

//Altere o numero abaixo de acordo com o seu disco encoder
unsigned int pulsos_por_volta = 20;


// ========================================================================================================
// --- Interrupção ---
void contador()
{
  //Incrementa contador
  pulsos++;
}


// ========================================================================================================
// --- Configurações Iniciais ---
void setup()
{
  Serial.begin(115200);

  pinMode(2, INPUT);
   
  
  //Interrupcao 0 - pino digital 2
  //Aciona o contador a cada pulso
  attachInterrupt(0, contador, RISING);
  pulsos  = 0;
  rpm     = 0;
  timeold = 0;
  
} //end setup


// ========================================================================================================
// --- Loop Infinito ---
void loop()
{
  //Atualiza contador a cada segundo
  if (millis() - timeold >= 1000)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(0);
    rpm = (60 * 1000 / pulsos_por_volta ) / (millis() - timeold) * pulsos;
    timeold = millis();
    pulsos = 0;

    //Mostra o valor de RPM no serial monitor
    Serial.print("RPM = ");
    Serial.println(rpm, DEC);
    //Habilita interrupcao
    attachInterrupt(0, contador, RISING);
  }
  
} //end loop



