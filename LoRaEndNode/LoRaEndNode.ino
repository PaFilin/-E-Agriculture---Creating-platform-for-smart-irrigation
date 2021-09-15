#include <SPI.h>
#include <LoRa.h>
#include <Wire.h> 
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <driver/adc.h>


#define ID  228      // Node ID

#define SCK     5    // GPIO5  -- SX1278's SCK
#define MISO    19   // GPIO19 -- SX1278's MISnO
#define MOSI    27   // GPIO27 -- SX1278's MOSI
#define SS      18   // GPIO18 -- SX1278's CS
#define RST     14   // GPIO14 -- SX1278's RESET
#define DI0     26   // GPIO26 -- SX1278s IRQ(Interrupt Request)
#define RXPin   34    // GPIO34  -- GPS RX
#define TXPin   12    // GPIO12  -- GPS TX
#define MoistPin 0    // GPIO0 -- Moisture Sensor
#define ONE_WIRE_BUS 2 // One Wire bus pin GPIO 2 for DS18B20 Temperature sensor

#define GPSBaud   9600    
#define BAND  868100000

const int AirValue = 3400;   
const int WaterValue = 1600; 

int soilMoistureValue = 0;
int soilmoisturepercent=0;


OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);



void setup() {
  pinMode(16,OUTPUT);
  pinMode(2,OUTPUT);
  pinMode(0,INPUT);

  adc1_config_width(ADC_WIDTH_12Bit);
  adc1_config_channel_atten(ADC1_CHANNEL_6, ADC_ATTEN_DB_11);
  
  digitalWrite(16, LOW);    // set GPIO16 low to reset OLED
  delay(50); 
  digitalWrite(16, HIGH); // while OLED is running, must set GPIO16 in high
  
  Serial.begin(115200);
  ss.begin(GPSBaud);

  Serial.print(F("Testing TinyGPS++ library v. ")); 
  Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();
  
  while (!Serial);
  Serial.println();
  Serial.println("LoRa Sender Test");
  

  LoRa.setPins(SS,RST,DI0);
  LoRa.setSpreadingFactor(10);
  if (!LoRa.begin(BAND)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  LoRa.enableCrc();
  Serial.println("init ok");
  
  sendData();
   
  delay(1500);
}

void loop() {

  
  
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayInfo();
      //delay(3000);

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }

  
}


void sendData()
{
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(ID);
  LoRa.endPacket();
}




void displayInfo()
{
  LoRa.beginPacket();
  LoRa.print(ID); // 1
  LoRa.print(":");
  //Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  { 
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);

    LoRa.print(gps.location.lat(), 6);// 2
    LoRa.print(":");
    LoRa.print(gps.location.lng(), 6);// 3
    LoRa.print(":");

    
  }
  else
  {
    Serial.print(F("INVALID"));
    LoRa.print("NA:NA:");
  }

//  Serial.print(F("  Date/Time: "));
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());

    LoRa.print(gps.date.month()); // 4
    LoRa.print(":");
    LoRa.print(gps.date.day()); // 5
    LoRa.print(":");
    LoRa.print(gps.date.year()); // 6
    LoRa.print(":");
  }
  else
  {
    LoRa.print(F("NA:NA:NA:"));
  }
  Serial.print(F(" "));
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    LoRa.print(gps.time.hour());// 7
    LoRa.print(":");
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    LoRa.print(gps.time.minute());// 8
    LoRa.print(":");
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    LoRa.print(gps.time.second()); // 9
    LoRa.print(":");
    Serial.print(F("."));
  }
  else
  {
    Serial.print(F("INVALID"));
    LoRa.print(F("NA:NA:NA:"));
  }

  soilMoistureValue = analogRead(MoistPin);
  Serial.println();
  
  Serial.println(soilMoistureValue);
  soilmoisturepercent = map(soilMoistureValue, AirValue, WaterValue, 0, 100);
  
  if(soilmoisturepercent >= 100)
  {
    Serial.println("100 %");
    LoRa.print(100); // 10
    LoRa.print(":");
  }
  else if(soilmoisturepercent <= 0)
  {
    Serial.println("0 %");
    LoRa.print(0); // 10
    LoRa.print(":");
  }
  else if(soilmoisturepercent >0 && soilmoisturepercent < 100)
  {
    Serial.print(soilmoisturepercent);
    Serial.println("%");
    LoRa.print(soilmoisturepercent); // 10
    LoRa.print(":");
  }
  
  sensors.requestTemperatures();
  Serial.println("Temperature is ");
  Serial.print(sensors.getTempCByIndex(0));
  LoRa.print(sensors.getTempCByIndex(0)); 
  
  LoRa.endPacket();
  Serial.println();
  delay(2000);
}
