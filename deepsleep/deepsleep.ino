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
int sensorValue = 0; //pino pra tirar uma amostra de tensão da bateria

String valor = ""; 

float h=0;
float hfinal=0;
float t=0;
float tfinal=0;
float bateria=0;
int i=0;
int j=0;
 
void setup()
{
Serial.begin(74880);
Serial.println();

j=0;
hfinal=0;
tfinal=0;
bateria=0;

for(i=0;i<10;i++){
  do{
    if (j>100) {ESP.deepSleep(300e6);}//dorme o esp se 100 medidas foram erradas
    delay(100);
    h = dht.readHumidity();
    t = dht.readTemperature();
    sensorValue = analogRead(analogInPin);
    j++;
  }while((isnan(h) || isnan(t) || isnan(sensorValue)));//lê temperaturas
  
  hfinal=hfinal+h;
  tfinal=tfinal+t;
  bateria=bateria+sensorValue;
}

hfinal=hfinal/10;
tfinal=tfinal/10;
bateria=bateria/10;

valor=(String)"tensao="+bateria+"&temp="+tfinal+"&umi="+hfinal;//monta a string pro POST, no formato variavel1=valor1&variavel2=valor2 e assim por diante

//valor=(String)"tensao="+sensorValue+"&temp="+t+"&umi="+h;//monta a string pro POST, no formato variavel1=valor1&variavel2=valor2 e assim por diante




  Serial.printf("Connecting to wifi");
  WiFi.begin(ssid, password);
  i=0;
  while (WiFi.status() != WL_CONNECTED)
  {
    i++;
    delay(500);
    Serial.print(".");
    if (i>40) ESP.deepSleep(60e6);//dorme 1min se não conectar em até 20 seg
  }
  Serial.println(" connected");


HTTPClient http;
http.begin(host);
http.addHeader("Content-Type", "application/x-www-form-urlencoded");
http.POST(valor);
http.writeToStream(&Serial);
http.end();//posta e encerra a conexão

  
}
 
void loop() {
ESP.deepSleep(300e6);//bota o esp pra dormir, 300 segundos, 5 minutos
}
