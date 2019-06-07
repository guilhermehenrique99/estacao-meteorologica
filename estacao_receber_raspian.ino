#include <WiFi.h>                                                   //Biblioteca responsável pelo envio
#include <SPI.h>             
#include <LoRa.h>                                                  //Biblioteca responsavel pelo LoRa
#include <HTTPClient.h>
#ifdef ARDUINO_SAMD_MKRWAN1300
#error This example is not compatible with the Arduino MKR WAN 1300 board!
#endif
                                                                  //Ativação dos pinos
const int LORA_SCK_PIN = 5;
const int LORA_MISO_PIN = 19;
const int LORA_MOSI_PIN = 27;
const int LORA_SS_PIN = 18;
const int LORA_RST_PIN = 15;
const int LORA_DI00_PIN = 26;
const int BAND = 915E6;
                                                                  //WiFi - Coloque aqui suas configura��es de WI-FI
const char ssid[] = "EstaçãoMeteorológica";
const char password[] = "12345678";
                                                                  //Site remoto - Coloque aqui os dados do site que vai receber a requisi��o GET
const char http_site[] = "http://10.42.0.1/";
const int http_port = 80;
String site;
                                                                  // Variaveis globais
HTTPClient http;
WiFiClient client;         
IPAddress server(10, 42, 0, 1);                                  // Endereço IP do servidor - http_site
String incoming;
int trava = 0;
void setup() {
  Serial.begin(9600);
  while (!Serial);
  Serial.println("Acessando o WIFI");
  //---------------------------------------------------------------------
  Serial.println("Conectando a ");
  Serial.println(ssid);
  delay(100);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(100);
  Serial.print("Aguarde ");
  int ultimoTempoCon = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("#");
    if (millis() - ultimoTempoCon > 10000) {
      ESP.restart();
    }
  }
  Serial.println("LoRa Receiver Callback");
  if (!loraBegin()) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
                                                                // Registre o retorno de chamada de recebimento
  LoRa.onReceive(onReceive);
                                                               //  Coloque o rádio no modo de recepção
  LoRa.receive();
}
bool loraBegin()
{
                                                                //Iniciamos a comunica��o SPI
  SPI.begin(LORA_SCK_PIN, LORA_MISO_PIN, LORA_MOSI_PIN, LORA_SS_PIN);
                                                                //Setamos os pinos do lora
  LoRa.setPins(LORA_SS_PIN, LORA_RST_PIN, LORA_DI00_PIN);
                                                                //Iniciamos o lora
  return LoRa.begin(915E6);
}                                                               //Identificação se houver travamento na mensagem
void loop() {
  if (trava == 1) {
    php();
    trava = 0;
  }
}
void onReceive(int packetSize) {
 
  if (packetSize == 0) return;                                   // Se nenhuma mesnagem foi recebida, retorna nada
  byte incomingLength = LoRa.read();                            // Tamanho da mensagem
  while (LoRa.available())
  {
    incoming += (char)LoRa.read();
  }
  if (incomingLength != incoming.length())
  {
    Serial.println("erro! o tamanho da mensagem nao condiz com o conteudo!");
    return;
  }
  Serial.println("Tamanho da mensagem  " + String(incomingLength));
  Serial.println("Mensagem"  + String(incoming));
  Serial.println();
  trava = 1;
}
void php() {
  site = (String(http_site) + "insert.php");                   //Envio ao site 
  site += incoming;
  Serial.println(site);
  http.begin(site);                                           // Inicio da conexão
  int httpCode = http.GET();
  http.end();                                                 //Final da 
  incoming = "";
}
