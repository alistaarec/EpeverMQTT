#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWiFiManager.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <SoftwareSerial.h>
#include <ModbusMaster.h>
#include <AsyncElegantOTA.h>
#include <ESPAsyncTCP.h>
#include <bms2.h>
#include <uptime_formatter.h>
#include <PubSubClient.h>


#include "index.h" //Web on ESP
#include "epregister.h" //Modbus 

char mqtt_server[20];
char mqtt_port[6];
char mqtt_user[20];
char mqtt_pass[20];

//callback to save config
bool shsaveConf = false;

void shsaveConfCallback () {
  Serial.println("Debug : Should save config");
  shsaveConf = true;
}

//MQTT client
WiFiClient espWiFi;
PubSubClient mqtt(espWiFi);

//Async Server
AsyncWebServer server(80);
DNSServer dns;

//SWSerial
SoftwareSerial epSerial (14, 13); //D1 RX, D2 TX
SoftwareSerial bmsSerial (5, 4); //SD3 RX, SD2 TX

//Modbus
ModbusMaster epModbus;

//BMS
OverkillSolarBms2 bms = OverkillSolarBms2();


#define EP_BAUD 115200
#define EP_ADDR 1
#define EP485RW 12 //D6
#define BMS_BAUD 9600

String solarV, solarA, solarW, batV, batA, batW, dcoutV, dcoutA, dcoutW,
      acoutV, acoutW, mpptTemp, energyDay, energyTotal;
String bmsV, bmsA, bmsSOC, bmsCap, bmsCycle, bmsTemp0, bmsTemp1, bmsTemp2,
      cell1, cell2, cell3, cell4, cell5, cell6, cell7, cell8;
String uptime;

unsigned long read2loop = 5000;
unsigned long read2loopOld = 0;

unsigned long read10loop = 10000;
unsigned long read10loopOld = 0;

unsigned long read60loop = 20000;
unsigned long read60loopOld = 0;

long lastRecAtt = 0;



//------------------VOID--------------//
void preTransmission()
{
  digitalWrite(EP485RW, HIGH);
}

void postTransmission()
{
  digitalWrite(EP485RW, LOW);
}


//-----------------END-VOID----------//

void setup() {

Serial.begin(9600);
delay(5);
Serial.println();
pinMode(EP485RW, OUTPUT);

//-----------SPIFFS AND WIFI MANAGER-------------------------//
//Format FS debug
//SPIFFS.format();

//read configuration from FS json
Serial.println("Debug : mounting FS...");

if (SPIFFS.begin()) {
  Serial.println("Debug : mounted file system");
  if (SPIFFS.exists("/config.json")) {
    //file exists, reading and loading
    Serial.println("Debug : reading config file");
    File configFile = SPIFFS.open("/config.json", "r");
    if (configFile) {
      Serial.println("Debug : opened config file");
      size_t size = configFile.size();
      // Allocate a buffer to store contents of the file.
      std::unique_ptr<char[]> buf(new char[size]);

      configFile.readBytes(buf.get(), size);
      DynamicJsonDocument doc(1024);
      auto error = deserializeJson(doc, buf.get());
      if (error) {
        Serial.print(F("deserializeJson() failed with code "));
        Serial.println(error.c_str());
      } else {

        Serial.println("\nparsed json");

        strcpy(mqtt_server, doc["mqtt_server"]);
        strcpy(mqtt_port, doc["mqtt_port"]);
        strcpy(mqtt_user, doc["mqtt_user"]);
        strcpy(mqtt_pass, doc["mqtt_pass"]);
      }
    }
  }
}

AsyncWiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 20);
AsyncWiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
AsyncWiFiManagerParameter custom_mqtt_user("username", "mqtt username", mqtt_user, 20);
AsyncWiFiManagerParameter custom_mqtt_pass("password", "mqtt password", mqtt_pass, 20);

//WiFiManager
//Local intialization. Once its business is done, there is no need to keep it around
AsyncWiFiManager wifiManager(&server,&dns);

//set config save notify callback
wifiManager.setSaveConfigCallback(shsaveConfCallback);

//add all your parameters here
wifiManager.addParameter(&custom_mqtt_server);
wifiManager.addParameter(&custom_mqtt_port);
wifiManager.addParameter(&custom_mqtt_user);
wifiManager.addParameter(&custom_mqtt_pass);

if (!wifiManager.autoConnect("EPEVER")) {
  Serial.println("Debug : failed to connect and hit timeout");
  delay(3000);
  //reset and try again, or maybe put it to deep sleep
  ESP.reset();
  delay(5000);
}

//if you get here you have connected to the WiFi
Serial.println("Debug : connected...yeey :)");

//read updated parameters
strcpy(mqtt_server, custom_mqtt_server.getValue());
strcpy(mqtt_port, custom_mqtt_port.getValue());
strcpy(mqtt_user, custom_mqtt_user.getValue());
strcpy(mqtt_pass, custom_mqtt_pass.getValue());

//save the custom parameters to FS
if (shsaveConf) {
  Serial.println("Debug : saving config");
  DynamicJsonDocument doc(1024);
  serializeJson(doc, Serial);
  doc["mqtt_server"] = mqtt_server;
  doc["mqtt_port"] = mqtt_port;
  doc["mqtt_user"] = mqtt_user;
  doc["mqtt_pass"] = mqtt_pass;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("Debug : failed to open config file for writing");
  }

  serializeJson(doc, Serial);
  serializeJson(doc, configFile);
  configFile.close();
  //end save
  }

  Serial.println("Debug : local ip");
  Serial.println(WiFi.localIP());


  Serial.print("mqtt server: "); Serial.println(mqtt_server);
  Serial.print("mqtt port: "); Serial.println(mqtt_port);
  Serial.print("mqtt username: "); Serial.println(mqtt_user);
  Serial.print("mqtt password: "); Serial.println(mqtt_pass);


//--------------------------END-------------------------------------------//

  epSerial.begin(EP_BAUD);
  delay(10);
  epModbus.begin(EP_ADDR, epSerial);
  delay(10);
  epModbus.preTransmission(preTransmission);
  epModbus.postTransmission(postTransmission);
  
  bmsSerial.begin(BMS_BAUD);
  delay(10);
  bms.begin(&bmsSerial);

    while(1) {
      bms.main_task(true);
      if (millis() >= 5000) {
        break;
      }
      delay(10);
    }



  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                AsyncResponseStream *response = request->beginResponseStream("text/html");
                response->printf_P(HTML_HEAD);
                response->printf_P(HTML_MAIN);
                response->printf_P(HTML_FOOT);
                request->send(response);
              });
  
  server.on("/livedata", HTTP_GET, [](AsyncWebServerRequest *request)
              {
                AsyncResponseStream *response = request->beginResponseStream("application/json");
                DynamicJsonDocument liveJson(1024);
                liveJson["solarV"] = solarV;
                liveJson["solarA"] = solarA;
                liveJson["solarW"] = solarW;
                liveJson["batV"] = batV;
                liveJson["batA"] = batA;
                liveJson["batW"] = batW;
                liveJson["dcoutV"] = dcoutV;
                liveJson["dcoutA"] = dcoutA;
                liveJson["dcoutW"] = dcoutW;
                liveJson["acoutV"] = acoutV;
                liveJson["acoutW"] = acoutW;
                liveJson["mpptTemp"] = mpptTemp;
                liveJson["bmsV"] = bmsV;
                liveJson["bmsA"] = bmsA;
                liveJson["bmsSOC"] = bmsSOC;
                liveJson["bmsCap"] = bmsCap;
                liveJson["bmsCycle"] = bmsCycle;
                liveJson["cell1"] = cell1;
                liveJson["cell2"] = cell2;
                liveJson["cell3"] = cell3;
                liveJson["cell4"] = cell4;
                liveJson["cell5"] = cell5;
                liveJson["cell6"] = cell6;
                liveJson["cell7"] = cell7;
                liveJson["cell8"] = cell8;
                liveJson["bmsTemp0"] = bmsTemp0;
                liveJson["bmsTemp1"] = bmsTemp1;
                liveJson["bmsTemp2"] = bmsTemp2;  
                liveJson["energyDay"] = energyDay;
                liveJson["energyTotal"] = energyTotal;
                liveJson["uptime"] = uptime;             
                serializeJson(liveJson, *response);
                request->send(response);
              });

  AsyncElegantOTA.begin(&server); 

  server.begin();
  delay(20);
  read2loop = millis();

  mqtt.setServer(mqtt_server, 1883);
    while (!mqtt.connected()) {
    if (mqtt.connect("Alis_FVE", mqtt_user, mqtt_pass)) {
      Serial.println("Connected to MQTT server");
    } else {
      Serial.print("Failed to connect to MQTT server, rc=");
      Serial.print(mqtt.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}


//MQTT Reconnect void
void mqttreconnect(){
  if (mqtt.connect("Alis_FVE", mqtt_user, mqtt_pass)) {
      Serial.println("Connected to MQTT server");
      lastRecAtt = 0;
  }
}

void readEPliveData() {
  epModbus.clearResponseBuffer();
  uint8_t result;

  result = epModbus.readInputRegisters(LIVE_DATA, LIVE_DATA_CNT);
  if (result == epModbus.ku8MBSuccess) { 

    solarV = (epModbus.getResponseBuffer(0x00) / 100.f);
    solarA = (epModbus.getResponseBuffer(0x01) / 100.f);
    solarW = (epModbus.getResponseBuffer(0x02) | epModbus.getResponseBuffer(0x03) << 16) / 100.f;

    batV = (epModbus.getResponseBuffer(0x04) / 100.f);
    batA = (epModbus.getResponseBuffer(0x05) / 100.f);
    batW = (epModbus.getResponseBuffer(0x06) | epModbus.getResponseBuffer(0x07) << 16) / 100.f;


    dcoutV = (epModbus.getResponseBuffer(0x0C) / 100.f);
    dcoutA = (epModbus.getResponseBuffer(0x0D) / 100.f);
    dcoutW = (epModbus.getResponseBuffer(0x0E) | epModbus.getResponseBuffer(0x0F) << 16) / 100.f;


  }
  Serial.println(result);
  Serial.print(solarV);
  Serial.print(solarA);
  Serial.print(solarW);
  Serial.print(batV);
  Serial.print(batA);
  Serial.print(batW);
}

void readEPTempData() {
  epModbus.clearResponseBuffer();
  uint8_t result;

  result = epModbus.readInputRegisters(DEVICE_TEMPERATURE, 1);
  if (result == epModbus.ku8MBSuccess) { 

    mpptTemp = (epModbus.getResponseBuffer(0x00) / 100.f);

  }

}

void readEPEnergyData() {
  epModbus.clearResponseBuffer();
  uint8_t result;

  result = epModbus.readInputRegisters(STATISTICS, STATISTICS_CNT);
  if (result == epModbus.ku8MBSuccess) {

    energyDay = (epModbus.getResponseBuffer(0x0C) | epModbus.getResponseBuffer(0x0D) << 16) / 100.f;
    energyTotal = (epModbus.getResponseBuffer(0x12) | epModbus.getResponseBuffer(0x13) << 16) / 100.f;

  }
}

void bmsLiveData() {
  bms.query_0x03_basic_info();
  bmsV = bms.get_voltage();
  bmsA = bms.get_current();
  bmsCap = bms.get_balance_capacity();
  bmsCycle = bms.get_cycle_count();
  bmsSOC = bms.get_state_of_charge();
  bmsTemp0 = bms.get_ntc_temperature(0);
  bmsTemp1 = bms.get_ntc_temperature(1);
  bmsTemp2 = bms.get_ntc_temperature(2);

}

void bmsCellData() {
  bms.query_0x04_cell_voltages();
  cell1 = bms.get_cell_voltage(0);
  cell2 = bms.get_cell_voltage(1);
  cell3 = bms.get_cell_voltage(2);
  cell4 = bms.get_cell_voltage(3);
  cell5 = bms.get_cell_voltage(4);
  cell6 = bms.get_cell_voltage(5);
  cell7 = bms.get_cell_voltage(6);
  cell8 = bms.get_cell_voltage(7);

  }

  void mqttupdate() {
  mqtt.publish("FVE/solarV",      solarV.c_str());
  mqtt.publish("FVE/solarA",      solarA.c_str());
  mqtt.publish("FVE/solarW",      solarW.c_str());
  mqtt.publish("FVE/mpptTemp",    mpptTemp.c_str());
  mqtt.publish("FVE/energyTotal", energyTotal.c_str());
  if(bmsV != 0){
    mqtt.publish("FVE/bmsV",        bmsV.c_str());
    mqtt.publish("FVE/bmsA",        bmsA.c_str());
    mqtt.publish("FVE/bmsCap",      bmsCap.c_str());
    mqtt.publish("FVE/bmsCycle",    bmsCycle.c_str());
    mqtt.publish("FVE/bmsSOC",      bmsSOC.c_str());
  }
  
  if(cell1 != 0) {
    mqtt.publish("FVE/cell1",       cell1.c_str());
    mqtt.publish("FVE/cell2",       cell2.c_str());
    mqtt.publish("FVE/cell3",       cell3.c_str());
    mqtt.publish("FVE/cell4",       cell4.c_str());
    mqtt.publish("FVE/cell5",       cell5.c_str());
    mqtt.publish("FVE/cell6",       cell6.c_str());
    mqtt.publish("FVE/cell7",       cell7.c_str());
    mqtt.publish("FVE/cell8",       cell8.c_str());
  }

}

void loop() {


  if (millis() - read2loopOld > read2loop) {   //Every 2 sec
    readEPliveData();
    bmsLiveData();
    bmsCellData();
    uptime = uptime_formatter::getUptime();
    readEPTempData();
    read2loopOld = millis();
  }

  if(millis() - read10loopOld > read10loop) {  //Every 10 sec
    
    

    if (mqtt.connected()) {
        mqttupdate();
        } else {
          long now = millis();
          if(now - lastRecAtt > 10000) {
            lastRecAtt = now;
            mqttreconnect();
            }

    
    }
    read10loopOld = millis();
  }

  if(millis() - read60loopOld > read60loop) {
    readEPEnergyData();
    read60loopOld = millis();
  }
}