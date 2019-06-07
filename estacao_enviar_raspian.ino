#include "SimpleDHT.h"        //Biblioteca responsável pelo sensor DHT22
#include <SPI.h>              // 
#include <LoRa.h>             //Biblioteca responsável pelo LoRa
                              //Ativação dos Pinos do LoRa
const int LORA_SCK_PIN = 5;
const int LORA_MISO_PIN = 19;
const int LORA_MOSI_PIN = 27;
const int LORA_SS_PIN = 18;
const int LORA_RST_PIN = 15;
const int LORA_DI00_PIN = 26;
const int BAND = 915E6;
byte msgCount = 0;            // Mensagens de saida
byte localAddress = 0xFF;     // Endereço do dispositivo
byte destination = 0xBB;      // Destino para enviar
long int lastSendTime = 0;    // Última hora de envio
long int tempo=0;
long int tempo1=0;
double mes = 0; 
String param;
const int REED = 12;          //O interruptor reed é enviado para o pino digital 12
int val = 0;                  
int old_val = 0;            
int REEDCOUNT = 0;            //Esta é a variável que contém a contagem do pluviometro
 
SimpleDHT22 dht22(25);        // Atribuindo o pino para o DHT22
void setup() {
 pinMode (REED, INPUT_PULLUP);//Ativação do Pluviometro
                              // initializa a comunicaçao serial:
   Serial.begin(9600);
   if (!loraBegin()) {         // Iniciando a frequência 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       
  }
   digitalWrite(2,HIGH);
}
                               //Função ativação LoRa
bool loraBegin()
{
                               // Iniciamos a comunicação SPI
  SPI.begin(LORA_SCK_PIN, LORA_MISO_PIN, LORA_MOSI_PIN, LORA_SS_PIN);
                               // Setamos os pinos do lora
  LoRa.setPins(LORA_SS_PIN, LORA_RST_PIN, LORA_DI00_PIN);
                               // Iniciamos o lora
  return LoRa.begin(BAND);     // retorna a função frequência

}

void loop() {
float u;                     // Variável de umidade 
float t;                     // Variável de temperatura  
dht22.read2(&t, &u, NULL);   //Leitura do sensor DHT22 
                             //Enviar os dados quando o pluviometro estiver sem chuvas
                             // Envio das informações captadas
if (millis() - lastSendTime > 1200000) {
param = "?chuva=" + String(mes) +"&temp=" + String(t) + "&humidade=" + String(u); // Envio das informações captadas
Serial.println("Envio sem dados");
Serial.println(param); 
sendMessage(param);
param = "";
lastSendTime= millis();
}
                              // ler o estado do switch pelo pino de entrada:
  val = digitalRead(REED);     //Leia o status do switch Reed
 if ((val == LOW) && (old_val == HIGH)){    //Verifique se o status foi alterado
   delay(10);                    
   REEDCOUNT = REEDCOUNT + 1;   //Adicionar 1 a contagem
   old_val = val;               //
       Serial.print("Medida de chuva (contagem): ");
       Serial.print(REEDCOUNT);//*0.2794); 
       Serial.println(" pulso");
       Serial.print("Medida de chuva (calculado): ");
       mes=REEDCOUNT*0.25;      //Cálculo do pulso 
       Serial.print(mes); 
       Serial.println(" m/m");     
                               //Variável responsável pelo envio dos dados
       param = "?chuva=" + String(mes) +"&temp=" + String(t) + "&humidade=" + String(u);
       
       Serial.println(param); 
       sendMessage(param);      // Enviar a mensagem para o receptor
       param = "";
       lastSendTime= millis() ;  
              tempo = millis();
       } 
     else {
         old_val = val;     
       }
 zerar();                       // chamando a função zerar
}
                                //Responsável por zerar a variável do pluviometro  a cada 20 min
void zerar(){
  if(millis() - tempo > 1800000 ){
  if (mes==0){
      return;
    }
  tempo=0;
  mes=0;
  REEDCOUNT=0;
  Serial.println("ZEROU");
 }
}
void sendMessage(String mes) {
  LoRa.beginPacket();                 // Iniciar pacote envio
  LoRa.write(mes.length());         // Tamanho da mensagem em bytes
  LoRa.print(mes);                             
  LoRa.endPacket();                 // Fim do pacote e enviando a mensagem
                            
}
