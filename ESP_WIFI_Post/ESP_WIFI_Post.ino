#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

//For storing configurations
#include "FS.h"
#include <ArduinoJson.h>






char URL[60] = "tchululu"; //URL recebida pelo portal
char ID[5] = "tc";
// flag for saving data
bool shouldSaveConfig = false;
//callback notifying us of the need to save config

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);




  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(URL, json["URL"]);
          strcpy(ID, json["ID"]);
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read






  //WiFiManager
  //Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;
  wifiManager.setSaveConfigCallback(saveConfigCallback);//add isso, treta do json
  //wifiManager.setDebugOutput(true);
  //reset saved settings
  wifiManager.resetSettings();


  WiFiManagerParameter customURL("cURL", "URL", URL, 60);//parametro recebido na pagina, url
  WiFiManagerParameter customID("cID", "ID", ID, 5);//parametro recebido na pagina, id
  wifiManager.addParameter(&customURL);//adiciona
  wifiManager.addParameter(&customID);//adiciona


  //set custom ip for portal
  //wifiManager.setAPStaticIPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));


  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AcessPoint");
  //or use this for auto generated name ESP + ChipID
  //wifiManager.autoConnect();


  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");





//Serial.println(strcpy(URL, customURL.getValue())); 



//save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["URL"] = URL;
    json["ID"] = ID;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }




}




void loop() {
  // put your main code here, to run repeatedly:

Serial.println(URL); 
Serial.println(ID);


}
