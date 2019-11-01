uint8_t entradas[] = {34, 35, 32, 33, 25};
#define N_ENT 5
uint8_t saidas[] = {2, 4, 5, 18, 19, 21, 22, 23};
#define N_SAI 8

//Estrutura para capturar borda e debounce
struct porta_t {
  uint8_t ultimoEstado;
  long ultimoMillis;
};

typedef struct porta_t PORTA;
PORTA p_entrada[N_ENT];
PORTA p_saida[N_SAI];


extern AsyncMqttClient mqttClient;


void setSaida(uint8_t saida, uint8_t level) {
  p_saida[saida].ultimoEstado = level;
  digitalWrite(saidas[saida], level);
}
uint8_t getSaida(uint8_t saida) {
  return p_saida[saida].ultimoEstado;
}
void processaReqMQTT(const char* topic, const char* payload, size_t len) {
  char saida = topic[strlen(topic) - 1] - '0';
  if ((saida >= 0) && (saida < N_SAI) ) {
    setSaida(saida, strncmp(payload, "ON", len) == 0 ? LOW : HIGH);
  }
}
String topicoEntrada = "/"MQTT_HOSTNAME"/entradas/";
String topicoSaida = "/"MQTT_HOSTNAME"/saidas/";
void publicaReqMQTT(uint8_t entOuSai, uint8_t idx, uint8_t level) {
  String topico = entOuSai ? topicoEntrada : topicoSaida + String(idx);
  if (mqttClient.connected()) {
    mqttClient.publish(topico.c_str(), 1, true, level ? "ON" : "OFF");
  }
}


void setupSaidasOnMQTT() {
  for (int i = 0; i < N_SAI; i++) {
    publicaReqMQTT(false, i, !getSaida(i));
  }
}
void setupIO() {
  for (int i = 0; i < N_ENT; i++) {
    pinMode(entradas[i], INPUT);
    PORTA &p = p_entrada[i];
    p.ultimoMillis = 0;
    p.ultimoEstado = HIGH;
  }
  for (int i = 0; i < N_SAI; i++) {
    pinMode(saidas[i], OUTPUT);
    setSaida(i, HIGH);
  }
}

void loopIO() {
  uint8_t sinal;
  PORTA *p;
  long m = millis();
  for (int i = 0; i < N_ENT; i++) {
    sinal = digitalRead(entradas[i]);
    p = &p_entrada[i];
    if ((p->ultimoEstado != sinal) && ((m - p->ultimoMillis) > DEBOUNCE_ENTRADA)) {
      //publica!
      publicaReqMQTT(true, i, sinal);
      p->ultimoEstado = sinal;
      p->ultimoMillis = m;
    }
  }
}
