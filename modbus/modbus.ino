#include "ESP8266WiFi.h"

const char* ssid = "TP-LINK_B95E";
const char* password = "peitodemacaco";
int ModbusTCP_port = 502;
int dsm = 502;
float dsmt ;
float dsmh ;
String Message ;
String Message1 ;
 
//////// Required for Modbus TCP / IP /// 
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
//////////////////////////////////////////////////////////////////////////


#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN            2        // Pin which is connected to the DHT sensor.
#define DHTTYPE           DHT11     // DHT 22 (AM2302)
DHT_Unified dht(DHTPIN, DHTTYPE);


//////////////////////////////////////////////////////////////////////////

//#include <Wire.h>  // Only needed for Arduino 1.6.5 and earlier
//#include "SSD1306.h" // alias for `#include "SSD1306Wire.h"`
//#include "images.h"
//SSD1306  display(0x3c, D3, D5);
//#define DEMO_DURATION 3000
//typedef void (*Demo)(void);
//int demoMode = 0;
//int counter = 1;
//////////////////////////////////////////////////////////////////////////


void setup() {

 //pinMode(D2, OUTPUT);
 //pinMode(D4, OUTPUT);
 //digitalWrite(D2, HIGH );
 //digitalWrite(D4, HIGH );
 
 
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
 Serial.println("DHTxx Unified Sensor Example");
 // Print temperature sensor details.
 sensor_t sensor;
 dht.temperature().getSensor(&sensor);
 Serial.println("------------------------------------");
 Serial.println("Temperature");
 Serial.print  ("Sensor:       "); Serial.println(sensor.name);
 Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
 Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
 Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" *C");
 Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" *C");
 Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" *C");  
 Serial.println("------------------------------------");
 // Print humidity sensor details.
 dht.humidity().getSensor(&sensor);
 Serial.println("------------------------------------");
 Serial.println("Humidity");
 Serial.print  ("Sensor:       "); Serial.println(sensor.name);
 Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
 Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
 Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println("%");
 Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println("%");
 Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println("%");  
 Serial.println("------------------------------------");
 dsmt = 0 ;
 dsm = 0 ;
 //display.init();
 //display.flipScreenVertically();
 //display.setFont(ArialMT_Plain_10);
 
}

//void drawFontFaceDemo() {
//    display.setTextAlignment(TEXT_ALIGN_LEFT);
 //   display.setFont(ArialMT_Plain_16);
//    Message = "Humidity: " + String(dsmh) + "%";
//    display.drawString(0, 0, Message);
//    display.setFont(ArialMT_Plain_16);
//    Message1 = "Temp: " + String(dsmt) + "C";
//    display.drawString(0, 16, Message1);
//    display.setFont(ArialMT_Plain_24);
//    display.drawString(0, 26, "Hello world");
//}

//Demo demos[] = {drawFontFaceDemo}; //, drawTextFlowDemo, drawTextAlignmentDemo, drawRectDemo, drawCircleDemo, drawProgressBarDemo, drawImageDemo};
//int demoLength = (sizeof(demos) / sizeof(Demo));
//long timeSinceLastModeSwitch = 0;

 
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
 
 
 
///// code here --
 
 ///////// Holding Register [0] A [9] = 10 Holding Registers Writing

 dsm++;
 //Serial.println(dsm);
 if ( dsm >= 30 ){
  sensors_event_t event;  
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println("Error reading temperature!");
  }
  else {
    dsmt = event.temperature ;
  }
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println("Error reading humidity!");
  }
  else {
    dsmh = event.relative_humidity ;
  }
  dsm = 0 ;
  //display.clear();
  // draw the current demo method
  //demos[demoMode]();
  //display.setTextAlignment(TEXT_ALIGN_RIGHT);
  //display.drawString(10, 128, String(millis()));
  // write the buffer to the display
  //display.display();
 }

 
  union {
    float a;
    signed char bytes[4];
  } TT;

  union {
    float a;
    signed char bytes[4];
  } HT;
  
  TT.a = dsmt ;
  HT.a = dsmh ;
  
 
 
 MBHoldingRegister[0] = random(0,12);
 MBHoldingRegister[1] = random(0,12);


 
 MBHoldingRegister[2] = HT.bytes[0];
 MBHoldingRegister[3] = HT.bytes[1];
 MBHoldingRegister[4] = HT.bytes[2];
 MBHoldingRegister[5] = HT.bytes[3]; 
 MBHoldingRegister[6] = TT.bytes[0];
 MBHoldingRegister[7] = TT.bytes[1];
 MBHoldingRegister[8] = TT.bytes[2];
 MBHoldingRegister[9] = TT.bytes[3];
 
 

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
// digitalWrite(D2, MBHoldingRegister[12] );
// digitalWrite(D4, MBHoldingRegister[16] );
// Serial.println("");
// Serial.print("Read Digital Input : ");
// Serial.print(digitalRead(D7));
// Serial.println("");
 
 //// debug
 
 //for (int i = 0; i < 10; i++) {
 
 //Serial.print("[");
 //Serial.print(i);
 //Serial.print("] ");
 //Serial.print(Temporal[i]);
 
// }
 //Serial.println("");
 
 
 
 
//// end code - fin 
 
 
 //// rutine Modbus TCP
 byteFN = ByteArray[MB_TCP_FUNC];
 //Serial.println(byteFN);
 Start = word(ByteArray[MB_TCP_REGISTER_START],ByteArray[MB_TCP_REGISTER_START+1]);
 //Serial.println(Start);
 WordDataLength = word(ByteArray[MB_TCP_REGISTER_NUMBER],ByteArray[MB_TCP_REGISTER_NUMBER+1]);
 //Serial.println(WordDataLength);
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
