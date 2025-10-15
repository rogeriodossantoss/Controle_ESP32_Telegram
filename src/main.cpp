#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "DHT.h"

#define WIFI_SSID "NansenADM"
#define WIFI_PASSWORD ""
#define BOTtoken ""

#define LED_PIN1 2
#define LED_PIN2 15
#define DHT_PIN 23
#define DHTTYPE DHT11

#define BOT_SCAN 1000 
long lastTimeScan;

bool ledStatus1;
bool ledStatus2;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);
DHT dht(DHT_PIN, DHTTYPE);

float humidity, temperature;
String message, chat_id;

void handleNewMessages(int numNewMessages) {
  String text, from_name;

  for (int i=0; i<numNewMessages; i++) {
    chat_id = String(bot.messages[i].chat_id);
    from_name = bot.messages[i].from_name;    
    text = bot.messages[i].text;
    Serial.println("ChatID " + chat_id);
    Serial.println(text);

    if (text == "/led1") {
      digitalWrite(LED_PIN1, HIGH);
      ledStatus1 = true;
      bot.sendMessage(chat_id, "Led está ligado", "");
      Serial.println("Led esta ligado");
    }
    
    if (text == "/led2") {
      digitalWrite(LED_PIN2, HIGH);
      ledStatus2 = true;
      bot.sendMessage(chat_id, "Led está ligado", "");
      Serial.println("Led esta ligado");
    }

    if (text == "/ledoff") {
      ledStatus1 = false;
      digitalWrite(LED_PIN1, LOW);
      bot.sendMessage(chat_id, "Led está desligado", "");
      Serial.println("Led esta desligado");
    }

    if (text == "/ledoff")
    {
      ledStatus2 = false;
      digitalWrite(LED_PIN2, LOW);
      bot.sendMessage(chat_id, "Led está desligado", "");
      Serial.println("Led esta desligado");
    }

    if( text == "/env") {
      humidity = dht.readHumidity();
      temperature = dht.readTemperature();
      message = "A temperatura é de " + String(temperature, 2) + " graus celsius.\n";
      message += "A umidade relativa do ar é de " + String(humidity, 2)+ "%.\n";
      bot.sendMessage(chat_id, message, "Markdown");
      Serial.println(message);      
    }

   if (text == "/status") {
  String message = "";

  // LED 1
  message += "LED 1 está ";
  if (ledStatus1) {
    message += "ligado";
  } else {
    message += "desligado";
  }

  // Quebra de linha entre os dois LEDs
  message += "\n";

  // LED 2
  message += "LED 2 está ";
  if (ledStatus2) {
    message += "ligado";
  } else {
    message += "desligado";
  }
      message += ". \n";
      humidity = dht.readHumidity();
      temperature = dht.readTemperature();
      message += "A temperatura é de " + String(temperature, 2) + " graus celsius.\n";
      message += "A umidade relativa do ar é de " + String(humidity, 2)+ "%.\n";
      bot.sendMessage(chat_id, message, "");
      Serial.println(message);
    }

    if (text == "/start") {
      String welcome = from_name + ", bem vindo ao Bot do DevTitans do Barreto.\n";
      welcome += "Para interagir, use um dos comandos a seguir.\n";
      welcome += "/ledon : para ligar o Led \n";
      welcome += "/ledoff : para desligar o Led \n";
      welcome += "/env : saber a temperatura e umidade do ambiente \n";
      welcome += "/status : para saber o status dos sensores e atuadores \n";
      bot.sendMessage(chat_id, welcome, "");
      Serial.println(welcome);
    }
  }
}


void setupWifi(){  
  Serial.print("Connecting Wifi: ");
  Serial.println(WIFI_SSID);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
}

void setupPins(){
  pinMode(LED_PIN1, OUTPUT); 
  pinMode(LED_PIN2, OUTPUT); 
  pinMode(DHT_PIN, INPUT); 
  digitalWrite(LED_PIN1, LOW);
  digitalWrite(LED_PIN2, LOW);
  ledStatus1 = false;
  ledStatus2 = false;
  dht.begin();
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  setupWifi();
  setupPins();
  lastTimeScan = millis();
}

void loop() {
  if (millis() > lastTimeScan + BOT_SCAN)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);  
    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    // Geração de Alertas de Temperatura
    temperature = dht.readTemperature(); 
    if (temperature > 30) {
      message = "Alerta: Temperatura alta! " + String(temperature) + " °C";
      bot.sendMessage("-4507574946", message, "");
      Serial.println(message);
    }
    lastTimeScan = millis();
  }
}
