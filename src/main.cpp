#include <wemos_lora.h>

const long freq = 868E6;
const int SF = 7;
const long bw = 125E3;

char value = '1';
bool transmit_done = false;
String tx_data = "{'name':'HA1BALLON','packetcnt':' '}";


#ifdef DEBUG_Local
#define DEBUGPRINT(x) Serial.print(x)
#define DEBUGPRINTARG(x,y) Serial.print(x,y)
#define DEBUGPRINTLN(x) Serial.println(x)
#define DEBUGPRINTLNARG(x,y) Serial.println(x, y)
#elif DEBUG_Remote
#define DEBUGPRINT(x) TelnetStream2.print(x)
#define DEBUGPRINTARG(x,y) TelnetStream2.print(x,y)
#define DEBUGPRINTLN(x) TelnetStream2.println(x)
#define DEBUGPRINTLNARG(x,y) TelnetStream2.println(x, y)
#else
#define DEBUGPRINT(x)
#define DEBUGPRINTARG(x,y)
#define DEBUGPRINTLN(x)
#define DEBUGPRINTLNARG(x,y)
#endif

void sendMessage(String message) {
  // DEBUGPRINTLN(message);
  // send packet
  LoRa.beginPacket();
  LoRa.print(message);
  LoRa.endPacket();
}

void readEEPROM_MAC(int deviceaddress, byte eeaddress)
{
  Wire.beginTransmission(deviceaddress);
  Wire.write(eeaddress); // LSB 
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress, 8); //request 8 bytes from the device

  while (Wire.available()){
    DEBUGPRINT("0x");
    DEBUGPRINTARG(Wire.read(), HEX);
    DEBUGPRINT(" ");
  }
  DEBUGPRINTLN();
}

void setup() {

    LoRa.setPins(16, 17, 15); // set CS, reset, IRQ pin
    //while (!Serial); // wait for Serial to be initialized
    Serial.begin(115200);
    delay(100);
    DEBUGPRINTLN(F("Starting"));
    setupOTA("Lora", mySSID, myPASSWORD);
    Serial.println(ArduinoOTA.getHostname());

    DEBUGPRINTLN(F("wire init..."));
    Wire.begin();
    DEBUGPRINTLN(F("done"));
    
        
    // Get Device Unique ID
    DEBUGPRINTLN(F("DeviceEUI:"));
    readEEPROM_MAC(0x50, 0xF8);  //0x50 is the I2c address, 0xF8 is the memory address where the read-only MAC value is
    
    //Initialize LoRa Module
    while (!LoRa.begin(freq)) {
      Serial.println("Starting LoRa failed!");
      while (1);
    }
    LoRa.setSpreadingFactor(SF);
    //  LoRa.setSignalBandwidth(bw);
    DEBUGPRINTLN(F("Lora module init Done"));

}

void loop() {
 unsigned long now;
 now = millis();
 ArduinoOTA.handle();
    if ((now & 1024) != 0) {
      if (transmit_done == false){
        DEBUGPRINT(F("Send: "));

        tx_data[33] = value;
        DEBUGPRINTLN(tx_data);
        char radiopacket[1] = {value};
        sendMessage(tx_data);
        value++;
        if (value > '9')
        value = 48;
        transmit_done = true;
      }
    }
    else {
      transmit_done = false;
    }

 ArduinoOTA.handle();
}