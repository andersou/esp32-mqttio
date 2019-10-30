uint8_t entradas[] = {34, 35, 32, 33, 25};
#define N_ENT 5
uint8_t saidas[] = {2, 4, 5, 18, 19, 21, 22, 23};
#define N_SAI 8

//Estrutura para capturar borda e debounce
struct porta_t {
  uint8_t ultimoEstado;
  uint8_t ultimoMillis;
};

typedef struct porta_t PORTA;
PORTA p_entrada[N_ENT];

extern AsyncMqttClient mqttClient;
void setupIO() {
  for (int i = 0; i < N_ENT; i++) {
    pinMode(entradas[i], INPUT);
    PORTA &p = p_entrada[i];
    p.ultimoMillis = 0;
    p.ultimoEstado = HIGH;
  }
  for (int i = 0; i < N_SAI; i++) {
    pinMode(saidas[i], OUTPUT);
    digitalWrite(saidas[i], HIGH);
  }
}
void processaReqMQTT(const char* topic, const char* payload) {
  Serial.println("Processando msg");

  char saida = topic[strlen(topic)-1] -'0';
  Serial.println(saida);
  Serial.println(payload);
  Serial.println(topic);
  Serial.println(strlen(topic));


  if ((saida >= 0) && (saida < N_SAI) ) {
    digitalWrite(saidas[saida], strcmp(payload, "ON") == 0 ? LOW : HIGH);
    Serial.println("FOI");
  }
}
void publicaReqMQTT(uint8_t idx, uint8_t level) {
  String topico = "/"MQTT_HOSTNAME"/entrada/" + String(idx);
  if (mqttClient.connected()) {
    mqttClient.publish(topico.c_str(), 1, true, level ? "ON" : "OFF");
  }
}
void loopIO() {
  uint8_t sinal;
  PORTA *p;
  long m = millis();
  for (int i = 0; i < N_ENT; i++) {
    sinal = digitalRead(entradas[i]);
    p = &p_entrada[i];
    if ((p->ultimoEstado != sinal) && (m - p->ultimoMillis) > DEBOUNCE_ENTRADA) {
      //publica!
      publicaReqMQTT(i, sinal);
      p->ultimoEstado = sinal;
      p->ultimoMillis = m;
    }
  }
}
