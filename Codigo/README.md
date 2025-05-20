#include <arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

#define PIN_LED 22

/* Definicoes para o MQTT */
#define TOPICO_SUBSCRIBE_LED         "topico_liga_desliga_led"
#define TOPICO_PUBLISH_TEMPERATURA   "topico_sensor_temperatura"
#define TOPICO_PUBLISH_DISTANCIA      "topico_sensor_ultrassonico"
#define TOPICO_PUBLISH_CLARIDADE     "topico_sensor_luminosidade"

#define ID_MQTT  "Trabalho_IOT_Carro"     //id mqtt (para identificação de sessão)


const char* BROKER_MQTT = "test.mosquitto.org";
//const char* BROKER_MQTT = "broker.emqx.io";
int BROKER_PORT = 1883; // Porta do Broker MQTT

//celular
//const char* SSID = "NET_PAGANI";
//const char* PASSWORD = "93060874";

//const char* SSID = "iPhone(2)";//
//const char* PASSWORD = "12345678";

const char* SSID = "IphoneBernardo";//
const char* PASSWORD = "Baer1234";

//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient

long numAleatorio;


/* Prototypes */
void initWiFi(void);
void initMQTT(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT(void);
void reconnectWiFi(void);
void VerificaConexoesWiFIEMQTT(void);

/*
   Implementações
*/

/* Função: inicializa e conecta-se na rede WI-FI desejada
   Parâmetros: nenhum
   Retorno: nenhum
*/
void initWiFi(void) {
  delay(10);
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");

  reconnectWiFi();
}


/* Função: inicializa parâmetros de conexão MQTT(endereço do
           broker, porta e inicializa a função de callback)
   Parâmetros: nenhum
   Retorno: nenhum
*/
void initMQTT(void) {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
  MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}

/* Função: função de callback
           esta função é chamada toda vez que uma informação de
           um dos tópicos subescritos chega)
   Parâmetros: nenhum
   Retorno: nenhum
*/
void mqtt_callback(char* topic, byte* payload, unsigned int length){
  String msg;
  String ele = "L";

  /* obtem a string do payload recebido */
  for (int i = 0; i < length; i++)
  {
    char c = (char)payload[i];
    msg += c;
  }

  Serial.print("Chegou a seguinte string via MQTT: ");
  Serial.println(msg);
  /* toma ação dependendo da string recebida */
  //if(msg.equals("L")
  if(msg[0] == 'L')
  {
    digitalWrite(PIN_LED, HIGH);
    Serial.println("LED aceso mediante comando MQTT");
  }
  else
  //if (msg.equals("D"))
  if(msg[0] == 'D')
  {
    digitalWrite(PIN_LED, LOW);
    Serial.println("LED apagado mediante comando MQTT");
  }
  else
    Serial.println("Não identificou comando MQTT");
}


/* Função: reconecta-se ao broker MQTT (caso ainda não esteja conectado ou em caso de a conexão cair)
           em caso de sucesso na conexão ou reconexão, o subscribe dos tópicos é refeito.
   Parâmetros: nenhum
   Retorno: nenhum
*/
void reconnectMQTT(void) {
  while (!MQTT.connected())
  {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT))
    {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe(TOPICO_SUBSCRIBE_LED);
    }
    else
    {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Havera nova tentativa de conexao em 2s");
      delay(2000);
    }
  }  
}

/* Função: verifica o estado das conexões WiFI e ao broker MQTT.
           Em caso de desconexão (qualquer uma das duas), a conexão
           é refeita.
   Parâmetros: nenhum
   Retorno: nenhum
*/
void VerificaConexoesWiFIEMQTT(void) {
  if (!MQTT.connected())
    reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita

  reconnectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

/* Função: reconecta-se ao WiFi
   Parâmetros: nenhum
   Retorno: nenhum
*/
void reconnectWiFi(void) {
  //se já está conectado à rede WI-FI, nada é feito.
  //Caso contrário, são efetuadas tentativas de conexão
  if (WiFi.status() == WL_CONNECTED)
    return;

  WiFi.begin(SSID, PASSWORD); // Conecta na rede WI-FI

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(100);
    Serial.println(".");
  }

  Serial.println();
  Serial.print("Conectado com sucesso na rede ");
  Serial.print(SSID);
  Serial.println("\nIP obtido: ");
  Serial.println(WiFi.localIP());
}

//Sensor Ultrassonico

#define TRIG 15
#define ECHO 16
const float distanciaMAX = 10.00;

float distancia(){
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  long duration = pulseIn(ECHO, HIGH);
  float distance = (duration * 0.0343) / 2;

  Serial.print("Distancia: ");
  Serial.print(distance);
  Serial.println(" cm");

  return distance;
}

//Sensor Luminosidade

#define ldr 32
int valorldr = 0;//Declara a variável valorldr como inteiro

const char* claroEscuro(){
  valorldr = analogRead(ldr);
  Serial.print("Valor lido pelo LDR = ");
  Serial.println(valorldr);

  return (valorldr > 1500) ? "Claro" : "Escuro";
}

int PinoNTC = 33; // PINO DO NTC10K
double Vs = 3.3; // TENSÃO DE SAIDA DO ESP32
double R1 = 10000; //RESISTOR UTILIZADO NO DIVISOR DE TENSÃO
double Beta = 3950; // VALOR DE BETA
double To=298.15; // VALOR EM KELVIN REFERENTE A 25° CELSIUS
double Ro=10000;
double adcMax = 4095.0;

double temperaturaNTC() {
  //GARANTE QUE AS INFORMAÇÕES SERÃO RESETADAS APÓS CADA LEITURA
  double Vout, Rt = 0;
  double T, Tc, Tf;
  int adc = analogRead(PinoNTC);// VARIÁVEL QUE RECEBE A LEITURA DO NTC10K
  //CALCULOS PARA CONVERSÃO DA LEITURA RECEBIDA PELO ESP32 EM TEMPERATURA EM °C
  Vout = adc * Vs/adcMax;
  Rt = R1 * Vout / (Vs - Vout);
  T = 1/(1/To + log(Rt/Ro)/Beta);
  Tc = T - 273.15;
  Tf = Tc * 9 / 5 + 32;
  //Imprime no monitor serial o texto e a temperatura lida pelo sensor
  Serial.print("Temperatura: ");
  Serial.print(Tc);
  Serial.println(" °C");

  return Tc;
}

void setup() {
  Serial.begin(115200); //Enviar e receber dados em 9600 baud
  delay(1000);
  Serial.println("\nDisciplina IoT: acesso a nuvem via ESP32");
  delay(1000);
  // programa LED interno como saida
  
  pinMode(PIN_LED, OUTPUT);

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(ldr, INPUT);
  pinMode(PinoNTC, INPUT);

  digitalWrite(PIN_LED, HIGH);
  delay(1000);
  digitalWrite(PIN_LED, LOW);
  

  /* Inicializa a conexao wi-fi */
  initWiFi();

  /* Inicializa a conexao ao broker MQTT */
  initMQTT();
}

// the loop function runs over and over again forever
void loop() {

  // cria string para temperatura, luminosidade e distancia.
  char temperatura_str[10] = {};
  char luminosidade_str[10] = {};
  char distancia_str[10] = {};
  
  // garante funcionamento das conexões WiFi e ao broker MQTT 
  VerificaConexoesWiFIEMQTT();

  
  // formata a temperatura como string
  sprintf(temperatura_str, "%.1fC", temperaturaNTC());

  // formata a distancia com string
  sprintf(distancia_str, "%.1fcm", distancia());

  strncpy(luminosidade_str, claroEscuro(), sizeof(luminosidade_str) - 1);
  luminosidade_str[sizeof(luminosidade_str) - 1] = '\0';

  // Receber se esta claro ou escuro pela funcao

  /*  Publica a temperatura, distancia e claridade */
  MQTT.publish(TOPICO_PUBLISH_TEMPERATURA, temperatura_str);
  MQTT.publish(TOPICO_PUBLISH_DISTANCIA, distancia_str);
  MQTT.publish(TOPICO_PUBLISH_CLARIDADE, luminosidade_str);
  
  /* keep-alive da comunicação com broker MQTT */
  MQTT.loop();

  /* Refaz o ciclo após 2 segundos */
  delay(2000);
}
