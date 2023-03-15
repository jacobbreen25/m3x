#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <string>
#include <simpleble/Adapter.h>
#include <simpleble/Peripheral.h>
#include <simpleble/Service.h>
#include <simpleble/Characteristic.h>
#include <simpleble/Descriptor.h>

using namespace std::chrono_literals;

int main(int argc, char* argv[]){
   if (!SimpleBLE::Adapter::bluetooth_enabled()) {
      std::cout << "Bluetooth is not enabled" << std::endl;
      return 1;
   }

   auto adapter_optional = SimpleBLE::Adapter::get_adapters();
   if (adapter_optional.empty()) {
      std::cout << "No Bluetooth adapters found" << std::endl;
      return 1;
   }
   // std::cout << adapter_optional.size() << std::endl;

   // Use the first adapter
   auto adapter = adapter_optional[0];

   std::vector<SimpleBLE::Peripheral> peripherals;

   adapter.set_callback_on_scan_found([&](SimpleBLE::Peripheral peripheral) {
      peripherals.push_back(peripheral);
   });
   adapter.set_callback_on_scan_start([]() { std::cout << "Peripheral Scan has begun." << std::endl; });
   adapter.set_callback_on_scan_stop([]() { std::cout << "Peripheral Scan has ended." << std::endl; });
   adapter.scan_for(2000);

   SimpleBLE::Peripheral device;
   for(auto peripheral : peripherals) {
      std::cout << peripheral.identifier() << std::endl;
      if(peripheral.identifier() == "m3xBLE") {
         device = peripheral;
         break;
      }
   }
   device.connect();
   std::pair<SimpleBLE::BluetoothUUID, SimpleBLE::BluetoothUUID> uuids;
   for (auto service : device.services()) {
      for (auto characteristic : service.characteristics()) {
         if(service.uuid() == "e54aa130-bdbf-11ed-a901-0800200c9a66" && characteristic.uuid() == "e54aa131-bdbf-11ed-a901-0800200c9a66") {
            std::cout << service.uuid() << " : " << characteristic.uuid() << std::endl;
            uuids.first = service.uuid();
            uuids.second = characteristic.uuid();
         }
      }
   }
   //std::string data = "TEST";
   
   SimpleBLE::ByteArray responseArray;
   for(size_t i = 0; i < 5; i++) {
      responseArray = device.read(uuids.first, uuids.second);
      std::this_thread::sleep_for(50ms);
   }
   std::string responseData = responseArray.c_str();
   std::cout << responseData << std::endl;

   SimpleBLE::ByteArray data("ON");
   device.write_request(uuids.first, uuids.second, data);

   std::this_thread::sleep_for(50ms);
   for(size_t i = 0; i < 5; i++) {
      responseArray = device.read(uuids.first, uuids.second);
      std::this_thread::sleep_for(50ms);
   }
   responseData = responseArray.c_str();
   std::cout << responseData << std::endl;
   device.disconnect();
}