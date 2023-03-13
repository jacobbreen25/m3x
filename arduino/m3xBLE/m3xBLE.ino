/*
* Name: m3xBLE
* Purpose: To allow control of the M3X arm robot via bluetooth
* Author: Jacob Breen
* Date Started: 3/8/23
* Date Last Worked on: 3/8/23
*
* Version: 00.00.01
*/

/*
X(e54aa130-bdbf-11ed-a901-0800200c9a66)
X(e54aa131-bdbf-11ed-a901-0800200c9a66)
X(e54aa132-bdbf-11ed-a901-0800200c9a66)
e54aa133-bdbf-11ed-a901-0800200c9a66
*/

#include <ArduinoBLE.h>
#include <String.h>

BLEService MessageService("e54aa130-bdbf-11ed-a901-0800200c9a66");

BLEStringCharacteristic commandData("e54aa131-bdbf-11ed-a901-0800200c9a66", BLERead | BLEWrite, 15);
BLEStringCharacteristic responseData("e54aa132-bdbf-11ed-a901-0800200c9a66", BLEWrite | BLENotify, 15);

bool status = false;

void setup() {
  Serial.begin(57600);
  if(!BLE.begin()){
    Serial.println("BLE Failed");
  }

  BLE.setDeviceName("m3xBLE");
  BLE.setLocalName("m3xBLE");

  MessageService.addCharacteristic(commandData);
  MessageService.addCharacteristic(responseData);

  BLE.addService(MessageService);
  BLE.advertise();
  Serial.println("\nNano is waiting for connections...");
}

void loop() {
  BLEDevice central = BLE.central();

  if(central && central.connected()) {
    //Serial.print("Connected to device: ");
    //Serial.println(central.address());
    if(commandData.valueUpdated()) {
      byte byteData[15];
      commandData.readValue(byteData, 15);
      String message = String((char *)byteData);
      if(message == "ON") {
        Serial.println("ON");
        status = true;
        commandData.writeValue("STARTING");
      } else if(message == "OFF") {
        status = false;
        commandData.writeValue("STOPPING");
      } else if(message == "STATUS") {
        String Data;
        if(status) {
          Data = "ACTIVE";
        } else {
          Data = "NOT ACTIVE";
        }
        responseData.writeValue(Data);
      } else {
        commandData.writeValue("NON VALID");
      }
    }

  } else if(!central.connected()) {
    Serial.print("Disconnected from Device ");
    //Serial.println(central.address());
  }

}
