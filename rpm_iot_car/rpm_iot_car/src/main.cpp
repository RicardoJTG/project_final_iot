#include <Arduino.h>
#define pin_RPM 13
#define MCD1    33
#define MCD2    32

///Variables Globales///
int                 rpm           = 0;
volatile byte       pulsos        = 0;
unsigned long       tiempoF       = 0;
const unsigned int  pulsos_vuelta = 20; // Número de orificios en el encoder

//Prototipo de funciones//
void IRAM_ATTR isr(void); //Contador pulsos
void motor(boolean opc);

void setup() {
  Serial.begin(115200);
  pinMode(MCD1, OUTPUT);
  pinMode(MCD2, OUTPUT);
  pinMode(pin_RPM, INPUT);
  attachInterrupt(pin_RPM, isr, RISING);

  //Encendido motor
  motor(true);
}

void loop() {
  if (millis() - tiempoF >= 1000)
  {
    detachInterrupt(pin_RPM);
    rpm     = (60 * 1000 / pulsos_vuelta ) / (millis() - tiempoF) * pulsos;
    tiempoF = millis();
    pulsos  = 0;

    Serial.print("RPM = ");
    Serial.println(rpm, DEC);

    attachInterrupt(pin_RPM, isr, RISING);
  }
  
}

//Implementación de funciones//
void IRAM_ATTR isr(void) {
  pulsos++; //contador pulsos
}

void motor(boolean opc) {
  if (opc) {
    digitalWrite(MCD1, HIGH);
    digitalWrite(MCD2, LOW);
  } else {
    digitalWrite(MCD1, LOW);
    digitalWrite(MCD2, LOW);
  }
}