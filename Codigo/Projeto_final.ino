#include <arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <stdlib.h>

#define SETA_DIR 5
#define SETA_ESQ 27
#define FAROL 18
#define FREIO 21

unsigned long tempoSensorAnterior = 0;
unsigned long tempoMotorAnterior = 0;
const long intervaloMotor = 110;
const long intervaloSensor = 2000;

#define TOPICO_SUBSCRIBE_ANDAR            "topico_andar"
#define TOPICO_PUBLISH_TEMPERATURA        "topico_sensor_temperatura"
#define TOPICO_PUBLISH_DISTANCIA_FRENTE   "topico_sensor_ultrassonico_frente"
#define TOPICO_PUBLISH_DISTANCIA_TRASEIRO "topico_sensor_ultrassonico_traseiro"
#define TOPICO_PUBLISH_CLARIDADE          "topico_sensor_luminosidade"

#define ID_MQTT  "Trabalho_IOT_Carro"


const char* BROKER_MQTT = "test.mosquitto.org";
int BROKER_PORT = 1883;
const char* SSID = "";//mude para a sua rede
const char* PASSWORD = "";//mude para a senha da sua rede

WiFiClient espClient; 
PubSubClient MQTT(espClient);

long numAleatorio;

void initWiFi(void);
void initMQTT(void);
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void reconnectMQTT(void);
void reconnectWiFi(void);
void VerificaConexoesWiFIEMQTT(void);

void initWiFi(void) {
  delay(10);
  Serial.println("------Conexao WI-FI------");
  Serial.print("Conectando-se na rede: ");
  Serial.println(SSID);
  Serial.println("Aguarde");

  reconnectWiFi();
}

void initMQTT(void) {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
  MQTT.setCallback(mqtt_callback);            //atribui função de callback (função chamada quando qualquer informação de um dos tópicos subescritos chega)
}

char comandoRecebido = '\0';


void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  char msg[200] = "";

  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
  msg[length] = '\0';

  Serial.print("Chegou a seguinte string via MQTT: ");
  Serial.println(msg);

  if (msg[0] >= '1' && msg[0] <= '8') {
    comandoRecebido = msg[0];
  }
}


void reconnectMQTT(void) {
  while (!MQTT.connected())
  {
    Serial.print("* Tentando se conectar ao Broker MQTT: ");
    Serial.println(BROKER_MQTT);
    if (MQTT.connect(ID_MQTT))
    {
      Serial.println("Conectado com sucesso ao broker MQTT!");
      MQTT.subscribe(TOPICO_SUBSCRIBE_ANDAR);
    }
    else
    {
      Serial.println("Falha ao reconectar no broker.");
      Serial.println("Havera nova tentativa de conexao em 2s");
      delay(2000);
    }
  }  
}

void VerificaConexoesWiFIEMQTT(void) {
  if (!MQTT.connected())
    reconnectMQTT(); //se não há conexão com o Broker, a conexão é refeita

  reconnectWiFi(); //se não há conexão com o WiFI, a conexão é refeita
}

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

//======================================Celula================================================

typedef struct Celula {
    int valor;
    struct Celula *prox;
} Celula;

Celula *newCelula(int valor);

// Agora a função pode ser definida normalmente
Celula *newCelula(int valor) {
    Celula *nova = (Celula *)malloc(sizeof(Celula));
    nova->valor = valor;
    nova->prox = NULL;
    return nova;
}


//==================================Fila=========================================================

typedef struct Fila {
    Celula *primeiro;
    Celula *ultimo;
} Fila;

Fila *newFila(int valor);

Fila *newFila(int valor) {
    Fila *nova = (Fila *)malloc(sizeof(Fila));
    Celula *elemento = newCelula(valor);
    nova->primeiro = nova->ultimo = elemento;
    return nova;
}

Fila *fila;  // fila global

int verificarDez() {
    int count = 0;
    Celula *i = fila->primeiro;
    while (i != NULL) {
        count++;
        i = i->prox;
    }
    return (count == 10);
}

void inserir(int valor) {
    if (verificarDez()) {
        Celula *temp = fila->primeiro;
        fila->primeiro = fila->primeiro->prox;
        free(temp);  // libera memória da célula removida
    }

    Celula *nova = newCelula(valor);
    fila->ultimo->prox = nova;
    fila->ultimo = nova;
}

//====================================Sensor Ultrassonico Frente============================================

#define TRIGFrente 15
#define ECHOFrente 16

float distanciaFrente(){
  digitalWrite(TRIGFrente, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGFrente, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGFrente, LOW);

  long duration = pulseIn(ECHOFrente, HIGH);
  float distance = (duration * 0.0343) / 2;
  return distance;
}

//====================================Sensor Ultrassonico Traseira==========================================

#define TRIGTraseiro 22
#define ECHOTraseiro 23

float distanciaTraseiro(){
  digitalWrite(TRIGTraseiro, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGTraseiro, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGTraseiro, LOW);

  long duration = pulseIn(ECHOTraseiro, HIGH);
  float distance = (duration * 0.0343) / 2;
  return distance;
}

//====================================Sensor Luminosidade===========================================

#define ldr 32
int valorldr = 0;

const char* claroEscuro(){
  valorldr = analogRead(ldr);
  inserir(valorldr);
  Serial.print("Valor lido pelo LDR = ");
  Serial.println(valorldr);

  return (valorldr > 1000) ? "Claro" : "Escuro";
}

int mediaValores() {
    valorldr = analogRead(ldr);
    inserir(valorldr);
    int contador = 0;
    int soma = 0;
    Celula *atual = fila->primeiro;
    while (atual != NULL) {
        soma += atual->valor;
        contador++;
        atual = atual->prox;
    }
    if (contador == 0) return 0;
    return soma / contador;
}

//=====================================Sensor Temperatura===========================================

int PinoNTC = 33;
double Vs = 3.3;
double R1 = 10000;
double Beta = 3950;
double To=298.15;
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

//========================================Controle Carrinho======================================

const int IN1A = 25;
const int IN2A = 26;
const int IN3B = 12;
const int IN4B = 13;

bool piscandoSetaEsq = false;
bool piscandoSetaDir = false;
unsigned long tempoUltimoPiscaSeta = 0;
const long intervaloPiscaSeta = 300;
int contadorPiscaSetaEsq = 0;
int contadorPiscaSetaDir = 0;


void andarFrenteEsquerda(){
  digitalWrite(FREIO, LOW);
  digitalWrite(IN1A, LOW);
  digitalWrite(IN2A, LOW);
  digitalWrite(IN3B, LOW);
  digitalWrite(IN4B, HIGH);
}

void andarFrente(){
  digitalWrite(FREIO, LOW);
  digitalWrite(IN1A, LOW);
  digitalWrite(IN2A, HIGH);
  digitalWrite(IN3B, LOW);
  digitalWrite(IN4B, HIGH);
}

void andarFrenteDireita(){
  digitalWrite(FREIO, LOW);
  digitalWrite(IN1A, LOW);
  digitalWrite(IN2A, HIGH);
  digitalWrite(IN3B, LOW);
  digitalWrite(IN4B, LOW);
}

void andarEsquerda(){
  digitalWrite(FREIO, LOW);
  digitalWrite(IN1A, HIGH);
  digitalWrite(IN2A, LOW);
  digitalWrite(IN3B, LOW);
  digitalWrite(IN4B, HIGH);
}

void andarDireita(){
  digitalWrite(FREIO, LOW);
  digitalWrite(IN1A, LOW);
  digitalWrite(IN2A, HIGH);
  digitalWrite(IN3B, HIGH);
  digitalWrite(IN4B, LOW);
}

void andarReEsquerda(){
  digitalWrite(FREIO, LOW);
  digitalWrite(IN1A, LOW);
  digitalWrite(IN2A, LOW);
  digitalWrite(IN3B, HIGH);
  digitalWrite(IN4B, LOW);
}

void andarRe(){
  digitalWrite(FREIO, LOW);
  digitalWrite(IN1A, HIGH);
  digitalWrite(IN2A, LOW);
  digitalWrite(IN3B, HIGH);
  digitalWrite(IN4B, LOW);
}

void andarReDireita(){
  digitalWrite(FREIO, LOW);
  digitalWrite(IN1A, HIGH);
  digitalWrite(IN2A, LOW);
  digitalWrite(IN3B, LOW);
  digitalWrite(IN4B, LOW);
}

void parado(){
  Serial.println("Parado!");
  digitalWrite(IN1A, LOW);
  digitalWrite(IN2A, LOW);
  digitalWrite(IN3B, LOW);
  digitalWrite(IN4B, LOW);
  digitalWrite(FREIO, HIGH);
}

bool alertaAtivo = false;
unsigned long tempoUltimoPisca = 0;
const long intervaloPiscaAlerta = 500;

void piscaAlerta() {
  static bool estado = false;
  estado = !estado;
  digitalWrite(SETA_DIR, estado);
  digitalWrite(SETA_ESQ, estado);
}

void desligarAlerta() {
  alertaAtivo = false;
  digitalWrite(SETA_DIR, LOW);
  digitalWrite(SETA_ESQ, LOW);
}

void gerenciarPiscaSeta() {
  if (alertaAtivo) return; // se alerta ligado, não pisca setas individuais

  unsigned long agora = millis();

  if (piscandoSetaEsq && agora - tempoUltimoPiscaSeta >= intervaloPiscaSeta) {
    tempoUltimoPiscaSeta = agora;
    digitalWrite(SETA_ESQ, !digitalRead(SETA_ESQ));
    contadorPiscaSetaEsq++;
    if (contadorPiscaSetaEsq >= 6) { // 3 piscadas (liga/desliga = 2)
      piscandoSetaEsq = false;
      digitalWrite(SETA_ESQ, LOW);
    }
  }

  if (piscandoSetaDir && agora - tempoUltimoPiscaSeta >= intervaloPiscaSeta) {
    tempoUltimoPiscaSeta = agora;
    digitalWrite(SETA_DIR, !digitalRead(SETA_DIR));
    contadorPiscaSetaDir++;
    if (contadorPiscaSetaDir >= 6) {
      piscandoSetaDir = false;
      digitalWrite(SETA_DIR, LOW);
    }
  }
}


//=======================================SETUP===================================================

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\nDisciplina IoT: acesso a nuvem via ESP32");
  delay(1000);

  fila = newFila(analogRead(ldr));

  //LEDs
  pinMode(SETA_DIR, OUTPUT);
  pinMode(SETA_ESQ, OUTPUT);
  pinMode(FAROL, OUTPUT);
  pinMode(FREIO, OUTPUT);

  //Motor
  pinMode(IN1A, OUTPUT);
  pinMode(IN2A, OUTPUT);
  pinMode(IN3B, OUTPUT);
  pinMode(IN4B, OUTPUT);  

  //Sensor Ultrassonico
  pinMode(TRIGFrente, OUTPUT);
  pinMode(ECHOFrente, INPUT);

  
  //Sensor Ultrassonico
  pinMode(TRIGTraseiro, OUTPUT);
  pinMode(ECHOTraseiro, INPUT);

  //Sensor Luminosidade
  pinMode(ldr, INPUT);

  //Sensor Temperatura
  pinMode(PinoNTC, INPUT);

  //Acender todos os LEDs
  digitalWrite(SETA_DIR, HIGH);
  digitalWrite(SETA_ESQ, HIGH);
  digitalWrite(FAROL, HIGH);
  digitalWrite(FREIO, HIGH);
  delay(1000);
  digitalWrite(SETA_DIR, LOW);
  digitalWrite(SETA_ESQ, LOW);
  digitalWrite(FAROL, LOW);
  digitalWrite(FREIO, LOW);
  
  initWiFi();

  initMQTT();
}

//=========================================LOOP==================================================

void loop() {
  VerificaConexoesWiFIEMQTT();

  if(mediaValores() < 1000){
    digitalWrite(FAROL, HIGH);
  }
  else{
    digitalWrite(FAROL, LOW);
  }
  unsigned long tempoAtual = millis();

  gerenciarPiscaSeta();

  if (alertaAtivo && (tempoAtual - tempoUltimoPisca >= intervaloPiscaAlerta)) {
    tempoUltimoPisca = tempoAtual;
    piscaAlerta();
  }


  char temperatura_str[10] = {};
  char luminosidade_str[10] = {};
  char distanciaFrente_str[10] = {};
  char distanciaTraseiro_str[10] = {};

  if(tempoAtual - tempoSensorAnterior >= intervaloSensor){
    tempoSensorAnterior = tempoAtual;

    sprintf(temperatura_str, "%.1fC", temperaturaNTC());
    sprintf(distanciaFrente_str, "%.1fcm", distanciaFrente());
      Serial.print("Distancia: ");
      Serial.println(distanciaFrente_str);
    sprintf(distanciaTraseiro_str, "%.1fcm", distanciaTraseiro());
      Serial.print("Distancia: ");
      Serial.println(distanciaTraseiro_str);
    strncpy(luminosidade_str, claroEscuro(), sizeof(luminosidade_str) - 1);
    luminosidade_str[sizeof(luminosidade_str) - 1] = '\0';

    MQTT.publish(TOPICO_PUBLISH_TEMPERATURA, temperatura_str);
    MQTT.publish(TOPICO_PUBLISH_DISTANCIA_FRENTE, distanciaFrente_str);
    MQTT.publish(TOPICO_PUBLISH_DISTANCIA_TRASEIRO, distanciaTraseiro_str);
    MQTT.publish(TOPICO_PUBLISH_CLARIDADE, luminosidade_str);
  }

  static char ultimoComando = '\0';

  if(tempoAtual - tempoMotorAnterior >= intervaloMotor){
    tempoMotorAnterior = tempoAtual;
    MQTT.loop();

    bool mudouSentido = false;

    if ((ultimoComando >= '1' && ultimoComando <= '3') && (comandoRecebido >= '6' && comandoRecebido <= '8')) {
      mudouSentido = true; // frente -> ré
    } else if ((ultimoComando >= '6' && ultimoComando <= '8') && (comandoRecebido >= '1' && comandoRecebido <= '3')) {
      mudouSentido = true; // ré -> frente
    } else if ((ultimoComando == '4' && comandoRecebido == '5') || (ultimoComando == '5' && comandoRecebido == '4')) {
      mudouSentido = true; // esquerda -> direita ou direita -> esquerda
    }

    if (mudouSentido) {
      parado();
      delay(100); // pausa curta para evitar pico de corrente
    }

    float distF = distanciaFrente();
    float distT = distanciaTraseiro();

    bool indoFrente = (comandoRecebido >= '1' && comandoRecebido <= '3');
    bool indoTras   = (comandoRecebido >= '6' && comandoRecebido <= '8');

    bool comandoDeMovimento = (comandoRecebido >= '1' && comandoRecebido <= '8');

    if (comandoDeMovimento && ((indoFrente && distF < 25.0) || (indoTras && distT < 25.0))) {
      parado();
      if (!alertaAtivo) {
        alertaAtivo = true;
        tempoUltimoPisca = tempoAtual; // reseta pisca
      }
    }
    else if (alertaAtivo) {
      desligarAlerta();
    }

    else{
      switch(comandoRecebido){
        case '1':
          andarFrenteEsquerda();
          piscandoSetaEsq = true;
          contadorPiscaSetaEsq = 0;
          break;
        case '2':
          andarFrente();
          break;
        case '3':
          andarFrenteDireita();
          piscandoSetaDir = true;
          contadorPiscaSetaDir = 0;
          break;
        case '4':
          andarEsquerda();
          piscandoSetaEsq = true;
          contadorPiscaSetaEsq = 0;
          break;
        case '5':
          andarDireita();
          piscandoSetaDir = true;
          contadorPiscaSetaDir = 0;
          break;
        case '6':
          andarReEsquerda();
          piscandoSetaEsq = true;
          contadorPiscaSetaEsq = 0;
          break;
        case '7':
          andarRe();
          break;
        case '8':
          andarReDireita();
          piscandoSetaDir = true;
          contadorPiscaSetaDir = 0;
          break;
        default:
          parado();
          break;
      }
    }

    ultimoComando = comandoRecebido;
    comandoRecebido = '\0';
  }  
}
