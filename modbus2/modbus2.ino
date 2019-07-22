
#include <ESP8266WiFi.h> 
#include "DHT.h"

int sensorValue = 0;  // value read from the pot
const int analogInPin = A0;
 
const char* ssid = "TP-LINK_B95E";
const char* password = "peitodemacaco";
int ModbusTCP_port = 502;


#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

 
//////// Required for Modbus TCP / IP /// Requerido para Modbus TCP/IP /////////
#define maxInputRegister 20
#define maxHoldingRegister 20
 
#define MB_FC_NONE 0
#define MB_FC_READ_REGISTERS 3 //implemented
#define MB_FC_WRITE_REGISTER 6 //implemented
#define MB_FC_WRITE_MULTIPLE_REGISTERS 16 //implemented
//
// MODBUS Error Codes
//
#define MB_EC_NONE 0
#define MB_EC_ILLEGAL_FUNCTION 1
#define MB_EC_ILLEGAL_DATA_ADDRESS 2
#define MB_EC_ILLEGAL_DATA_VALUE 3
#define MB_EC_SLAVE_DEVICE_FAILURE 4
//
// MODBUS MBAP offsets
//
#define MB_TCP_TID 0
#define MB_TCP_PID 2
#define MB_TCP_LEN 4
#define MB_TCP_UID 6
#define MB_TCP_FUNC 7
#define MB_TCP_REGISTER_START 8
#define MB_TCP_REGISTER_NUMBER 10
 
byte ByteArray[260];
unsigned int MBHoldingRegister[maxHoldingRegister];
 
//////////////////////////////////////////////////////////////////////////
 
WiFiServer MBServer(ModbusTCP_port);
 
void setup() {
 
 //pinMode(14, OUTPUT);
 
 Serial.begin(9600);
 delay(100) ;
 WiFi.begin(ssid, password);
 delay(100) ;
 Serial.println(".");
 while (WiFi.status() != WL_CONNECTED) {
 delay(500);
 Serial.print(".");
 }
 MBServer.begin();
 Serial.println("Connected ");
 Serial.print("ESP8266 Slave Modbus TCP/IP ");
 Serial.print(WiFi.localIP());
 Serial.print(":");
 Serial.println(String(ModbusTCP_port));
 Serial.println("Modbus TCP/IP Online");

  dht.begin();

 
}
 
 
void loop() {
 
 
 // Check if a client has connected // Modbus TCP/IP
 WiFiClient client = MBServer.available();
 if (!client) {
 return;
 }
 
 
 boolean flagClientConnected = 0;
 byte byteFN = MB_FC_NONE;
 int Start;
 int WordDataLength;
 int ByteDataLength;
 int MessageLength;
 
 // Modbus TCP/IP
 while (client.connected()) {
 
 if(client.available())
 {
 flagClientConnected = 1;
 int i = 0;
 while(client.available())
 {
 ByteArray[i] = client.read();
 i++;
 }
 
 client.flush();
 
 ///////// Holding Register [0] A [9] = 10 Holding Registers Escritura
 ///////// Holding Register [0] A [9] = 10 Holding Registers Writing

sensorValue = analogRead(analogInPin);

float h = dht.readHumidity();
float t = dht.readTemperature();

 
 MBHoldingRegister[0] = h*10;
 MBHoldingRegister[1] = t*10;
 MBHoldingRegister[2] = sensorValue;
 MBHoldingRegister[3] = 4;
 MBHoldingRegister[4] = 5;
 MBHoldingRegister[5] = 6;
 MBHoldingRegister[6] = 7;
 MBHoldingRegister[7] = 8;
 MBHoldingRegister[8] = 9;
 MBHoldingRegister[9] = 10;
 
 
 
 
 ///////// Holding Register [10] A [19] = 10 Holding Registers Lectura
 ///// Holding Register [10] A [19] = 10 Holding Registers Reading
 
 int Temporal[10];
 
 Temporal[0] = MBHoldingRegister[10];
 Temporal[1] = MBHoldingRegister[11];
 Temporal[2] = MBHoldingRegister[12];
 Temporal[3] = MBHoldingRegister[13];
 Temporal[4] = MBHoldingRegister[14];
 Temporal[5] = MBHoldingRegister[15];
 Temporal[6] = MBHoldingRegister[16];
 Temporal[7] = MBHoldingRegister[17];
 Temporal[8] = MBHoldingRegister[18];
 Temporal[9] = MBHoldingRegister[19];
 
 /// Enable Output 14
// digitalWrite(14, MBHoldingRegister[14] );
 
 
 //// debug
 
 for (int i = 0; i < 10; i++) {
 
 Serial.print("[");
 Serial.print(i);
 Serial.print("] ");
 Serial.print(Temporal[i]);
 
 }
 Serial.println("");
 
 
 //// rutine Modbus TCP
 byteFN = ByteArray[MB_TCP_FUNC];
 Start = word(ByteArray[MB_TCP_REGISTER_START],ByteArray[MB_TCP_REGISTER_START+1]);
 WordDataLength = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
 }
 
 // Handle request
 
 switch(byteFN) {
 case MB_FC_NONE:
 break;
 
 case MB_FC_READ_REGISTERS: // 03 Read Holding Registers
 ByteDataLength = WordDataLength * 2;
 ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
 ByteArray[8] = ByteDataLength; //Number of bytes after this one (or number of bytes of data).
 for(int i = 0; i < WordDataLength; i++)
 {
 ByteArray[ 9 + i * 2] = highByte(MBHoldingRegister[Start + i]);
 ByteArray[10 + i * 2] = lowByte(MBHoldingRegister[Start + i]);
 }
 MessageLength = ByteDataLength + 9;
 client.write((const uint8_t *)ByteArray,MessageLength);
 
 byteFN = MB_FC_NONE;
 
 break;
 
 
 case MB_FC_WRITE_REGISTER: // 06 Write Holding Register
 MBHoldingRegister[Start] = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
 ByteArray[5] = 6; //Number of bytes after this one.
 MessageLength = 12;
 client.write((const uint8_t *)ByteArray,MessageLength);
 byteFN = MB_FC_NONE;
 break;
 
 case MB_FC_WRITE_MULTIPLE_REGISTERS: //16 Write Holding Registers
 ByteDataLength = WordDataLength * 2;
 ByteArray[5] = ByteDataLength + 3; //Number of bytes after this one.
 for(int i = 0; i < WordDataLength; i++)
 {
 MBHoldingRegister[Start + i] = word(ByteArray[ 13 + i * 2],ByteArray[14 + i * 2]);
 }
 MessageLength = 12;
 client.write((const uint8_t *)ByteArray,MessageLength); 
 byteFN = MB_FC_NONE;
 
 break;
 }
 }
 
 
 
 
}
