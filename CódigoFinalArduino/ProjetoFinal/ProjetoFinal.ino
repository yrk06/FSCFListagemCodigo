#include <LiquidCrystal.h>
#include "DHTesp.h"
#include "ThingSpeak.h"
#include "WiFi.h"


#define WIFI_NETWORK "ssid"
#define WIFI_PASSWORD "pass"

#define WIFI_TIMEOUT_MS 20000

#define CHANNEL_ID "ID"
#define CHANNEL_API_KEY "API_KEY"

void connectToWifi() {
   Serial.print("Connecting to WiFi..."); // Para saber se o ESP está tentando se conectar
   WiFi.mode(WIFI_STA); // Modo de WiFi Station, conectando à rede já existente
   WiFi.begin(WIFI_NETWORK, WIFI_PASSWORD); // Iniciando a conexão à rede com a senha passada
   
   unsigned long startAttemptTime = millis(); // Começo da contagem de tempo das tentativas de conexão
   while((WiFi.status() != WL_CONNECTED) && (millis() - startAttemptTime) < WIFI_TIMEOUT_MS) {
      //Serial.print(".");
      delay(100);
   }

   if(WiFi.status() != WL_CONNECTED) {
      Serial.println("Failed while trying to connect to network.");
      connectToWifi();
   }else{
      Serial.printf("Connected to: %s \n", WIFI_NETWORK);
      Serial.printf("Local IP: %s \n", WiFi.localIP().toString());
   }

}

LiquidCrystal lcd(19, 21, 2, 4, 5, 18);
DHTesp dht;
WiFiClient client;
void setup() {
  Serial.begin(115200);
  lcd.begin(16,2);
  lcd.write("Aloha");
  int dhtPin = 23;
  dht.setup(dhtPin, DHTesp::DHT11);
  connectToWifi();
  ThingSpeak.begin(client);
}

void loop() {
  TempAndHumidity lastValues = dht.getTempAndHumidity();

  char templateTempFrase[] = " T: %.1f%cC";
  char templateRHFrase[] = "RH:  %.1f%%";
  char T[16];
  char RH[16];
  sprintf(T, templateTempFrase,lastValues.temperature,(char)0b11011111);
  sprintf(RH, templateRHFrase,lastValues.humidity);
  lcd.setCursor(0,0);
  lcd.write(T);
  lcd.setCursor(0,1);
  lcd.write(RH);

  ThingSpeak.setField(1, lastValues.temperature);
  ThingSpeak.setField(2, lastValues.humidity);

  // Aqui é onde nós fazemos a HTTP call, uma só para todos os
  // dados a serem escritos nos gráficos do ThingSpeak
  ThingSpeak.writeFields(CHANNEL_ID, CHANNEL_API_KEY);
  
  delay(10000);
}
