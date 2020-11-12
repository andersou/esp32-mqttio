#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);


/*
  /CLIENT/saidas ou /saidas -> começa 0
  /entradas   -> começa em 0
*/
String topicos[] = {"/saidas/#", ""};
long lastMsg = 0, lastReconnect = 0;
char msg[50];
int value = 0;

void callback(const char *topic, byte *payload, unsigned int length)
{
  Serial.print("Mensagem recebida [");
  Serial.print(topic);
  Serial.print("] ");
  processaReqMQTT(topic, (const char *)payload, length);
}

void reconnect()
{
  Serial.print("Tentando conexão com servidor MQTT...");
  String clientId = MQTT_CLIENT;
  // Attempt to connect
  if (client.connect(clientId.c_str()))
  {
    String s = topicos[0];
    int i = 0;
    while (!s.isEmpty())
    {
      client.subscribe(s.c_str());
      client.subscribe( ("/" +String(MQTT_CLIENT) + s).c_str());
      s = topicos[++i];
    }
    lastReconnect = millis();
  }
  else
  {
    Serial.print("erro, rc=");
    Serial.print(client.state());
  }
}

void setupMQTT()
{
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(callback);
}

void loopMQTT()
{
  long nowLoop = millis();
  if (!client.connected()) {
    reconnect();
    if ((nowLoop - lastReconnect > 3000))
    {
      Serial.println("Nao conectou mqtt! Rebooting...");
      delay(5000);
      ESP.restart();
      return;

    }
  }



  client.loop();
  if (nowLoop - lastMsg > 2000)
  {
    lastMsg = nowLoop;
    ++value;
    snprintf(msg, 50, "#%ld", millis());
    Serial.println(msg);
    client.publish( "/"MQTT_HOSTNAME"/heartbeat/", msg);
  }
}
