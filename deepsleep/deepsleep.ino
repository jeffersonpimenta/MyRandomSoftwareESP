#include <ESP8266WiFi.h> //biblioteca wifi 
#include <ESP8266HTTPClient.h> //biblioteca do http

#include "DHT.h" //biblioteca do dht11, adafruit

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE); //configuração do tipo de sensor

//#include <OneWire.h>
//#include <DallasTemperature.h> 
//const int oneWireBus = 4;  
//OneWire oneWire(oneWireBus);
//DallasTemperature sensors(&oneWire); //caso for usar o ds18b20, descomente


const char* ssid = "TP-LINK_B95E";
const char* password = "peitodemacaco"; //sua wifi e senha

const char* host = "http://192.168.1.236:8080/ScadaBR/httpds"; //use o ip do seu scadabr e a porta, mas mantenha o /httpds

const int analogInPin = A0;
float sensorValue = 0; //pino pra tirar uma amostra de tensão da bateria

String valor = ""; 

float h;
float t;

 
void setup()
{
  Serial.begin(74880);
  Serial.println();

  Serial.printf("Connecting to wifi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

}
 
void loop() {

HTTPClient http;
http.begin(host);
http.addHeader("Content-Type", "application/x-www-form-urlencoded");

//float temperatureC = sensors.getTempCByIndex(0);//leitura de temperatura do ds


do{
delay(100);
h = dht.readHumidity();
t = dht.readTemperature();
sensorValue = analogRead(analogInPin);
}while((isnan(h) || isnan(t) || isnan(sensorValue)));//lê temperaturas


valor=(String)"tensao="+sensorValue+"&temp="+t+"&umi="+h;//monta a string pro POST, no formato variavel1=valor1&variavel2=valor2 e assim por diante

http.POST(valor);
http.writeToStream(&Serial);
http.end();//posta e encerra a conexão

  
ESP.deepSleep(300e6);//bota o esp pra dormir, 300 segundos, 5 minutos
}
