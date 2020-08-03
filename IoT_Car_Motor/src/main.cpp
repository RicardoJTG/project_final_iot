#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

//Asignación de pines y tamaños de buffer
#define MA1                 14
#define MA2                 27
#define MB1                 26
#define MB2                 25
#define MCD1                33
#define MCD2                32
#define MSG_BUFFER_SIZE	(50)
#define MSG_BUFFER_AIRE	(10)
#define pin_RPM             13
#define pin_Caire           34

//Parámetros de la red wifi local, dirección y puerto del servidor mqtt remoto
const char* ssid        = "RED_JHL_FLIA_TRUJILLO";
const char* password    = "Trullo@*2746#";
const char* mqtt_server = "54.204.32.210"; //IP elástica
const int   mqttPort    = 1883;

//Configuración librría wifi y PubSubClient
WiFiClient espClient;
PubSubClient client(espClient);

//Variables globales
unsigned long       lastMsg       = 0;
char                msg[MSG_BUFFER_SIZE];
char                msg_a[MSG_BUFFER_AIRE];
int                 rpm           = 0;
volatile byte       pulsos        = 0;
const unsigned int  pulsos_vuelta = 20; // Número de orificios en el encoder
int                 c_aire        = 0;
volatile boolean    act           = false;

//Prototipos de funciones
void setup_wifi(void);
void callback(char* topic, byte* payload, unsigned int length);
void reconnect(void);
void Stop(void);
void IRAM_ATTR isr(void); //Contador pulsos

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, mqttPort);
  client.setCallback(callback);  
  //Configuración pines como I/O
  pinMode(pin_RPM, INPUT);
  pinMode(MA1, OUTPUT);
  pinMode(MA2, OUTPUT);
  pinMode(MB1, OUTPUT);
  pinMode(MB2, OUTPUT);
  pinMode(MCD1, OUTPUT);
  pinMode(MCD2, OUTPUT);
  //Interrupción externa
  attachInterrupt(pin_RPM, isr, RISING);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (millis() - lastMsg > 1000) {
    detachInterrupt(pin_RPM);
    rpm     = (60 * 1000 / pulsos_vuelta ) / (millis() - lastMsg) * pulsos;
    lastMsg = millis();
    pulsos  = 0;
    snprintf (msg, MSG_BUFFER_SIZE, "%i", rpm);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("iot/car/rpm", msg);
    //Calidad aire
    c_aire = analogRead(pin_Caire);
    snprintf(msg_a, MSG_BUFFER_AIRE, "%i", c_aire);
    Serial.print("Publish message: ");
    Serial.println(msg_a);
    client.publish("iot/car/mq135", msg_a);
    attachInterrupt(pin_RPM, isr, RISING);
  }
}

//Implementación de funciones

// Conexión wifi
void setup_wifi(void) {
  delay(10);
  // Conectando a red wifi
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi conectada");
  Serial.println("Dirección IP: ");
  Serial.println(WiFi.localIP());
}

// Funcion Callback 
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if (String(topic) == "iot/car")
  {
    if ((char)payload[0] == '5')
    {
      act = true;
    }

    if ((char)payload[0] == '6')
    {
      act = false;
    }
  }
  
  if (act)
  {
    if (String(topic) == "iot/car/motor") {
      char op = (char)payload[0];
      switch (op)
      {
        case '1':
          digitalWrite(MA1, HIGH);
          digitalWrite(MA2, LOW);
          digitalWrite(MB1, LOW);
          digitalWrite(MB2, HIGH);
          digitalWrite(MCD1, HIGH);
          digitalWrite(MCD2, LOW);
          break;
        case '2':
          digitalWrite(MA1, LOW);
          digitalWrite(MA2, HIGH);
          digitalWrite(MB1, HIGH);
          digitalWrite(MB2, LOW);
          digitalWrite(MCD1, LOW);
          digitalWrite(MCD2, HIGH);
          break;
        case '3':
          digitalWrite(MA1, LOW);
          digitalWrite(MA2, HIGH);
          digitalWrite(MB1, HIGH);
          digitalWrite(MB2, LOW);
          digitalWrite(MCD1, HIGH);
          digitalWrite(MCD2, LOW);
          break;
        case '4':
          digitalWrite(MA1, HIGH);
          digitalWrite(MA2, LOW);
          digitalWrite(MB1, LOW);
          digitalWrite(MB2, HIGH);
          digitalWrite(MCD1, LOW);
          digitalWrite(MCD2, HIGH);
        break;
        default:
          Stop();
          break;
      }
    }
  }
}  

//Reconexión wifi
void reconnect(void) {
  // Bucle hasta que se conecte
  while (!client.connected()) {
    Serial.print("Esperando conexion MQTT...");
    // Creando un ID de cliente aleatorio
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    // Esperando conexión
    if (client.connect(clientId.c_str())) {
      Serial.println("Conectado");
      // Once connected, publish an announcement...
      //client.publish("iot/car/motor", "Motor conectado");
      // ... and resubscribe
      client.subscribe("iot/car/motor");
      client.subscribe("iot/car");
      //client.subscribe("iot/car/rpm");
      //client.subscribe("iot/car/moq135");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// Detener car wifi
void Stop(void)
{
  digitalWrite(MA1, LOW);
  digitalWrite(MA2, LOW);
  digitalWrite(MB1, LOW);
  digitalWrite(MB2, LOW);
  digitalWrite(MCD1, LOW);
  digitalWrite(MCD2, LOW);
}

void IRAM_ATTR isr(void) {
  pulsos++; //contador pulsos
}